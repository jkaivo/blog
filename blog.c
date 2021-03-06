#define _XOPEN_SOURCE 700
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "blog.h"

int current_year(void)
{
	static int year = 0;
	if (year == 0) {
		time_t t = time(NULL);
		struct tm *tm = localtime(&t);
		year = tm->tm_year + 1900;
	}
	return year;
}

void readfile(const char *path, char *buf, size_t n)
{
	FILE *f = fopen(path, "r");
	fgets(buf, n, f);
	fclose(f);
	size_t len = strlen(buf);
	if (buf[len-1] == '\n') {
		buf[len-1] = '\0';
	}
}

char *user_name(const char *user)
{
	static char name[BUFSIZ] = {0};
	char path[FILENAME_MAX];
	snprintf(path, sizeof(path), "/%s/.name", user);
	readfile(path, name, sizeof(name));
	return name;
}

char *user_email(const char *user)
{
	static char email[BUFSIZ] = {0};
	char path[FILENAME_MAX];
	snprintf(path, sizeof(path), "/%s/.email", user);
	readfile(path, email, sizeof(email));
	return email;
}

int handle_post(void)
{
	char *user = getenv("REMOTE_USER");
	if (!user) {
		printf("Status 403 Forbidden\r\n");
		printf("Content-Type: text/plain\r\n\r\n");
		puts("Incorrect username or password. Go back and try again.");
		return 0;
	}

	read_post_data();

	chdir("/");
	int blogdir = open(user, O_DIRECTORY);
	if (blogdir == -1) {
		return 1;
	}

	time_t now = time(NULL);
	struct tm *tm = localtime(&now);

	char ymd[16] = { 0 };
	strftime(ymd, sizeof(ymd), "%Y", tm);
	mkdirat(blogdir, ymd, 0755);

	strftime(ymd, sizeof(ymd), "%Y/%m", tm);
	mkdirat(blogdir, ymd, 0755);

	strftime(ymd, sizeof(ymd), "%Y/%m/%d", tm);
	mkdirat(blogdir, ymd, 0755);

	char *title = find_post_data("title");
	while (isblank(*title)) {
		title++;
	}
	size_t len = strlen(title);
	while (isblank(title[--len])) {
		title[len] = '\0';
	}

	char uri[FILENAME_MAX] = { 0 };
	char *end = stpcpy(uri, ymd);
	*end++ = '/';
	int dash = 0;
	for (char *f = title; *f != '\0'; f++) {
		if (isalnum(*f)) {
			*end++ = tolower(*f);
			dash = 0;
		} else if (!dash) {
			dash = 1;
			*end++ = '-';
		}
	}
	strcat(uri, ".html");

	int newpost = openat(blogdir, uri, O_WRONLY | O_CREAT, 0644);
	if (newpost == -1) {
		return 1;
	}

	char *body = find_post_data("body");
	if (body == NULL) {
		return 1;
	}

	dprintf(newpost, HTML_HEAD, title);
	dprintf(newpost, "<h1>%s</h1>\n", title);
	if (write(newpost, body, strlen(body)) != strlen(body)) {
		return 1;
	}
	dprintf(newpost, HTML_TAIL, current_year(), user_name(user), user_email(user));

	close(newpost);
	close(blogdir);

	add_to_index(user, uri, title);

	printf("Status: 302 Found\r\n");
	printf("Location: http%s://%s/%s\r\n\r\n", getenv("HTTPS") ? "s" : "", getenv("HTTP_HOST"), uri);
	return 0;
}

int main(void)
{
	char *user = getenv("REMOTE_USER");
	char *method = getenv("REQUEST_METHOD");
	if (!strcmp(method, "POST")) {
		return handle_post();
	}

	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/html\r\n\r\n");

	printf(HTML_HEAD, "new blog entry");

	if (user) {
		printf("<p>Posting as %s</p>\n", user);
	}

	puts("<script type=\"text/javascript\">");
	puts("function update() {");
        puts("\tdocument.getElementById(\"output\").innerHTML = document.getElementById(\"body\").value;");
	puts("\twindow.onbeforeunload = function() {");
	puts("\t\treturn 'Are you sure you want to leave?';");
	puts("\t}");
	puts("}");
	puts("</script>");

	printf("<form method=\"POST\">\n");
	puts("<input type=\"text\" name=\"title\" placeholder=\"title\" size=\"80\"><br>");
	puts("<textarea id=\"body\" name=\"body\" cols=\"80\" rows=\"24\" oninput=\"update();\"><p></p></textarea><br>");
	puts("<input type=\"submit\" value=\"Post\">");
	puts("</form>");
	puts("<div id=\"output\"></div>");

	printf(HTML_TAIL, current_year(), "Jakob Kaivo", "jakob@kaivo.net");

	return 0;
}
