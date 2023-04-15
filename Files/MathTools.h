#pragma once
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <random>
#include <chrono>

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

class MathTools 
{
public:
    double normalCDF(double x) const; // Standard normal distribution function
    double normalPDF(double x) const; // Standard normal density function
    double eq_r(double r, double psi) const; // Function such that f(r) = 0
    double secantMethod(int n_iterations, double psi, std::function<double(double, double)> func, double precision = 0.01) const; // secant root algorithm
    double trapezoidalMethod(double previous_x, double next_x, double delta, double gamma_1 = 0.5) const; // trapezoidal method for the computation of an integral
    double WinerIntegral(double previous_x, double next_x, double delta, double gamma_1 = 0.5) const; // trapezoidal method for the computation of a Wiener integral of the square root of x
};
