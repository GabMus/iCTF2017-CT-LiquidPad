#include <command.h>
#include <message.h>

#include <user.h>
#include <types.h>
#include <utils.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define string_t(max) \
	struct { \
		char str[max]; \
		int len; \
	}

#define DIR_PATH_LEN 	MAX_USERNAME_LEN + sizeof(MESSAGE_DIR)
#define FILE_NAME_LEN 	MAX_USERNAME_LEN + sizeof(STR(MAX_MESSAGE_ID)) + 1
#define FILE_PATH_LEN 	DIR_PATH_LEN + FILE_NAME_LEN

#define message_dir(buffer, user) \
	char buffer[DIR_PATH_LEN]; \
	snprintf(buffer, sizeof(buffer), "%s%s", MESSAGE_DIR, user); \
	buffer[sizeof(buffer) - 1] = '\0';

#define message_path(buffer, user, id) \
	char buffer[FILE_PATH_LEN]; \
	snprintf(buffer, sizeof(buffer), "%s%s/%s", MESSAGE_DIR, user, id); \
	buffer[sizeof(buffer) - 1] = '\0';


bool correctPath(const char* path) {
	const char *iter;

	for (iter = path; iter[0] != '\0'; iter++)
		if (iter[0] == '.' && iter[1] == '.')
			return false;
	return true;
}

int getMessageFile(char *user, char *id, int flags) {
	if (strlen(id) >= FILE_NAME_LEN)
		id[FILE_NAME_LEN - 1] = '\0';
	if (!correctPath(id))
		return -2;

	message_path(fileName, user, id);

	return open(fileName, flags, 0660);
}

int createMessageFile(char *user) {
	int i, fd;
	char id[FILE_NAME_LEN] = {0}; 
	char *id_start;

	if (strlen(user) > MAX_USERNAME_LEN)
		user[MAX_USERNAME_LEN] = '\0';
	if (!correctPath(user))
		return -2;

	strncpy(id, currentUser, MAX_USERNAME_LEN);
	id_start = id + strlen(id);
	id_start[0] = '_';
	id_start++;
	for (i = 0; i <= MAX_MESSAGE_ID; i++) {
		snprintf(id_start, sizeof(STR(MAX_MESSAGE_ID)), "%d", i);
 		if ((fd = getMessageFile(user, id, O_WRONLY|O_CREAT|O_EXCL)) != -1) {
			return fd;
		}
	}
	return -3;
}

#ifndef NO_SEND
const command send_command = {
	send_init,
	send_exec,
	send_free,
	"SEND",
	"SEND username\n"
	"sends a message to user 'username'\n"
};
#endif

int readInput(char *dest, int max) {
	int i = 0;
	char c;
	while (i < max && read(STDIN_FILENO, &c, 1) == 1) {
		dest[i++] = c;
		if (c == '\n')
			break;
	}
	dest[i-1] = '\n';
	dest[i] = '\0';
	if (c != '\n')
		while (read(STDIN_FILENO, &c, 1) == 1 && c != '\n');
	return i;
}

void writeMessage(int fd) {
	string_t(MAX_SUBJECT_LEN) subj;
	string_t(MAX_MESSAGE_LEN) mess;
	int ret;
	char *ptr;
	
	printf("Subject: ");
	subj.len = readInput(subj.str, sizeof(subj.str));
	printf("Body: End data with <LF>.<LF>\n");
	mess.len = 0;
	ptr = mess.str;
	while (mess.len < sizeof(mess.str) -1) {
		ret = readInput(ptr, sizeof(mess.str) - mess.len);
		mess.len += ret;
		ptr += ret;
		if (mess.len >= 3 && strcmp("\n.\n", mess.str + mess.len -3) == 0) {
			mess.len -= 2;
			break;
		}
	}
	mess.str[mess.len] = '\0';
	
	#define MAIL_FROM "From: "
	#define SUBJECT "Subject: "
	write(fd, MAIL_FROM, sizeof(MAIL_FROM) - 1);
	write(fd, currentUser, strlen(currentUser));
	write(fd, "\n", 1);
	write(fd, SUBJECT, sizeof(SUBJECT) - 1);
	write(fd, subj.str, subj.len);
	write(fd, "\n", 1);
	write(fd, mess.str, mess.len);
	#undef MAIL_FROM
	#undef SUBJECT
}

status_t send_init() {
	struct stat st;
	if (stat(MESSAGE_DIR, &st) == -1 && errno == ENOENT) {
		return mkdir(MESSAGE_DIR, 0770) != -1 ? OK : FAIL;
	}
	return OK;
}

status_t send_exec(int argc, char *argv[]) {
	int fd;

	if (argc == 2) {
		if (currentUser != NULL) {
			if ((fd = createMessageFile(argv[1])) > 0) {
				writeMessage(fd);
				close(fd);
				return OK;
			} else if (fd == -2) {
				printf("Invalid username\n");
				return FAIL;
			} else if (fd == -3) {
				printf("Can't send more messages to the user\n");
				return FAIL;
			} else {
				printf("Error\n");
				return FAIL;
			}
		} else {
			printf("You must be authenticated\n");
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t send_free() {
	return OK;
}


#ifndef NO_READ
const command read_command = {
	read_init,
	read_exec,
	read_free,
	"READ",
	"READ message_id\n"
	"reads the message\n"
};
#endif

status_t read_init() {
	return OK;
}

status_t read_exec(int argc, char *argv[]) {
	int fd;
	char buff[256] = {0};
		
	if (argc == 2) {
		if (currentUser != NULL) {
			if((fd = getMessageFile(currentUser, argv[1], O_RDONLY)) > 0) {
				printf("Message id: %s\n", argv[1]);
				while (read(fd, buff, sizeof(buff))) {
					printf("%s",buff);
				}
				close(fd);
		
				return OK;
			} else {
				printf("Invalid message_id\n");
				return FAIL;
			}
		} else {
			printf("You must be authenticated\n");
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t read_free() {
	return OK;
}


#ifndef NO_DELETE
const command delete_command = {
	delete_init,
	delete_exec,
	delete_free,
	"DELETE",
	"DELETE message_id\n"
	"deletes the message\n"
};
#endif

status_t delete_init() {
	return OK;
}

status_t delete_exec(int argc, char *argv[]) {
	if (argc == 2) {
		if (currentUser != NULL) {
			message_path(filename, currentUser, argv[1]);
			if (correctPath(filename)) {
				if (unlink(filename) == 0)
					return OK;
			}
			printf("Invalid message_id\n");
			return FAIL;
		} else {
			printf("You must be authenticated\n");
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t delete_free() {
	return OK;
}


#ifndef NO_MESSAGES
const command messages_command = {
	messages_init,
	messages_exec,
	messages_free,
	"MESSAGES",
	"MESSAGES\n"
	"lists all received messages\n"
};
#endif

status_t messages_init() {
	return OK;
}

status_t messages_exec(int argc, char *argv[]) {
	DIR *dir;
	FILE *file;
	struct dirent* ent;
	size_t len = 0;
	ssize_t read;
	char *line = NULL;
	bool first = true;
	
	if (argc == 1) {
		if (currentUser != NULL) {
			message_dir(messagesDir, currentUser);
			
			if ((dir = opendir(messagesDir)) != NULL) {
				while ((ent = readdir(dir)) != NULL) {
					if(ent->d_name[0] != '.') {
						message_path(filename, currentUser, ent->d_name);
						
						if (first)
							first = false;
						else 
							printf("--------------------------------\n");

						file = fopen(filename, "r");
						printf("Message id: %s\n", ent->d_name);
						while ((read = getline(&line, &len, file)) != -1) {
							printf("%s", line);
							
							if(line[0] == 'S' && line[1] == 'u') break;
						}
						fclose(file);
					}
				}
				if (line != NULL)
					free(line);
				closedir (dir);
				
				return OK;
			} else {
				printf("Couldn't open your INBOX\n");
				return FAIL;
			}
		} else {
			printf("You must be authenticated\n");
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t messages_free() {
	return OK;
}

