all: polyTest

polyTest: main.o
	@g++ -o polyTest main.o

main.o: main.cpp main.hpp
	@g++ -c -o main.o main.cpp
 
clean:
	@rm -f polyTest *.o	