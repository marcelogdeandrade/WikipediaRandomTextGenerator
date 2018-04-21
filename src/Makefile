CFLAGS = -std=c++11 -Wall

all: pugixml.o read_input.o main.o tree.o
	g++ $^ -o main 

pugixml.o: pugixml.hpp pugiconfig.hpp pugixml.cpp
	g++ $(CFLAGS) -c pugixml.cpp 

read_input.o: read_input.cpp pugixml.hpp read_input.hpp
	g++ $(CFLAGS) -c read_input.cpp

main.o: pugixml.o read_input.o main.cpp main.hpp
	g++ ${CFLAGS} -c main.cpp

tree.o: tree.cpp tree.hpp
	g++ ${CFLAGS} -c tree.cpp

clean:
	rm pugixml.o read_input.o main.o tree.o main

