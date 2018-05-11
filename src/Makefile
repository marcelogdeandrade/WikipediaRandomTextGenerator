CFLAGS = -std=c++11 -Wall
FLAGS_BOOST = -lboost_mpi -lboost_serialization
CC = mpic++

all: main_sequential main_parallel main_parallel_2

main_parallel: pugixml.o read_input.o main_parallel.o tree.o
	$(CC) $^ -o main_parallel $(FLAGS_BOOST)

main_parallel_2: pugixml.o read_input.o main_parallel_2.o tree.o
	$(CC) $^ -o main_parallel_2 $(FLAGS_BOOST)

main_sequential: pugixml.o read_input.o main_sequential.o tree.o
	$(CC) $^ -o main_sequential $(FLAGS_BOOST)

pugixml.o: pugixml.hpp pugiconfig.hpp pugixml.cpp
	$(CC) $(CFLAGS) -c pugixml.cpp 

read_input.o: read_input.cpp pugixml.hpp read_input.hpp
	$(CC) $(CFLAGS) -c read_input.cpp

main_sequential.o: pugixml.o read_input.o main_sequential.cpp main.hpp
	$(CC) ${CFLAGS} -c main_sequential.cpp

main_parallel.o: pugixml.o read_input.o main_parallel.cpp main.hpp
	$(CC) ${CFLAGS} -c main_parallel.cpp

main_parallel_2.o: pugixml.o read_input.o main_parallel_2.cpp main.hpp
	$(CC) ${CFLAGS} -c main_parallel_2.cpp

tree.o: tree.cpp tree.hpp
	$(CC) ${CFLAGS} -c tree.cpp

clean:
	rm pugixml.o read_input.o main_sequential.o main_parallel.o tree.o main_parallel main_sequential

