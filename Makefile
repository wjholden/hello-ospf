CC = gcc
IFLAGS =
CFLAGS = -march=native -Wall -Wextra -pedantic -std=gnu11 -Werror
DFLAGS = -g
OBJ = chksum.o msgtype.o authtype.o print-ip.o print-header.o print-hello.o print-lls.o init.o getiface.o hello.o receive.o
EXTRAS = AUTHOR README VERSION CHANGELOG ROADMAP

%.o:	%.c $(DEPS)
	$(CC) $(IFLAGS) -c -o $@ $< $(CFLAGS) $(DFLAGS)

all:	hello

hello:	$(OBJ)
	$(CC) $(CFLAGS) $(DFLAGS) -o $@ $^

clean:
	$(RM) *~ -v *.o hello listen

release: 
	tar czf hello_ospf-`cat VERSION`.tar.gz Makefile $(EXTRAS) *.c *.h 
	sha256sum hello_ospf-`cat VERSION`.tar.gz > hello_ospf-`cat VERSION`.tar.gz.sha256
