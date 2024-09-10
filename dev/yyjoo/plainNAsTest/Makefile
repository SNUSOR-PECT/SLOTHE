all: runAct

runAct: asor.o main.o
	@g++ -o runAct asor.o main.o
 
asor.o: asor.hpp 
	@g++ -c -o asor.o asor.cpp
 
main.o: main.cpp main.hpp asor.hpp
	@g++ -c -o main.o main.cpp
 
clean:
	@rm -f runAct *.o	