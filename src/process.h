#include <stddef.h>

typedef struct process {
	int pid;
	int input;
	int output;

	int exitted;

	void (*on_data)(char *data);
	void (*on_exit)(int code);
} process;

process *process_create(char **args);

void process_poll(process *proc);

inline int process_write(process *proc, const void *buf, size_t n);

int process_close(process *proc);

