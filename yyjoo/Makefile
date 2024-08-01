all: stepTest
 
stepTest: _erf.o main.o
	@g++ -o stepTest _erf.o main.o
 
_erf.o: _erf.hpp _erf.cpp
	@g++ -c -o _erf.o _erf.cpp
 
main.o: main.cpp main.hpp _erf.hpp
	@g++ -c -o main.o main.cpp
 
clean:
	@rm -f stepTest *.o	