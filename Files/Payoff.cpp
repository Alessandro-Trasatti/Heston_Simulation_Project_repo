#include "Payoff.h"

EuropeanOptionPayoff * EuropeanOptionPayoff::clone() const
{
	return new EuropeanOptionPayoff(*this);
}

EuropeanOptionPayoff::EuropeanOptionPayoff(const CALL_PUT & call_put, const double & strike)
	: _call_put(call_put), _strike(strike)
{
}

double EuropeanOptionPayoff::payoff(const Matrix & path, bool is_log) const
{
	double mult = (_call_put == CALL_PUT::CALL) ? 1. : -1.;
	if (is_log) {
		return std::max(mult * (std::exp(path[path.size() - 1][0]) - _strike), 0.);
	}
	else {
		return std::max(mult * (path[path.size() - 1][0] - _strike), 0.);
	}
}

DigitalOptionPayoff * DigitalOptionPayoff::clone() const
{
	return new DigitalOptionPayoff(*this);
}

DigitalOptionPayoff::DigitalOptionPayoff(const CALL_PUT & call_put, const double & strike)
	: EuropeanOptionPayoff(call_put, strike)
{
}

double DigitalOptionPayoff::payoff(const Matrix & path, bool is_log) const
{
	double mult = (_call_put == CALL_PUT::CALL) ? 1. : -1.;
	if (is_log) {
		return (mult * (std::exp(path[path.size() - 1][0]) - _strike) > 0) ? 1. : 0.;
	}
	else {
		return (mult * (path[path.size() - 1][0] - _strike) > 0) ? 1. : 0.;
	}
}

AmericanOptionPayoff * AmericanOptionPayoff::clone() const
{
	return new AmericanOptionPayoff(*this);
}

AmericanOptionPayoff::AmericanOptionPayoff(const CALL_PUT & call_put, const double & strike)
	: _call_put(call_put), _strike(strike)
{
}

double AmericanOptionPayoff::payoff(const Matrix & path, bool is_log) const
{
	double mult = (_call_put == CALL_PUT::CALL) ? 1. : -1.;

	double payoff_max = 0.;
	for (size_t idx = 0; idx < path.size(); idx++)
	{
		double current_payoff = std::max(mult * (std::exp(path[idx][0]) - _strike), 0.) ? is_log : std::max(mult * (path[idx][0] - _strike), 0.);
		if (current_payoff > payoff_max)
			payoff_max = current_payoff;
	}
	return payoff_max;
}

AmericanBarrierOptionPayoff * AmericanBarrierOptionPayoff::clone() const
{
	return new AmericanBarrierOptionPayoff(*this);
}

AmericanBarrierOptionPayoff::AmericanBarrierOptionPayoff(const CALL_PUT & call_put, const double & strike, const double & barrier_level, const BARRIER_TYPE & barrier_type)
	: AmericanOptionPayoff(call_put, strike), _barrier_level(barrier_level), _barrier_type(barrier_type)
{
}

double AmericanBarrierOptionPayoff::payoff(const Matrix & path, bool is_log) const
{
	// Exercise
	return 0.;
}
