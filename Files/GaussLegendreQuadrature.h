#pragma once
#include "MathTools.h"
#include "LegendrePolynomial.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <random>

using namespace std::placeholders;
using Vector = std::vector<double>;
class GaussLegendreQuadrature
{
public:

    /* By default we set the number of nodes to 16, in order to avoid the computation of the nods and weights,
    which involve finding the roots of legendre polynomials of high degrees wich is numerically expensive, for 16 nodes or more the accuracy is acceptable,
    the attribut _number_sample_points is actually the number of different weights used, each weight is used twice for non zero point.*/

    GaussLegendreQuadrature();
    //GaussLegendreQuadrature(const size_t& number_sample_points);

    // Integration of f over the real interval ]- infinity, + infinity[
    double integrate(std::function<double(double)> func) const;
    // To compute the nodes, one needs to compute first the legendre_polynomial

private:

    size_t _number_sample_points;
    Vector _weights;
    Vector _points;
    // Those data members are constructed in the constructor
    //Vector _weights = { 0.5556, 0.8889, 0.5556 }; // (w_i)
    //Vector _points = { -0.7746, 0.0001 , 0.7746 }; // (x_i)
};
