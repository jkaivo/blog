#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "blog.h"

#include <errno.h>

static void insert_into(int blogdir, char *index_dir, const char *uri, const char *title)
{
	char index_path[FILENAME_MAX] = { 0 };
	snprintf(index_path, sizeof(index_path), "%s/index.html", index_dir);

	/* check for exisint index */
	int old_index = openat(blogdir, index_path, O_RDONLY);

	char new_path[FILENAME_MAX] = { 0 };
	snprintf(new_path, sizeof(new_path), "%s.new", index_path);
	int new_index = openat(blogdir, new_path, O_WRONLY | O_CREAT, 0644);

	dprintf(new_index, HTML_HEAD, "index");
	dprintf(new_index, "<a href=\"/%s\">%s</a><br>", uri, title);

	if (old_index != -1) {
		/* add old entries */
		close(old_index);
	}

	dprintf(new_index, HTML_TAIL, current_year());

	close(new_index);

	renameat(blogdir, new_path, blogdir, index_path);
}

void add_to_index(const char *path, const char *title)
{
	/*
	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/plain\r\n\r\n");
	*/

	int blogdir = open(DATA_DIRECTORY, O_DIRECTORY);
	if (blogdir == -1) {
		printf("open(DATA_DIRECTORY): %s\n", strerror(errno));
		return;
	}

	char *dir = strdup(path);
	if (dir == NULL) {
		printf("strdup(path): %s\n", strerror(errno));
		return;
	}

	char *day = dirname(dir);
	insert_into(blogdir, day, path, title);
	char *month = dirname(day);
	insert_into(blogdir, month, path, title);
	char *year = dirname(month);
	insert_into(blogdir, year, path, title);

	free(dir);
	close(blogdir);
}
