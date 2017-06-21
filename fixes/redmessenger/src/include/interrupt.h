#ifndef INTERRUPT_H
#define INTERRUPT_H 1

#include <types.h>

#ifndef MAX_OPEN_FD
#define MAX_OPEN_FD 7
#endif

status_t interruptInit();

#endif
