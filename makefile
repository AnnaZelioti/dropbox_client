CC=gcc 
CFLAGS=-Wall
all: dropbox_client.c 
	$(CC) -o client dropbox_client.c funs.c $(CFLAGS)

clean:
	rm -f server
