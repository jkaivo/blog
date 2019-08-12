#define _XOPEN_SOURCE 700
#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blog.h"

#define HSIZE 4 /* username, password, title, body */

static void add_data(char *buf)
{
	char *value = strchr(buf, '=');
	*value = '\0';
	value++;

	char *key = strdup(buf);
	char *data = strdup(value);

	ENTRY e = {
		.key = key,
		.data = data
	};
	hsearch(e, ENTER);
}

void read_post_data(void)
{
	char *content_length = getenv("CONTENT_LENGTH");
	if (!content_length) {
		return;
	}

	int cl = atoi(content_length);
	if (!cl) {
		return;
	}

	if (!hcreate(HSIZE)) {
		return;
	}

	char *buf = malloc(cl + 1);
	if (!buf) {
		return;
	}

	int pos = 0;
	int c;
	buf[0] = '\0';

	while (pos < cl && (c = getchar()) != EOF) {
		if (c == '&') {
			add_data(buf);
			pos = 0;
		} else if (c == '+') {
			buf[pos] = ' ';
			buf[++pos] = '\0';
		} else if (c == '%') {
			char hex[3] = { 0, 0, 0 };
			hex[0] = getchar();
			hex[1] = getchar();
			buf[pos] = strtol(hex, NULL, 16);
			buf[++pos] = '\0';
		} else {
			buf[pos] = c;
			buf[++pos] = '\0';
		}
	}

	if (pos != 0) {
		add_data(buf);
	}

	free(buf);
}

char *find_post_data(char *key)
{
	ENTRY e = {
		.key = key
	};
	ENTRY *p = hsearch(e, FIND);
	if (p && p->data) {
		return p->data;
	}

	return NULL;
}
