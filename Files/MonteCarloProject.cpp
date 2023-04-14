#include <iostream>
#include "HestonModel.h"
#include "PathSimulator.h"
#include "Payoff.h"
#include "MCPricer.h"

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

int main()
{
	//Numerical values from the paper, Case 1
	double initial_spot = 100;          // S_0  
	double initial_variance = 0.2;      // V_0
	double drift = 0;                 // r, the rate
	double mean_reversion_speed = 0.5;  // kappa
	double mean_reversion_level = 0.04;  // theta
	double vol_of_vol = 1.;            // sigma_V
	double correlation = -0.9;           // rho

	int n_simulations = 1000;
	Vector time_points = { 0., 0.2, 0.4, 0.6, 0.8, 1.0 };

	//tests constructors class HestonModel and of the operator =
	HestonModel firstModel(initial_spot, initial_variance, drift, mean_reversion_speed, mean_reversion_level, vol_of_vol, correlation);
	HestonModel testModel(firstModel);
	firstModel = testModel;

	//Truncated Euler Scheme and tests of its methods;
	EulerPathSimulatorModified eps1(firstModel, time_points);

	double maturity = eps1.expiry();
	Matrix path = eps1.path();

	//Print of a trajectory of the spot, we can notice that the asset may become constant after few iterations, that's because in the truncated 
	//Euler Scheme, the variance can become null and in this case the asset price is only incremented with the drift, that we chose null in this example.
	//It is a flaw indicated in the article. 

	for (int i(0); i < path.size(); i++) {
		//std::cout << path[i][0] << std::endl;
	}

	//BroadieKaya scheme and tests of its methods
	BroadieKaya Bk1(firstModel, time_points);
	
	//test of the TG algorithm, we set the method in public to do the test
	for (int v(0); v < 10; v++) {
		std::cout << Bk1.truncature_Gaussian(0.1);
	}
	//Strike, in the article 70,100,140
	double strike = 70;

	//PayOff
	enum CALL_PUT call;
	call = CALL_PUT::CALL;
	EuropeanOptionPayoff payoff(call, strike);

	//Pricing
	MCPricer pricer(n_simulations, payoff, eps1, drift);
	double price = pricer.price();
	
	//std::cout << price << std::endl;
	std::cin.get();
	return 0;

}