CFLAGS = -std=c++11 -Wall
FLAGS_BOOST = -lboost_mpi -lboost_serialization
CC = mpic++

all: pugixml.o read_input.o main.o tree.o
	$(CC) $^ -o main $(FLAGS_BOOST)

pugixml.o: pugixml.hpp pugiconfig.hpp pugixml.cpp
	$(CC) $(CFLAGS) -c pugixml.cpp 

read_input.o: read_input.cpp pugixml.hpp read_input.hpp
	$(CC) $(CFLAGS) -c read_input.cpp

main.o: pugixml.o read_input.o main.cpp main.hpp
	$(CC) ${CFLAGS} -c main.cpp 

tree.o: tree.cpp tree.hpp
	$(CC) ${CFLAGS} -c tree.cpp

clean:
	rm pugixml.o read_input.o main.o tree.o main

