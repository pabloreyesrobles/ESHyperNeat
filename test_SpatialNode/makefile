CC = g++ -O3 -std=c++11
LDFLAGS = -lpthread -L. -leshyperneat -lneat

all:
	$(CC) test.cpp -g -o TEST $(LDFLAGS)

run:
	./TEST

clean:
	rm -f TEST