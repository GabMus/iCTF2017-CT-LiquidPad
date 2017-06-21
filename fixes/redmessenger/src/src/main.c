#include <stdlib.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>

#include <main.h>

#include <command.h>
#include <interrupt.h>
#include <parser.h>
#include <types.h>
#include <utils.h>
 
sigjmp_buf ctrlc_buf;

extern command help_command;

int main()
{
	setbuf(stdout, NULL);

	init_readline();

	cmdInit();
	
	interruptInit();

	char* input = NULL;
	while ( sigsetjmp( ctrlc_buf, 1 ) != 0 );

	if (input != NULL) {
		free(input);
		input = NULL;
	}

	for(;;) {
		
		input = readline(">>> ");

		if (!input) {
			printf("\n");
			free(input);
			break;
		}

		char *line = stripWhite(input);

		if (*line) {
			char *expansion;
			int result;

			result = history_expand(line, &expansion);
			if (result)
				printError("readline", NULL, expansion);

			if (result < 0 || result == 2) {
				free(expansion);
				free(input);
				input = NULL;
				continue;
			}

			add_history(expansion);
			line = expansion;

			status_t ret = execute(&line);
			switch (ret) {
				case SYNTAX:
					printf("Invalid syntax. Use %s for information\n",
							help_command.name);
					break;
				default:
					break;
			}

			free (expansion);
		}

		free(input);
		input = NULL;
	}
	exitProgram(0);

	return 0;
}

void init_readline() {

	rl_bind_key('\t', rl_complete);

	rl_readline_name = "RedCops";

	rl_attempted_completion_function = commandCompletion;

	using_history();
}

void exitProgram(int errcode){
	cmdFree();

	exit(errcode);
}

void printError(char *cmd, char *issue, char *detail){
	fprintf(stderr, "RedCops: ");
	if (cmd != NULL)
		fprintf(stderr, "%s: ", cmd);
	if (issue != NULL)
		fprintf(stderr, "%s: ", issue);
	fprintf(stderr, "%s.\n", detail);
}

