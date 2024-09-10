all: stepTest

stepTest: _erf.o _acosh.o optimize.o main.o
	@g++ -o stepTest _erf.o _acosh.o optimize.o main.o
 
_erf.o: common.hpp _erf.hpp _erf.cpp
	@g++ -c -o _erf.o _erf.cpp

_acosh.o: common.hpp _acosh.hpp _acosh.cpp
	@g++ -c -o _acosh.o _acosh.cpp

optimize.o: optimize.hpp optimize.cpp
	@g++ -c -o optimize.o optimize.cpp
 
main.o: main.cpp main.hpp optimize.hpp _erf.hpp _acosh.hpp
	@g++ -c -o main.o main.cpp
 
clean:
	@rm -f stepTest *.o	