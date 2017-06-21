#ifndef CREATE_CMD_H
#define CREATE_CMD_H 1

#include <types.h>

#ifndef ACCOUNT_DIR
#define ACCOUNT_DIR "users/"
#endif

#define MAX_USERNAME_LEN 64
#define MAX_PASSWD_LEN 256

char *currentUser;

int getUserFile(char *user, int flags);

status_t create_init();
status_t create_exec(int argc, char *argv[]);
status_t create_free();

status_t login_init();
status_t login_exec(int argc, char *argv[]);
status_t login_free();

status_t users_init();
status_t users_exec(int argc, char *argv[]);
status_t users_free();

#endif
