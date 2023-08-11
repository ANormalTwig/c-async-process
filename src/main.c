#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "process.h"

void server_on_data(char *data) {
	printf("%s", data);
}

void server_on_exit(int status) {
	printf("%d", status);
}

int main() {
	char *args[] = {"./srcds_run", NULL};
	process *proc = process_create(args);
	if (proc->pid == -1) {
		printf("%s", strerror(errno));
		return EXIT_FAILURE;
	}

	proc->on_data = server_on_data;
	proc->on_exit = server_on_exit;

	while (!proc->exitted) {
		process_poll(proc);
	}

	process_close(proc);

	return EXIT_SUCCESS;
}

