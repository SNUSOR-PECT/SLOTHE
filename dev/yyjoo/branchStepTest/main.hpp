#include <iostream>
#include <random>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

using namespace std;

double delta = 1e-6; // (~2^-20) for derivative

double runRandPoints(double begin, double end, double step, int opt);
double runBinTreePoints(double begin, double end, int d, double step, int opt);

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

//////////////////////////
// Code for Code Approx //
//////////////////////////

void getParts(std::vector<double>& parts, int d, double begin, double end) {
	int sz = pow(2, d) + 1;
	parts.resize(sz);
	double gap = (end - begin) / (sz-1);
	for (int i=0; i<sz; i++) {parts[i] = begin + gap*i;}
}
