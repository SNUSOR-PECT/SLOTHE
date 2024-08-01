#include <iostream>
#include <random>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <algorithm>

#include "main.hpp"
#include "_erf.hpp"

using namespace std;

/*
 * functions for branch
*/
double g(double x) {return _erf1(x);}
double h(double x) {return _erf2(x);}

// double g(double x) { double t = x*x; return log(2.0*x - 1 / (x + sqrt(t-1)));}
// double h(double x) { double t = x-1; return log1p(t+sqrt(2*t+t*t));}

// double g(double x) { return 1 - 2/(exp(2*abs(x))+1);}
// double h(double x) { return - (exp(-2*abs(x))-1) / (exp(-2*abs(x))-1+2);}

//double g(double x) { return x>0? x:0;} // ReLU
//double h(double x) { return log(1+exp(x));}

// double g(double x) { return exp(x); }
// double h(double x) { return log1p(exp(x)); }

//double g(double x) { return x; }
//double h(double x) { return log1p(exp(x)); }

double f(double x) { return fabs(g(x) - h(x)); }

/*
 * function for single eval
*/
// double f(double x) { return (x-1)*(x-2)*(x-3)*(x-5)+1; }
//double f(double x) { return tanh(x); }
// double f(double x) { return 4.3379*pow(10,-4) + 0.86758*x - 0.13112*pow(x, 3) + 1.0169*pow(10,-2)*pow(x, 5) - 3.0632*pow(10,-4)*pow(x, 7); } // approximated tanh in [-4, 4] (asiaccs)
// double f(double x) { return exp(x); }

double runRandPoints(double begin, double end, double step, int opt) {
	int cnt = 10;
	std::vector<int> iters(cnt);
	std::vector<double> xs(cnt), xMax(cnt), yMax(cnt);

	// Check end points
	if (opt) {
		if (f(begin) > step) {
			xMax[0] = begin; yMax[0] = f(begin);
			iters[0] += 1;
			cnt = 0; // do not enter loop
		}
		if (f(end) > step) {
			xMax[0] = end; yMax[0] = f(end);
			iters[0] += 1;
			cnt = 0; // do not enter loop
		}
	}
 
	for (int i=0; i<cnt; i++) {
		// for random start point
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(begin, end);
		double start_point = dist(gen);
		
		double point = start_point; // init
		double _xMax = -1000.0; // not in range
		double _yMax = -1000.0; // since f is defined w/ fabs
		
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
			// break loop when yMax is less than step(=precision)
			if (opt && _yMax > step) {break;}

			// move
			if (abs(grad)<1e-6) break;
			else if (grad>0) point = point+grad*step;
			else point = point-grad*step;
					
			// end if point is out of range
			if (point < begin || point > end) break;
		}

		// Check end points
		if (f(begin) > _yMax) {
			_xMax = begin; _yMax = f(begin);
		}
		if (f(end) > _yMax) {
			_xMax = end; _yMax = f(end);
		}

		iters[i] = iter;
		xs[i] = start_point;
		xMax[i] = _xMax;
		yMax[i] = _yMax;
	}

	int idx = max_element(yMax.begin(), yMax.end()) - yMax.begin();
	double xMaxFin = xMax[idx];
	double yMaxFin = yMax[idx];
	// std::cout << "Final maximum point = (" << xMaxFin << ", " << yMaxFin << ")\n";

	int iterSum = accumulate(iters.begin(), iters.end(), 0.0);
	// std::cout << "Total iter = " << iterSum << "\n";

	// for (int i=0;i<cnt;i++) {std::cout << yMax[i] << ", ";}
	// std::cout << std::endl;

	std::cout << yMaxFin << ", " << iterSum << ", ";
	return yMaxFin;
}

double runBinTreePoints(double begin, double end, int d, double step, int opt) {
	std::vector<double> parts;
	getParts(parts, d, begin, end); // get binary partitions
	// for (int i=0; i<pow(2, d) + 1; i++) {std::cout << parts[i] << ", ";} std::cout << std::endl;

	int cnt = parts.size()-1; // = pow(2, d)
	std::vector<int> iters(cnt, 0);
	std::vector<double> xs(cnt), xMax(cnt), yMax(cnt);

	// Check end points (max candidate > precision?)
	if (opt) {
		if (f(begin) > step) {
			xMax[0] = begin; yMax[0] = f(begin);
			iters[0] += 1;
			cnt = 0; // do not enter loop
		}
		if (f(end) > step) {
			xMax[0] = end; yMax[0] = f(end);
			iters[0] += 1;
			cnt = 0; // do not enter loop
		}
	}

	std::vector<std::pair<double,double>> mixIdx(cnt); // [0] [-1] ..
	for (int i=0;i<cnt;i++) { 
		if (i%2==0)
			mixIdx[i] = {parts[i/2], parts[i/2+1]}; 
		else
			mixIdx[i] = {parts[cnt-(i/2)-1], parts[cnt-(i/2)]};
	}

	for (int i=0; i<cnt; i++) {
		// for random in partial range start point
		begin = mixIdx[i].first, end = mixIdx[i].second;
		std::random_device rd; std::mt19937 gen(rd());
		std::uniform_real_distribution<> dist(begin, end);
		double start_point = dist(gen);

		// std::cout << "begin = " << begin << ", end = " << end << ", start_point = " << start_point << "\n";
		
		double point = start_point; // init
		double _xMax = -1000.0; // not in range
		double _yMax = -1000.0; // since f is defined w/ fabs
		
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
			// break loop when yMax is less than step(=precision)
			if (opt && _yMax > step) {break;}

			// move
			if (abs(grad)<1e-6) break;
			else if (grad>0) point = point+grad*step;
			else point = point-grad*step;
					
			// end if point is out of range
			if (point < begin || point > end) break;
		}

		// Check end points
		if (f(begin) > _yMax) {
			_xMax = begin; _yMax = f(begin);
		}
		if (f(end) > _yMax) {
			_xMax = end; _yMax = f(end);
		}

		iters[i] = iter;
		xs[i] = start_point;
		xMax[i] = _xMax;
		yMax[i] = _yMax;
	}

	int idx = max_element(yMax.begin(), yMax.end()) - yMax.begin();
	double xMaxFin = xMax[idx];
	double yMaxFin = yMax[idx];
	// std::cout << "Final maximum point = (" << xMaxFin << ", " << yMaxFin << ")\n";

	int iterSum = accumulate(iters.begin(), iters.end(), 0.0);
	// std::cout << "Total iter = " << iterSum << "\n";

	// for (int i=0;i<cnt;i++) {std::cout << "(" << xMax[i] << ", " << yMax[i] << ")\n";}
	// std::cout << std::endl;

	std::cout << yMaxFin << ", " << iterSum; // << "\n";
	return yMaxFin;
}

int main(void) {
	double step = 1e-3; // for SGD
	double prec = step; // user defined precision (=step)
	double begin, end, gbegin, gend, hbegin, hend; // begin, end of input range

	int cnt = 10; // # of test cases
	int d = 3; // depth of binTree
	std::cout << "d = " << d << "\n";

	/*
	 * Input range cases
	*/
	// begin = 1.0, end = 5.01; // range of input parts
	// begin = -4.0, end = 4.0;

	/*
	 * 1/0.35 = 2.85714
	*/

	gbegin = 0.0, gend = 0.84375;
	hbegin = 0.84375, hend = 1.25;

	// gbegin = -22.0, gend = -1.0;
	// hbegin = -1.0, hend = 0.0;
	// gbegin = -60.0, gend = -34.0437;
	// hbegin = -34.0437, hend = 34.0437;
	// gbegin = 34.0437, gend = 50.0;
	// hbegin = -60.0, hend = 34.0437;

	int opt = 1; // 0=single, 1=branch

	if (opt==0) { // single function
		std::cout << "<Range: (" << begin << ", " << end << ")>\n";
		for (int i=0; i<cnt; i++) {
			double randMax = runRandPoints(begin, end, step, opt);
            double binMax = runBinTreePoints(begin, end, d, step, opt);
			std::cout << "\n";
		}
	}
	else if (opt==1) { // branch
		std::cout << "<Left Branch range: (" << gbegin << ", " << gend << ")>\n";
		for (int i=0; i<cnt; i++) {
			double randMax = runRandPoints(gbegin, gend, step, opt);
			double binMax = runBinTreePoints(gbegin, gend, d, step, opt);
			std::cout << "\n";
			// std::cout << "\t\t isMergeable = " << (binMax < prec) << "\n";
		}
		std::cout << "\n<Right Branch range: (" << hbegin << ", " << hend << ")>\n";
		for (int i=0;i<cnt;i++) {
			double randMax = runRandPoints(hbegin, hend, step, opt);
			double binMax = runBinTreePoints(hbegin, hend, d, step, opt);
			std::cout << "\n";
			// std::cout << "\t\t isMergeable = " << (binMax < prec) << "\n";
		}
	}
}


