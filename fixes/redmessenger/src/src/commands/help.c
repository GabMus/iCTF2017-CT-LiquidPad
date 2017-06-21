#include <command.h>
#include <help.h>

#include <types.h>

const command help_command = {
	help_init,
	help_exec,
	help_free,
	"HELP",
	"HELP [command]\n"
	"get help on a command or list commands\n"
};

status_t help_init() {
	return OK;
}

status_t help_exec(int argc, char *argv[]) {
	if (argc > 2) {
		return SYNTAX;
	} else if (argc == 1) {
		cmdList();
	} else {
		cmdHelp(argv[1]);
	}
	return OK;
}

status_t help_free() {
	return OK;
}
