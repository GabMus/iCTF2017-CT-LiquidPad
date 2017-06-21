#ifndef COMMAND_H
#define COMMAND_H 1

#include <types.h>

typedef struct command {
	status_t (* init)();
	status_t (* exec)(int argc, char *argv[]);
	status_t (* free)();
	const char *name;
	const char *help;
} command;


status_t cmdInit();
void cmdHelp(const char *name);
int getCommand(const char *name);
status_t cmdExec(int argc, char *argv[]);
status_t cmdFree();
status_t cmdList();
char **commandCompletion(const char* text, int start, int end);

#endif
