#ifndef BLOG_H
#define BLOG_H

#define DOCTYPE "<!DOCTYPE html>\n"
#define HTML    "<html lang=\"en\">\n"
#define META    "<meta charset=\"utf-8\">\n"
#define TITLE   "<title>%s</title>\n"
#define ICON    "<link type=\"shortcut icon\" href=\"/icon.png\">\n"
#define STYLE   "<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n"
#define BODY    "</html>\n<body>\n"

#define HTML_HEAD DOCTYPE HTML META TITLE ICON STYLE BODY

#define ADDRESS "<address>Copyright &copy; %1$d <a href=\"/\">%2$s</a> &lt;<a href=\"mailto:%3$s\">%3$s</a>&gt;\n"

#define HTML_TAIL "\n" ADDRESS "</body>\n</html>\n"

void read_post_data(void);
char *find_post_data(char *key);

char *user_name(const char *user);
char *user_email(const char *user);

int handle_post(void);

void add_to_index(const char *user, const char *path, const char *title);

int current_year(void);

#endif
