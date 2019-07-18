#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "blog.h"

int authenticate(const char *username, const char *password)
{
	int authenticated = 0;

	char *pwline = NULL;
	FILE *pwfile = fopen(PASSWORD_FILE, "r");

	if (!pwfile) {
		goto end;
	}

	size_t ulen = strlen(username);

	while (pwline == NULL) {
		char *line = NULL;
		size_t n = 0;

		if (getline(&line, &n, pwfile) == -1) {
			goto end;
		}

		if (strncmp(username, line, ulen) == 0 && line[ulen] == ':') {
			pwline = line;
			break;
		}

		free(line);
	}

	if (pwline == NULL) {
		goto end;
	}

	char *stored_password = pwline + ulen + 1;
	char *match = crypt(password, stored_password);
	if (!strncmp(match, stored_password, strlen(match))) {
		authenticated = 1;
	}

end:
	if (pwline) {
		free(pwline);
	}

	if (pwfile) {
		fclose(pwfile);
	}
	return authenticated;
}
