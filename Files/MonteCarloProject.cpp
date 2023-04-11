#include <iostream>
#include "Model.h"
#include "PathSimulator.h"
#include "Payoff.h"

int main()
{
	BlackScholesModel bsModel(100, 0.05, 0.2);
	Vector time_points = { 0., 0.2, 0.4, 0.6, 0.8, 1. };

	EulerPathSimulator eps1(bsModel, time_points);
	EulerPathSimulator eps2(eps1);

	return 0;

}