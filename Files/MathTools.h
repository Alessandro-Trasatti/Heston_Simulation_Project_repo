#pragma once
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

class MathTools 
{
public:
    double normalCDF(double x); // Standard normal distribution function
    double normalPDF(double x); // Standard normal density function
    double eq_r(double r, double psi); // Function such that f(r) = 0
    double secantMethod(int n_iterations, double psi, std::function<double(double, double)> func); // secant root algorithm 
};
