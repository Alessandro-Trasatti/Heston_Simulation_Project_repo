#include "MCPricer.h"

MCPricer::MCPricer(const size_t & number_sims, const Payoff & payoff, const PathSimulator & pathSimulator, const double & risk_free_rate)
	: _number_sims(number_sims), _payoff(payoff.clone()), _pathSimulator(pathSimulator.clone()), _risk_free_rate(risk_free_rate)
{
}

MCPricer::MCPricer(const MCPricer & pricer)
	: _number_sims(pricer._number_sims), _payoff(pricer._payoff->clone()), _pathSimulator(pricer._pathSimulator->clone()), _risk_free_rate(pricer._risk_free_rate)
{
}

MCPricer & MCPricer::operator=(const MCPricer & pricer)
{
	if (this != &pricer)
	{
		_number_sims = pricer._number_sims;
		_payoff = pricer._payoff->clone();
		_pathSimulator = pricer._pathSimulator->clone();
		_risk_free_rate = pricer._risk_free_rate;
	}
	return *this;
}

MCPricer::~MCPricer()
{
	delete _payoff;
	delete _pathSimulator;
}

double MCPricer::price() const
{
	// for loop on the number of simulations
	// first step - focus only on European payoffs

	// price = DF(0,T) * E[ phi((S_t))] 
	// E[phi((S_t))] = 1/N * SUM(phi(each path))

	// if few trajectories diverge, instead of having a price that will diverge we don't consider them for the computation 
	// but we warn the user and tell him how many trajectories diverged.
	int n_nan = 0;
	double price = 0.;
	for (size_t sim_idx = 0; sim_idx < _number_sims; sim_idx++)
	{
		Matrix path = _pathSimulator->path();
		n_nan = (isnan(path[path.size() - 1][0])) ? n_nan + 1 : n_nan;
		price = (isnan(path[path.size() - 1][0])) ? price : price + _payoff->payoff(path);
	}
	// Case if all paths are nan
	if (n_nan == (double)_number_sims) {
		throw "Every path gave a nan";
	}
	else {
		price /= ((double)_number_sims - n_nan);
	}

	// Discount Factor
	double disc_fact = std::exp(-_risk_free_rate * _pathSimulator->expiry());

	price *= disc_fact;
	
	//We tell the user about the proportion of nan
	std::cout << 100.0 * ((double)_number_sims - n_nan) / ((double)_number_sims);
	std::cout << "% of the simulations were sucessful, the rest diverged" << std::endl;
	return price;

}
