#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "blog.h"

int verify_creds(const char *username, const char *password)
{
	printf("verifying '%s'/'%s'\n", username ? username : "", password ? password : "");
	return 1;
}
