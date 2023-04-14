#pragma once
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

double normalCDF(double x) // Standard normal distribution function
{
    return std::erfc(-x / std::sqrt(2)) / 2;
}

double normalPDF(double x) // Standard normal density function
{
    static const double inv_sqrt_2pi = 0.3989422804014327;

	return inv_sqrt_2pi * std::exp(-0.5f * x * x);
}

double eq_r(double r, double psi) {
    return (r * normalPDF(r)) + normalCDF(r) * (1 + r * r) - (1 + psi) * (normalPDF(r) + r * normalCDF(r)) * (normalPDF(r) + r * normalCDF(r));
}

double secantMethod(int n_iterations, double psi, std::function<double(double, double)> func) {
    double r = 0.0;
    Vector rn = { 0.0, 5.0 };
    for (int i(0); i < n_iterations; i++) {
        r = rn[1] - func(rn[1], psi) * ((rn[1] - rn[2]) / (func(rn[1], psi) - func(rn[0], psi)));
        rn[0] = rn[1];
        rn[1] = r;
    }
    return r;
}