.PHONY: all clean

all: kledsd

clean:
	rm -f kledsd

kledsd: *.cpp
	g++ $^ -o $@ -O2 -Wall -lX11 -g -ggdb

