CC=/usr/local/bin/mpicc 
CFLAGS = -Wall -O3 -std=gnu99 
LDFLAGS = -lwiringPi

all: pblink

pblink : pblink.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f *.o a.out core pblink
