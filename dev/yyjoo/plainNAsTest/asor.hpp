#pragma once
#include <functional>
#include <iostream>
#include <vector>
#include <cmath>

#define maxIter 100

inline std::function<double(double)> f_inv = [](double z) -> double {
        return z * (2 - z);
};

void getRelaxationFactor(std::function<double(double)> f, double alpha, double epsilon, std::vector<double>& K);
double Inverse(double alpha, double epsilon, std::vector<double>& K, double x);