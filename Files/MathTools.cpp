#include "MathTools.h"

double MathTools::normalCDF(double x) const// Standard normal distribution function
{
    return std::erfc(-x / std::sqrt(2)) / 2;
}

double MathTools::normalPDF(double x) const// Standard normal density function
{
    static const double inv_sqrt_2pi = 0.3989422804014327;

    return inv_sqrt_2pi * std::exp(-0.5f * x * x);
}

double MathTools::eq_r(double r, double psi) const{
    return (r * normalPDF(r)) + normalCDF(r) * (1 + r * r) - (1 + psi) * (normalPDF(r) + r * normalCDF(r)) * (normalPDF(r) + r * normalCDF(r));
}

double MathTools::secantMethod(int n_iterations, double psi, std::function<double(double, double)> func, double precision) const{
    double r = psi;
    Vector rn = { precision, psi + precision};
    int i = 0;
    // We stop the search either if we have reached the maximal number of iterations, or if the difference between two consecutives terms is low enough
    while (i < n_iterations && std::abs(rn[1] - rn[0]) > precision) {
        r = rn[1] - func(rn[1], psi) * ((rn[1] - rn[0]) / (func(rn[1], psi) - func(rn[0], psi)));
        rn[0] = rn[1];
        rn[1] = r;
        i++;
    }
    return r;
}

double MathTools::trapezoidalMethod(double previous_x, double next_x, double delta, double gamma_1) const
{   
    /* to speed up the process, we don't control gamma here
    if (gamma_1 > 1 || gamma_1 < 0) {
        throw "gamma must belong to [0,1]";
    }
    */
    return delta * (gamma_1 * previous_x + (1 - gamma_1) * next_x);
}

double MathTools::WinerIntegral(double previous_x, double next_x, double delta, double gamma_1) const
{
    if (previous_x < 0 || next_x < 0) {
        throw std::invalid_argument("received negative value for the variance");
    }
    else if (gamma_1 > 1 || gamma_1 < 0) {
        throw "gamma must belong to [0,1]";
    }
    else {
        std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
        std::normal_distribution<double> distribution(0., 1.);
        double delta_brownian = std::sqrt(delta) * distribution(generator);
        return delta_brownian * std::sqrt(gamma_1 * previous_x + (1 - gamma_1) * next_x);
    }

}

double MathTools::binomialcoefficient(const int& n, const int& k)
{
    return 0.0;
}
