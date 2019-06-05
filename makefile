CC=gcc 
CFLAGS=-Wall -lpthread
all: dropbox_client.c 
	$(CC) -o client dropbox_client.c funs.c $(CFLAGS)

clean:
	rm -f server
