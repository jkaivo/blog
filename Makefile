.POSIX:

CFLAGS=-static
OBJECTS=blog.o post.o auth.o

all: blog

blog: $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LDFLAGS) $(LIBS)

install: blog
	strip blog
	doas cp -f blog /var/www/cgi-bin/blog

clean:
	rm -f *.o blog
