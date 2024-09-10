#include "main.hpp"
#include "optimize.hpp"

#include "_erf.hpp"
#include "_acosh.hpp"

using namespace std;

/*
 * functions for branch
*/
double g(double x) {return _erf1(x);}
double h(double x) {return _erf2(x);}

double f(double x) { return fabs(g(x) - h(x)); }

int main(void) {
	double begin, end, gbegin, gend, hbegin, hend; // begin, end of input range

	int cnt = 5; // # of test cases
	int d = 3; // depth of binTree
	std::cout << "d = " << d << "\n";

	/*
	 * Input range
	*/
	gbegin = 1.25, gend = 2.85714;
	hbegin = 2.85714, hend = 6;

	int opt = 1; // 1=branch

	// if binMax < prec : right path's function can be merged into left branch's input range
	std::cout << "<Left Branch range: (" << gbegin << ", " << gend << ")>\n";
	for (int i=0; i<cnt; i++) auto result = _maximize_scalar_bounded(f, {gbegin, gend}, 1e-6, 1e+6, 1);
	std::cout << "-------------------------------------------- \n";
	for (int i=0; i<cnt; i++) auto result2 = _runRandPoints(f, {gbegin, gend}, 1e-6, 1e+6, 1);
	std::cout << "-------------------------------------------- \n";
	for (int i=0; i<cnt; i++) auto result3 = _runBinTreePoints(f, {gbegin, gend}, 1e-6, 1e+6, d, 1);
	
	std::cout << "\n<Right Branch range: (" << hbegin << ", " << hend << ")>\n";
	for (int i=0; i<cnt; i++) auto result = _maximize_scalar_bounded(f, {hbegin, hend}, 1e-6, 1e+6, 1);
	std::cout << "-------------------------------------------- \n";
	for (int i=0; i<cnt; i++) auto result2 = _runRandPoints(f, {hbegin, hend}, 1e-6, 1e+6, 1);
	std::cout << "-------------------------------------------- \n";
	for (int i=0; i<cnt; i++) auto result3 = _runBinTreePoints(f, {hbegin, hend}, 1e-6, 1e+6, d, 1);

}


