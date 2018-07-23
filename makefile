EXECUTABLE = ESHyperNeat
VPATH = ./src ./headers ./objects
CC = g++ -O3 -std=c++0x
CFLAGS = -g -Wall -fPIC -I./headers 
LDFLAGS = -lpthread -L. -lneat
OBJS = ./objects/ESHyperNeat.o ./objects/Substrate.o ./objects/SpatialNode.o ./objects/CPPNInputs.o ./objects/NeuralNetwork.o 

all: SpatialNode.o Substrate.o CPPNInputs.o ESHyperNeat.o NeuralNetwork.o 
	
ESHyperNeat.o: ESHyperNeat.cpp
	@mkdir -p objects
	@echo "Compiling ESHyperNeat.cpp to ESHyperNeat.o"
	@$(CC) $(CFLAGS) -c ./src/ESHyperNeat.cpp  -o ./objects/ESHyperNeat.o

Substrate.o: Substrate.cpp
	@mkdir -p objects
	@echo "Compiling Substrate.cpp to Substrate.o"
	@$(CC) $(CFLAGS) -c ./src/Substrate.cpp  -o ./objects/Substrate.o

SpatialNode.o: SpatialNode.cpp
	@mkdir -p objects
	@echo "Compiling SpatialNode.cpp to SpatialNode.o"
	@$(CC) $(CFLAGS) -c ./src/SpatialNode.cpp  -o ./objects/SpatialNode.o

CPPNInputs.o: CPPNInputs.cpp 
	@mkdir -p objects
	@echo "Compiling CPPNInputs.cpp to CPPNInputs.o"
	@$(CC) $(CFLAGS) -c ./src/CPPNInputs.cpp  -o ./objects/CPPNInputs.o

NeuralNetwork.o: NeuralNetwork.cpp 
	@mkdir -p objects
	@echo "Compiling NeuralNetwork.cpp to NeuralNetwork.o"
	@$(CC) $(CFLAGS) -c ./src/NeuralNetwork.cpp  -o ./objects/NeuralNetwork.o

clean:
	@rm -f -R ./objects
	@cd test; make clean

cleandocs:
	@rm -f -R ./doc

install:
	g++ -shared -Wl,-soname,libeshyperneat.so.1 -o libeshyperneat.so.1.0 $(OBJS) $(LDFLAGS)
	ln -sf libeshyperneat.so.1.0 libeshyperneat.so
	ln -sf libeshyperneat.so.1.0 libeshyperneat.so.1
	mv libeshyperneat.so.1.0 libeshyperneat.so libeshyperneat.so.1 /usr/lib
	mkdir -p /usr/include/ESHYPERNEAT_headers/
	cp ./headers/* /usr/include/ESHYPERNEAT_headers/
	cp ESHYPERNEAT /usr/include
	chmod go+r /usr/include/ESHYPERNEAT_headers/*
	chmod go+r /usr/include/ESHYPERNEAT

docs:
	@mkdir -p doc
	@doxygen ESHYPERNEAT_doxyfile 

git:
	make clean
	make cleandocs
	git add --all
	git commit -m "$(commit)"
	git push