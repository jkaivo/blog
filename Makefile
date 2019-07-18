.POSIX:

CFLAGS=-static

all: blog

install: blog
	strip blog
	doas cp -f blog /var/www/cgi-bin/blog

clean:
	rm -f *.o blog
