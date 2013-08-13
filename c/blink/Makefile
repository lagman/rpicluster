CC=/usr/bin/gcc
CFLAGS = -Wall -O3 -std=gnu99 
LDFLAGS = -lwiringPi

all: blink

blink : blink.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o a.out core blink
