#define _XOPEN_SOURCE 700
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "blog.h"

#include <errno.h>

int handle_post(void)
{
	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/plain\r\n\r\n");

	read_post_data();
	if (!authenticate(find_post_data("username"), find_post_data("password"))) {
		printf("Status 403 Forbidden\r\n");
		printf("Content-Type: text/plain\r\n\r\n");
		puts("Incorrect username or password. Go back and try again.");
		return 0;
	}

	int blogdir = open(DATA_DIRECTORY, O_DIRECTORY);
	if (blogdir == -1) {
		printf("open(%s): %s\n", DATA_DIRECTORY, strerror(errno));
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
	char uri[FILENAME_MAX] = { 0 };
	snprintf(uri, sizeof(uri), "%s/%s", ymd, title);

	int newpost = openat(blogdir, uri, O_WRONLY | O_CREAT, 0644);
	if (newpost == -1) {
		printf("open(%s): %s\n", uri, strerror(errno));
		return 0;
	}

	char *body = find_post_data("body");
	if (body == NULL) {
		puts("body is null?!");
	}

	if (write(newpost, body, strlen(body)) != strlen(body)) {
		printf("write: %s\n", strerror(errno));
		return 0;
	}

	close(newpost);
	close(blogdir);

	printf("Status: 302 Found\r\n");
	printf("Location: http%s://%s/%s\r\n\r\n", getenv("HTTPS") ? "s" : "", getenv("HTTP_HOST"), uri);
	return 0;
}

int main(void)
{
	char *method = getenv("REQUEST_METHOD");
	if (!strcmp(method, "POST")) {
		return handle_post();
	}

	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/html\r\n\r\n");

	puts("<!DOCTYPE html>");
	puts("<html lang=\"en\">");
	puts("<meta charset=\"utf-8\">");
	puts("<title>Jakob Kaivo/blog</title>");
	puts("<link type=\"shortcut icon\" href=\"/icon.png\">");
	puts("<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">");
	puts("</head>");
	puts("<body>");
	printf("<form method=\"POST\" action=\"%s\">\n", getenv("DOCUMENT_URI"));
	puts("<input type=\"text\" name=\"username\" placeholder=\"username\" size=\"80\"><br>");
	puts("<input type=\"password\" name=\"password\" placeholder=\"password\" size=\"80\"><br>");
	puts("<input type=\"text\" name=\"title\" placeholder=\"title\" size=\"80\"><br>");
	puts("<textarea name=\"body\" cols=\"80\" rows=\"24\"></textarea><br>");
	puts("<input type=\"submit\">");
	puts("</form>");
	puts("</body>");
	puts("</html>");

	return 0;
}
