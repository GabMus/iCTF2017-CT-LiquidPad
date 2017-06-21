#include <command.h>
#include <user.h>

#include <message.h>
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
#include <sys/mman.h>

int getUserFile(char *user, int flags) {
	char fileName[MAX_USERNAME_LEN + sizeof(ACCOUNT_DIR)];
	char *iter;
	
	snprintf(fileName, sizeof(fileName), "%s%s", ACCOUNT_DIR, user);
	fileName[sizeof(fileName) - 1] = '\0';
	for (iter = fileName; iter[0] != '\0'; iter++)
		if (iter[0] == '.' && iter[1] == '.')
			return -2;

	return open(fileName, flags, 0440);
}

#ifndef NO_CREATE
const command create_command = {
	create_init,
	create_exec,
	create_free,
	"CREATE",
	"CREATE username password\n"
	"create a new user with the provided username and password. Username must "
	"be smaller than " STR(MAX_USERNAME_LEN) " characters\n"
};
#endif

#ifndef NO_NEW
const command new_command = {
	create_init,
	create_exec,
	create_free,
	"NEW",
	"NEW username password\n"
	"create a new user with the provided username and password. Username must "
	"be smaller than " STR(MAX_USERNAME_LEN) " characters\n"
};
#endif

status_t create_init() {
	struct stat st;
	if (stat(ACCOUNT_DIR, &st) == -1 && errno == ENOENT) {
		return mkdir(ACCOUNT_DIR, 0770) != -1 ? OK : FAIL;
	}
	return OK;
}

void createUserInbox(char *user) {
	char dirName[MAX_USERNAME_LEN + sizeof(MESSAGE_DIR)];

	snprintf(dirName, sizeof(dirName), "%s%s", MESSAGE_DIR, user);
	dirName[sizeof(dirName) - 1] = '\0';
	mkdir(dirName, 0770);
}

status_t create_exec(int argc, char *argv[]) {
	int fd, len;

	if (argc == 3) {
		if ((fd = getUserFile(argv[1], O_WRONLY | O_CREAT | O_EXCL)) > 0) {
			len = strlen(argv[2]);
			if (len > MAX_PASSWD_LEN) {
				len = MAX_PASSWD_LEN;
				argv[2][len] = '\0';
				printf("Password truncated to '%s'\n", argv[2]);
			}
			write(fd, argv[2], len);
			close(fd);
			createUserInbox(argv[1]);
			printf("User created\n");
			return OK;
		} else if (fd == -2) {
			printf("Invalid username\n");
			return FAIL;
		} else {
			printf("User %s already exists\n", argv[1]);
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t create_free() {
	return OK;
}

#ifndef NO_LOGIN
const command login_command = {
	login_init,
	login_exec,
	login_free,
	"LOGIN",
	"LOGIN username password\n"
	"connect as user 'username' using the password\n"
};
#endif

status_t login_init() {
	return OK;
}

void setCurrentUser(char *user) {
	int len = strlen(user);
	if (currentUser != NULL)
		free(currentUser);
	currentUser = (char*) malloc((len + 1) * sizeof(char));
	strncpy(currentUser, user, len);
	currentUser[len] = '\0';
}

status_t login_exec(int argc, char *argv[]) {
	int fd;
	char *map;

	if (argc == 3) {
		if ((fd = getUserFile(argv[1], O_RDONLY)) != -1) {
			map = mmap(0, MAX_PASSWD_LEN, PROT_READ, MAP_PRIVATE, fd, 0);
			if (map == MAP_FAILED) {
				close(fd);
				return FAIL;
			}

			if (strncmp(map, argv[2], MAX_PASSWD_LEN) == 0) {
				setCurrentUser(argv[1]);
				printf("You're now logged in\n");
			} else
				printf("Wrong password\n");

			munmap(map, MAX_PASSWD_LEN);
			close(fd);
			return OK;
		} else if (fd == -2) {
			printf("Invalid username\n");
			return FAIL;
		} else {
			printf("User %s doesn't exist\n", argv[1]);
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t login_free() {
	if (currentUser != NULL)
		free(currentUser);
	return OK;
}


#ifndef NO_USERS
const command users_command = {
	users_init,
	users_exec,
	users_free,
	"USERS",
	"USERS\n"
	"lists all users. Requires to be authenticated\n"
};
#endif

status_t users_init() {
	return OK;
}

status_t users_exec(int argc, char *argv[]) {
	if (argc == 1) {
		if (currentUser != NULL) {
			DIR *d;
			struct dirent *dir;
			d = opendir(ACCOUNT_DIR);
			if (d) {
				while ((dir = readdir(d)) != NULL) {
					if (dir->d_name[0] != '.')
						printf("%s  ", dir->d_name);
				}
				printf("\n");
				closedir(d);
				return OK;
			}
			return FAIL;
		} else {
			printf("You must be authenticated\n");
			return FAIL;
		}
	} else {
		return SYNTAX;
	}
}

status_t users_free() {
	return OK;
}
