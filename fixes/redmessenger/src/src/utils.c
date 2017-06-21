#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include <utils.h>

char *stripWhite(char *string) {
	register char *s, *t;

	for (s = string; isspace(*s); s++)
		;

	if (*s == 0)
		return (s);

	t = s + strlen(s) - 1;
	while (t > s && isspace(*t))
		t--;
	*++t = '\0';

	return s;
}

char *strDuplicate(const char *s) {
  char *r;

  r = malloc(strlen(s) + 1);
  strcpy(r, s);
  return r;
}

