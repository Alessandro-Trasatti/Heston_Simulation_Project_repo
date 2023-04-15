#pragma once
#include <vector>
#include <iostream>
#include <algorithm>

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

class Payoff
{
public:
	virtual Payoff* clone() const = 0;
	virtual double payoff(const Matrix& path) const = 0;
};

// enum class
enum class CALL_PUT
{
	CALL,
	PUT
};


// EuropanOptionPayoff

class EuropeanOptionPayoff : public Payoff
{
public:
	EuropeanOptionPayoff* clone() const override;
	EuropeanOptionPayoff(const CALL_PUT& call_put, const double& strike);
	double payoff(const Matrix& path) const override;

protected:
	CALL_PUT _call_put;
	double _strike;
};


// DigitalOptionPayoff
class DigitalOptionPayoff : public EuropeanOptionPayoff
{
public:
	DigitalOptionPayoff* clone() const override;
	DigitalOptionPayoff(const CALL_PUT& call_put, const double& strike);
	double payoff(const Matrix& path) const override;
};


class AmericanOptionPayoff : public Payoff
{
public:
	AmericanOptionPayoff* clone() const override;
	AmericanOptionPayoff(const CALL_PUT& call_put, const double& strike);
	double payoff(const Matrix& path) const override;

protected:
	CALL_PUT _call_put;
	double _strike;
};

enum class BARRIER_TYPE
{
	KI,  // Knock In
	KO   // Knock Out
};

class AmericanBarrierOptionPayoff : public AmericanOptionPayoff
{
public:
	AmericanBarrierOptionPayoff* clone() const override;
	AmericanBarrierOptionPayoff(const CALL_PUT& call_put, const double& strike, const double& barrier_level, const BARRIER_TYPE& barrier_type);
	double payoff(const Matrix& path) const override;

private:
	double _barrier_level;
	BARRIER_TYPE _barrier_type;
};