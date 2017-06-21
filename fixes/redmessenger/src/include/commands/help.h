#ifndef HELP_CMD_H
#define HELP_CMD_H 1

#include <types.h>

status_t help_init();
status_t help_exec(int argc, char *argv[]);
status_t help_free();

#endif
