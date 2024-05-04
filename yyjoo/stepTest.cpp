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

void getParts(std::vector<double>& parts, int d, double begin, double end) {
	int sz = pow(2, d) + 1;
	parts.resize(sz);
	double gap = (end - begin) / (sz-1);
	for (int i=0; i<sz; i++) {parts[i] = begin + gap*i;}
}

//////////////////////////
// Code for Code Approx //
//////////////////////////

double delta = 1e-6; // (~2^-20) for derivative
double step = 1e-3; // for SGD

double g(double x) { return 1 - 2/(exp(2*abs(x))+1);}
double h(double x) { return - (exp(-2*abs(x))-1) / (exp(-2*abs(x))-1+2);}
//double f(double x) { return fabs(g(x) - h(x)); }
double f(double x) { return (x-1)*(x-2)*(x-3)*(x-5)+1; }

void runRandPoints(double begin, double end) {
	int cnt = 10;
	std::vector<int> iters(cnt);
	std::vector<double> xs(cnt), xMax(cnt), yMax(cnt);
 
	for (int i=0; i<cnt; i++) {
		// for random start point
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(begin, end);
		double start_point = dist(gen);
		
		double point = start_point; // init
		double _xMax = -1.0; // not in range
		double _yMax = -10.0; // since f is defined w/ fabs
		
		int epoch = 1e+6;
		int iter = 0;
		while(epoch--) {
			iter += 1;
			double cur = f(point); // current point
			double next = f(point+delta); // next point
			double grad = (next - cur) / delta; // numerical derivative
			//std::cout << "(x, y) = (" << point << ", " << cur << "), grad = " << grad << std::endl;
			
			// find _yMax
			if (cur > _yMax) {
				_xMax = point;
				_yMax = cur;
			}

			// move
			if (abs(grad)<1e-6) break;
			else if (grad>0) point = point+grad*step;
			else point = point-grad*step;
					
			// end if point is out of range
			if (point < begin || point > end) break;
		}

		// Check end points
		// if (f(begin) > _yMax) {
		// 	_xMax = begin; _yMax = f(begin);
		// }
		// if (f(end) > _yMax) {
		// 	_xMax = end; _yMax = f(end);
		// }

		iters[i] = iter;
		xs[i] = start_point;
		xMax[i] = _xMax;
		yMax[i] = _yMax;
	}

	int idx = max_element(yMax.begin(), yMax.end()) - yMax.begin();
	double xMaxFin = xMax[idx];
	double yMaxFin = yMax[idx];
	std::cout << "Final maximum point = (" << xMaxFin << ", " << yMaxFin << ")\n";

	int iterSum = accumulate(iters.begin(), iters.end(), 0.0);
	std::cout << "Total iter = " << iterSum << "\n";

	// for (int i=0;i<cnt;i++) {std::cout << yMax[i] << ", ";}
	// std::cout << std::endl;
}

void runBinTreePoints(double begin, double end) {
 
	int d = 2; // depth of binary tree
	std::vector<double> parts;
	getParts(parts, d, begin, end); // get binary partitions

	// for (int i=0; i<pow(2, d) + 1; i++) {std::cout << parts[i] << ", ";} std::cout << std::endl;

	int cnt = parts.size()-1; // = pow(2, d)
	std::vector<int> iters(cnt);
	std::vector<double> xs(cnt), xMax(cnt), yMax(cnt);

	for (int i=0; i<cnt; i++) {
		// for random in partial range start point
		begin = parts[i], end = parts[i+1];
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(begin, end);
		double start_point = dist(gen);
		
		double point = start_point; // init
		double _xMax = -1.0; // not in range
		double _yMax = -10.0; // since f is defined w/ fabs
		
		int epoch = 1e+6;
		int iter = 0;
		while(epoch--) {
			iter += 1;
			double cur = f(point); // current point
			double next = f(point+delta); // next point
			double grad = (next - cur) / delta; // numerical derivative
			// std::cout << "(x, y) = (" << point << ", " << cur << "), grad = " << grad << std::endl;
			
			// find _yMax
			if (cur > _yMax) {
				_xMax = point;
				_yMax = cur;
			}

			// move
			if (abs(grad)<1e-6) break;
			else if (grad>0) point = point+grad*step;
			else point = point-grad*step;
					
			// end if point is out of range
			if (point < begin || point > end) break;
		}

		// Check end points
		// if (f(begin) > _yMax) {
		// 	_xMax = begin; _yMax = f(begin);
		// }
		// if (f(end) > _yMax) {
		// 	_xMax = end; _yMax = f(end);
		// }

		iters[i] = iter;
		xs[i] = start_point;
		xMax[i] = _xMax;
		yMax[i] = _yMax;
	}

	int idx = max_element(yMax.begin(), yMax.end()) - yMax.begin();
	double xMaxFin = xMax[idx];
	double yMaxFin = yMax[idx];
	std::cout << "Final maximum point = (" << xMaxFin << ", " << yMaxFin << ")\n";

	int iterSum = accumulate(iters.begin(), iters.end(), 0.0);
	std::cout << "Total iter = " << iterSum << "\n";

	// for (int i=0;i<cnt;i++) {std::cout << "(" << xMax[i] << ", " << yMax[i] << ")\n";}
	// std::cout << std::endl;
}

int main(void) {
	double begin = 1.0, end = 5.1; // range of input parts
	//double begin = 0.0, end = 22.0;

	runRandPoints(begin, end);
	runBinTreePoints(begin, end);
	
}

