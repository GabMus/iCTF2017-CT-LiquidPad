#include <command.h>

#include "command.gen"
#include <types.h>
#include <utils.h>

#include <readline/readline.h>
#include <stdio.h>


status_t cmdInit() {
	int i;
	for(i=0;i<CMD_LENGTH;i++) {
		status_t status = cmd_list[i]->init();
		if(status != OK)
			return status;
	}
	return OK;
}

void cmdHelp(const char *name) {
	int p = getCommand(name);
	if (p == -1)
		printf("No such command: %s\n", name);
	else
		printf("%s\n", cmd_list[p]->help);
}

int getCommand(const char *name) {
	int i;
	for(i=0;i<CMD_LENGTH;i++) {
		if(strcmp(cmd_list[i]->name, name) == 0) {
			return i;
		}
	}
	return -1;
}

status_t cmdExec(int argc, char *argv[]) {
	if (argc > 0) {
		int cmd = getCommand(argv[0]);
		if (cmd == -1) {
			printf("No such command: %s\n", argv[0]);
			return FAIL;
		} else
			return cmd_list[cmd]->exec(argc, argv);

	} else {
		return FAIL;
	}
}

status_t cmdFree() {
	int i;
	for(i=0;i<CMD_LENGTH;i++) {
		status_t status = cmd_list[i]->free();
		if(status != OK)
			return status;
	}
	return OK;
}

status_t cmdList() {
	int i;
	for(i=0;i<CMD_LENGTH;i++) {
		printf("%s  ", cmd_list[i]->name);
	}
	printf("\n");
	return OK;
}

char *commandGenerator (const char *text, int state) {
	static int listIndex, len;

	if (!state) {
		listIndex = 0;
		len = strlen(text);
	} else
		listIndex++;
	
	while (listIndex < CMD_LENGTH) {
		if (strncmp(cmd_list[listIndex]->name, text, len) == 0)
			return strDuplicate(cmd_list[listIndex]->name);
		listIndex++;
	}


	return (char *)NULL;
}

char **commandCompletion (const char *text, int start, int end) {
	char **matches;

	matches = (char **)NULL;

	if (start == 0)
		matches = rl_completion_matches(text, commandGenerator);

	return (matches);
}

