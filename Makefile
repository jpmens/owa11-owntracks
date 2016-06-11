CFLAGS  =-Wall -Werror

all: owa11-owntracks

owa11-owntracks: owa11-owntracks.c json.o
	$(CC) $(CFLAGS) -o owa11-owntracks owa11-owntracks.c json.o
