CC = gcc
CFLAGS = -g -Wall -Wextra -pthread

all: req-res multi-lookup

multi-lookup: multi-lookup.c util.c
	$(CC) -o multi-lookup multi-lookup.c $(CFLAGS)

req-res:
	rm -f serviced.txt
	rm -f results.txt
	touch serviced.txt
	touch results.txt

clean:
	rm -f multi-lookup
	rm -f serviced.txt
	rm -f results.txt
