CC = g++ -O3 -std=c++11
LDFLAGS = -lpthread -L. -leshyperneat -lneat

all:
	@mkdir -p NEAT_organisms
	@rm -f ./NEAT_organisms/*
	$(CC) test.cpp -o TEST $(LDFLAGS)

run:
	./TEST eshyperneat_config.json user_def genetic_encoding

clean:
	rm -f TEST
	rm -f -R ./functions_files
	rm -f *.txt