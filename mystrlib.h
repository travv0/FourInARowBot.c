#include <stdio.h>

char *trim(char *str)
{
	char *end;

	while (isspace(*str))
		str++;

	if (*str == 0)
		return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	*(end + 1) = 0;

	return str;
}

// ssize_t readline(char **lineptr, FILE *stream) {
// 	int c;

// 	c = getc(stream)
// }
