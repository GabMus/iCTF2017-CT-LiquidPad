#ifndef MESSAGE_CMD_H
#define MESSAGE_CMD_H 1

#include <types.h>

#ifndef MESSAGE_DIR
#define MESSAGE_DIR "messages/"
#endif

#define MAX_MESSAGE_ID 99

#define MAX_SUBJECT_LEN 256
#define MAX_MESSAGE_LEN 1024

int getMessageFile(char *user, char *id, int flags);

status_t send_init();
status_t send_exec(int argc, char *argv[]);
status_t send_free();

status_t read_init();
status_t read_exec(int argc, char *argv[]);
status_t read_free();

status_t delete_init();
status_t delete_exec(int argc, char *argv[]);
status_t delete_free();

status_t messages_init();
status_t messages_exec(int argc, char *argv[]);
status_t messages_free();

#endif
