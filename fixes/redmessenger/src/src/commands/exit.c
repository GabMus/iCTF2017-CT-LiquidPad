#include <exit.h>
#include <command.h>

#include <main.h>
#include <types.h>

const command exit_command = {
	exit_init,
	exit_exec,
	exit_free,
	"EXIT",
	"EXIT\n"
	"quits the application\n"
};

status_t exit_init() {
	return OK;
}

status_t exit_exec(int argc, char *argv[]) {
	if (argc == 1) {
		exitProgram(0);
		return OK;
	} else {
		return SYNTAX;
	}
}

status_t exit_free() {
	return OK;
}

