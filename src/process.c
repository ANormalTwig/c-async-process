#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include <time.h>

#include "process.h"

#define PROCESS_READ_BUFSIZE 1024

process *process_create(char **args) {
	int inpipe[2];
	pipe(inpipe);

	int outpipe[2];
	pipe(outpipe);

	int pid = fork();
	if (pid == 0) {
		dup2(inpipe[0], STDIN_FILENO);
		dup2(outpipe[1], STDOUT_FILENO);
		dup2(outpipe[1], STDERR_FILENO);

		close(inpipe[1]); // Close write end in inpipe
		close(outpipe[0]); // Close read end of outpipe

		execvp(args[0], args);
		exit(1);
	}
	close(inpipe[0]); // Close read end in inpipe
	close(outpipe[1]); // Close write end of outpipe

	// fcntl(inpipe[1], O_NONBLOCK);
	fcntl(outpipe[0], O_NONBLOCK);

	process *proc = malloc(sizeof(process));
	proc->pid = pid;
	proc->input = inpipe[1];
	proc->output = outpipe[0];

	setpgid(proc->pid, proc->pid);
	return proc;
}

char read_buf[PROCESS_READ_BUFSIZE];
void process_poll(process *proc) {
	int byteCount;
	do {
		memset(&read_buf, 0, PROCESS_READ_BUFSIZE);
		byteCount = read(proc->output, &read_buf, PROCESS_READ_BUFSIZE - 1);
		if (proc->on_data) {
			proc->on_data(read_buf);
		}
		proc->on_data(read_buf);
	} while(byteCount > 0);
	if (byteCount == -1)
		fprintf(stderr, "Error reading process stdout.");

	int status;
	int p = waitpid(proc->pid, &status, WNOHANG);
	if (p == proc->pid) {
		proc->exitted = 1;
		if (proc->on_exit)
			proc->on_exit(status);
	}
}

inline int process_write(process *proc, const void *buf, size_t n) {
	return write(proc->input, buf, n);
}

int process_close(process *proc) {
	kill(proc->pid, SIGKILL);
	int status;
	waitpid(proc->pid, &status, 0);

	close(proc->input);
	close(proc->output);

	free(proc);

	return status;
}

