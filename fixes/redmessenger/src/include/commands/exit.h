#ifndef EXIT_CMD_H
#define EXIT_CMD_H 1

#include <types.h>

status_t exit_init();
status_t exit_exec(int argc, char *argv[]);
status_t exit_free();

#endif
