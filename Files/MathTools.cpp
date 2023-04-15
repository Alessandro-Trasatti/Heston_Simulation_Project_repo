#include "MathTools.h"

double MathTools::normalCDF(double x) // Standard normal distribution function
{
    return std::erfc(-x / std::sqrt(2)) / 2;
}

double MathTools::normalPDF(double x) // Standard normal density function
{
    static const double inv_sqrt_2pi = 0.3989422804014327;

    return inv_sqrt_2pi * std::exp(-0.5f * x * x);
}

double MathTools::eq_r(double r, double psi) {
    return (r * normalPDF(r)) + normalCDF(r) * (1 + r * r) - (1 + psi) * (normalPDF(r) + r * normalCDF(r)) * (normalPDF(r) + r * normalCDF(r));
}

double MathTools::secantMethod(int n_iterations, double psi, std::function<double(double, double)> func, double precision) {
    double r = psi;
    Vector rn = { precision, psi + precision};
    int i = 0;
    // We stop the search either if we have reached the maximal number of iterations, or if the difference between two consecutives terms is low enough
    while (i < n_iterations && std::abs(rn[1] - rn[0]) > precision) {
        r = rn[1] - func(rn[1], psi) * ((rn[1] - rn[0]) / (func(rn[1], psi) - func(rn[0], psi)));
        std::cout << r << std::endl;
        rn[0] = rn[1];
        rn[1] = r;
        i++;
    }
    return r;
}