#ifndef BLOG_H
#define BLOG_H

#define PASSWORD_FILE	"/blog/password"
#define DATA_DIRECTORY	"/jakob"

void read_post_data(void);
char *find_post_data(char *key);

int authenticate(const char *username, const char *password);

int handle_post(void);

int show_entry(const char *path);

void add_to_index(const char *path, const char *title);

#endif
