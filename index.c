#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "blog.h"

#include <errno.h>

void add_to_index(const char *path, const char *title)
{
	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/plain\r\n\r\n");

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

	char index_path[FILENAME_MAX] = { 0 };
	snprintf(index_path, sizeof(index_path), "%s/index.html", dirname(dir));

	/* open index.html */
	int index_fd = openat(blogdir, index_path, O_RDWR | O_CREAT, 0644);
	if (index_fd == -1) {
		printf("open(index_path): %s\n", strerror(errno));
		return;
	}

	/* find the end of text */
	off_t eot = 0;

	if (eot == 0) {
		dprintf(index_fd, HTML_HEAD, "index");
	} else {
		ftruncate(index_fd, eot);
	}

	/* add link */
	dprintf(index_fd, "<a href=\"/%s\">%s</a><br>", path, title);

	/* (re-)add footer */
	dprintf(index_fd, HTML_TAIL, current_year());

	close(index_fd);


	free(dir);
	close(blogdir);
}
