#include <iostream>
#include <cmath>


double normalCDF(double x) // Standard normal distribution function
{
    return std::erfc(-x / std::sqrt(2)) / 2;
}

double normalPDF(double x) // Standard normal density function
{
    static const double inv_sqrt_2pi = 0.3989422804014327;

	return inv_sqrt_2pi * std::exp(-0.5f * x * x);
}