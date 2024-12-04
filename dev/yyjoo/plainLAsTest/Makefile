all: run

run: func.o main.o
	@g++ -o run func.o main.o
 
func.o: func.hpp 
	@g++ -c -o func.o func.cpp
 
main.o: main.cpp main.hpp func.hpp
	@g++ -c -o main.o main.cpp
 
clean:
	@rm -f run *.o	