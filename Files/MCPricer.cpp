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

	double price = 0.;
	for (size_t sim_idx = 0; sim_idx < _number_sims; sim_idx++)
	{
		Matrix path = _pathSimulator->path();
		price += _payoff->payoff(path);
	}
	price /= (double)_number_sims;

	// Discount Factor
	double disc_fact = std::exp(-_risk_free_rate * _pathSimulator->expiry());

	price *= disc_fact;
	return price;

}
