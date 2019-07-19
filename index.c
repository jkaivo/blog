#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "blog.h"

static void insert_into(int blogdir, char *index_dir, const char *uri, const char *title)
{
	char index_path[FILENAME_MAX] = { 0 };
	snprintf(index_path, sizeof(index_path), "%s/index.html", index_dir);

	char new_path[FILENAME_MAX] = { 0 };
	snprintf(new_path, sizeof(new_path), "%s.new", index_path);
	int new_index = openat(blogdir, new_path, O_WRONLY | O_CREAT, 0644);

	dprintf(new_index, HTML_HEAD, "index");
	dprintf(new_index, "<a href=\"/%s\">%s</a><br>", uri, title);

	/* add old entries */
	int old_index = openat(blogdir, index_path, O_RDONLY);
	if (old_index != -1) {
		struct stat st;
		fstat(old_index, &st);
		char *buf = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, old_index, 0);
		if (buf != MAP_FAILED) {
			char *a = strstr(buf, "<a");
			char *add = strstr(a, "<address");
			write(new_index, a, add - a);
			munmap(buf, st.st_size);
		}
		close(old_index);
	}

	dprintf(new_index, HTML_TAIL, current_year());

	close(new_index);

	renameat(blogdir, new_path, blogdir, index_path);
}

void add_to_index(const char *path, const char *title)
{
	int blogdir = open(DATA_DIRECTORY, O_DIRECTORY);
	if (blogdir == -1) {
		return;
	}

	char *dir = strdup(path);
	if (dir == NULL) {
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
