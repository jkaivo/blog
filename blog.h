#ifndef BLOG_H
#define BLOG_H

void read_post_data(void);
char *find_post_data(char *key);

int verify_creds(const char *username, const char *password);

int handle_post(void);

int show_entry(const char *path);

#endif
