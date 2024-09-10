#include <iostream>
#include <random>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

typedef union v64_union {
	double f;
	uint64_t u;
} v64;

void printBinary(double d) {
	v64 v; v.f = d;
	uint64_t mask = 1ULL << 63;
	int count = 63;
	do {
		if (mask == 0x4000000000000000 || mask == 0x8000000000000) putchar(' ');
		putchar(v.u & mask ? '1' : '0');
		count--;
	} while (mask >>= 1);
}