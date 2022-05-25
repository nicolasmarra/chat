CC ?= gcc
CFLAGS ?= -Wall -Wextra -Werror -g

LDLIBS ?= -lm

all: serveur client

serveur: receiver-tcp.c
	$(CC) -o serveur $^ $(CFLAGS) $(LDLIBS)

client: sender-tcp.c
	$(CC) -o client $^ $(CFLAGS) $(LDLIBS)

%.o: %.c 
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean
clean:
	rm -rf *.o
	rm -f client
	rm -f serveur
