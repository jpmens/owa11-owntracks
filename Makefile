CFLAGS  =-Wall -Werror -DDEBUG=0

all: owa11-owntracks

owa11-owntracks: owa11-owntracks.c json.o Makefile
	$(CC) $(CFLAGS) -o owa11-owntracks owa11-owntracks.c json.o

clean:
	rm -f *.o
clobber: clean
	rm -f owa11-owntracks
