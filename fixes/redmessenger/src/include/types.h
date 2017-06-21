#ifndef TYPES_H
#define TYPES_H 1

typedef enum {
	OK = 0,
	FAIL = 1,
	SYNTAX = 2
} status_t;

typedef enum {
	false = 0,
	true = !false
} bool;

#endif
