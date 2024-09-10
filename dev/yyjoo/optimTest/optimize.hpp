#include <cmath>
#include <iostream>
#include <functional>
#include <limits>
#include <random>

// from scipy optimize
struct OptimizeResult {
    double fun;
    int status;
    bool success;
    std::string message;
    double x;
    int nfev;
    int nit;
};

void getParts(std::vector<double>& parts, int d, double begin, double end);

OptimizeResult _maximize_scalar_bounded(
    std::function<double(double)> func, 
    std::pair<double, double> bounds,
    double xatol = 1e-5, 
    int maxiter = 1e+6, 
    int disp = 0);

OptimizeResult _runRandPoints(
    std::function<double(double)> func,
    std::pair<double, double> bounds,
    double xatol = 1e-5,
    int maxiter = 1e+6,
    int disp = 0,
    int opt = 0);

OptimizeResult _runBinTreePoints(
    std::function<double(double)> func,
    std::pair<double, double> bounds,
    double xatol = 1e-5,
    int maxiter = 1e+6,
    int d = 2,
    int disp = 0,
    int opt = 0);