#include <iostream>
#include "HestonModel.h"
#include "HestonPricer.h"
#include "PathSimulator.h"
#include "Payoff.h"
#include "MCPricer.h"

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

int main()
{
	//Numerical values from the paper, Case 1
	double initial_spot = 100;          // S_0  
	double initial_variance = 0.04;      // V_0
	double drift = 0;                 // r, the rate
	double mean_reversion_speed = 0.5;  // kappa
	double mean_reversion_level = 0.04;  // theta
	double vol_of_vol = 1.;            // sigma_V
	double correlation = -0.9;           // rho
	double maturity = 1;                //T

	int n_simulations = 500;

	//Grid
	Vector time_points;
	double t = 0;
	double dt = 0.5;
	while (t < maturity) {
		time_points.push_back(t);
		t += dt;
	}

	//tests constructors class HestonModel and of the operator =
	HestonModel firstModel(initial_spot, initial_variance, drift, mean_reversion_speed, mean_reversion_level, vol_of_vol, correlation);
	HestonModel testModel(firstModel);
	firstModel = testModel;

	//Truncated Euler Scheme and tests of its methods;
	//EulerPathSimulatorModified eps1(firstModel, time_points);
	EulerPathSimulatorModified eps1(firstModel, maturity, 12);


	maturity = eps1.expiry();
	Matrix path = eps1.path();


	//Print of a trajectory of the spot, we can notice that the asset may become constant after few iterations, that's because in the truncated 
	//Euler Scheme, the variance can become null and in this case the asset price is only incremented with the drift, that we chose null in this example.
	//It is a flaw indicated in the article. 
	/*
	std::cout << "trajectory of one asset using the Truncated Euler scheme" << std::endl;
	for (int i(0); i < path.size(); i++) {
		std::cout << path[i][0] << std::endl;
	}
	*/
	//test class MathTools
	MathTools tools;
	bool tg = true;

	//BroadieKaya scheme and tests of its methods
	//BroadieKaya Bk1(firstModel, time_points, tools, tg);
	//BroadieKaya Bk2(firstModel, time_points, tools, !tg);
	BroadieKaya Bk1(firstModel, maturity, 12, tools, tg);
	BroadieKaya Bk2(firstModel, maturity, 12, tools, !tg);

	//test of the TG algorithm, we set the method in public to do the test
	//std::cout << Bk1.truncature_gaussian(0.1) << std::endl;
	
	//test of the computation of a trajectory of the spot using the BroadieKaya algorithm
	double maturity_bk_1 = Bk1.expiry();
	Matrix path_bk_1 = Bk1.path();
	double maturity_bk_2 = Bk2.expiry();
	Matrix path_bk_2 = Bk2.path();
	
	//Print of a trajectory of the spot, contrary to the truncated Euler scheme, we notice that the spot doesn't become constant in few iterations. 
	//However, with a frequency of approximatively (1/1000), it appears that if the time step delta is too small, the asset price goes to - infty,
	//(as we carry out the scheme on the log, it means that the asset price reaches 0).
	
	/*std::cout << "trajectory of one asset using the TG-BroadieKaya scheme" << std::endl;
	for (int i(0); i < path_bk_1.size(); i++) {
		std::cout << path_bk_1[i][0] << std::endl;
	}
	std::cout << "trajectory of one asset using the QE-BroadieKaya scheme" << std::endl;
	for (int i(0); i < path_bk_2.size(); i++) {
		std::cout << path_bk_2[i][0] << std::endl;
	}
	*/
	//Strike, in the article 70,100,140
	double strike = 100;

	//PayOff
	enum CALL_PUT call;
	call = CALL_PUT::CALL;
	EuropeanOptionPayoff payoff(call, strike);

	//Pricing
	MCPricer pricer_euler(n_simulations, payoff, eps1, drift);
	MCPricer pricer_bk_1(n_simulations, payoff, Bk1, drift);
	MCPricer pricer_bk_2(n_simulations, payoff, Bk1, drift);
	double price_euler = pricer_euler.price();
	double price_bk_1 = pricer_bk_1.price();
	double price_bk_2 = pricer_bk_2.price();

	//We notice a substancial difference between prices obtained with the BroadieKaya schemes, but almost no difference between the TG-QE schemes 
	std::cout << "Price obtained with the truncated Euler scheme" << std::endl;
	std::cout << price_euler << std::endl;
	std::cout << "Price obtained with the TG-BroadieKaya scheme" << std::endl;
	std::cout << price_bk_1 << std::endl;
	std::cout << "Price obtained with the QE-BroadieKaya scheme" << std::endl;
	std::cout << price_bk_2 << std::endl;

	//Test prices analytical
	AnalyticalHestonPricer EdoaPricer(firstModel, strike, maturity, 32);
	std::cout << EdoaPricer.price() << std::endl;
	std::cin.get();
	return 0;

}