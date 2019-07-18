#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern char **environ;

int handle_post(void)
{
	printf("Status: 301 Found\r\n");
	printf("Location: http%s://%s%s\r\n\r\n", getenv("HTTPS") ? "s" : "", getenv("HTTP_HOST"), getenv("DOCUMENT_URI"));
	return 0;
}

int show_entry(const char *path)
{
	printf("Status: 200 OK\r\n");
	printf("Content-Type: text/plain\r\n\r\n");
	puts(path);
	return 0;
}

int main(void)
{
	char *method = getenv("REQUEST_METHOD");
	if (!strcmp(method, "POST")) {
		return handle_post();
	}

	char *path_info = getenv("PATH_INFO");
	if (path_info && *path_info && strcmp(path_info, "/")) {
		return show_entry(path_info);
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
	puts("Username: <input type=\"text\" name=\"username\"><br>");
	puts("Password: <input type=\"password\" name=\"password\"><br>");
	puts("Title: <input type=\"text\" name=\"title\"><br>");
	puts("<textarea name=\"body\"></textarea><br>");
	puts("<input type=\"submit\">");
	puts("</form>");

	puts("<pre>");
	for (char **e = environ; e && *e; e++) {
		puts(*e);
	}
	puts("</pre>");

	puts("</body>");
	puts("</html>");

	return 0;
}
