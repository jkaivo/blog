#ifndef BLOG_H
#define BLOG_H

#define PASSWORD_FILE	"/blog/password"
#define DATA_DIRECTORY	"/jakob"

#define DOCTYPE "<!DOCTYPE html>\n"
#define HTML    "<html lang=\"en\">\n"
#define META    "<meta charset=\"utf-8\">\n"
#define TITLE   "<title>%s</title>\n"
#define ICON    "<link type=\"shortcut icon\" href=\"/icon.png\">\n"
#define STYLE   "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n"
#define BODY    "</html>\n<body>\n"

#define HTML_HEAD DOCTYPE HTML META TITLE ICON STYLE BODY

#define ADDRESS "<address>Copyright &copy; %d <a href=\"/\">Jakob Kaivo</a> &lt;<a href=\"mailto:jakob@kaivo.net\">jakob@kaivo.net</a>&gt;\n"

#define HTML_TAIL "\n" ADDRESS "</body>\n</html>\n"

void read_post_data(void);
char *find_post_data(char *key);

int authenticate(const char *username, const char *password);

int handle_post(void);

int show_entry(const char *path);

void add_to_index(const char *path, const char *title);

#endif
