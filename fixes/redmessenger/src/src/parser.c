#include <command.h>
#include <parser.h>
#include <main.h>
#include <types.h>

#include <stdlib.h>
#include <ctype.h>

#ifndef ARGV_MAX_LEN
#define ARGV_MAX_LEN 128
#endif

char *nextArg(char **cmdLine) {
	char c;
	char *ret = NULL;

	while ((c = (*cmdLine)[0]) != 0) {
		if (isspace(c)) {
			if (ret != NULL) {
				(*cmdLine)[0] = 0;
				(*cmdLine)++;
				return ret;
			}
			break;
		} else {
			if (ret == NULL)
				ret = *cmdLine;
			(*cmdLine)++;
		}
	}
	return ret;
}

status_t execute(char **cmd) {
	int argc = 0;
	char *argv[ARGV_MAX_LEN];
	char *arg;

	char **cmdLine = cmd;

	while (argc < ARGV_MAX_LEN && (arg = nextArg(cmdLine)) != NULL) {
		argv[argc] = arg;
		argc++;
	}
	argv[argc] = NULL;

	return cmdExec(argc, argv);
}

