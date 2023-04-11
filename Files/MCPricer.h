#pragma once

#include "PathSimulator.h"
#include "Payoff.h"

class MCPricer
{
public:
	// constructors
	MCPricer(const size_t& number_sims, const Payoff& payoff, const PathSimulator& pathSimulator, const double& risk_free_rate);
	MCPricer(const MCPricer& pricer);
	
	// assignment operator
	MCPricer& operator=(const MCPricer& pricer);

	// destructor
	~MCPricer();

	 double price() const;

private:
	// number of simulations
	size_t _number_sims;
	// payoff
	const Payoff* _payoff;
	// pathSimulator
	const PathSimulator* _pathSimulator;
	// risk free rate
	double _risk_free_rate;
};