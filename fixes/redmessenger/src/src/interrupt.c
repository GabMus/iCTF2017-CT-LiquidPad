#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

#include <interrupt.h>

#include <main.h>

sigjmp_buf ctrlc_buf;

static void interrupt(int signo) {
	int fd;
	for (fd = 3; fd < 3 + MAX_OPEN_FD; fd++)
		close(fd);

	printf("\n");
	siglongjmp(ctrlc_buf, 1);
}

status_t interruptInit() {
	if(signal(SIGINT, interrupt) == SIG_ERR) {
		printError(NULL, NULL, "Error in signal handling. It may cause a crash");
		return FAIL;
	}
	return OK;
}


