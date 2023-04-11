#include "Model.h"

Model::Model(const double & initial_value)
	: _initial_value(initial_value)
{
}

Model::Model(const Model & model)
	: _initial_value(model._initial_value)
{
}

Model & Model::operator=(const Model & model)
{
	if (this != &model)
		_initial_value = model._initial_value;
	return *this;
}



double Model::init_value() const
{
	return _initial_value;
}

BlackScholesModel::BlackScholesModel(const double & initial_value, const double & drift, const double & volatility)
	: Model(initial_value), _drift(drift), _volatility(volatility)
{
}

BlackScholesModel::BlackScholesModel(const BlackScholesModel & model)
	: Model(model), _drift(model._drift), _volatility(model._volatility)
{
}

BlackScholesModel * BlackScholesModel::clone() const
{
	return new BlackScholesModel(*this);
}

BlackScholesModel & BlackScholesModel::operator=(const BlackScholesModel & model)
{
	if (this != &model)
	{
		Model::operator=(model);
		_drift = model._drift;
		_volatility = model._volatility;
	}
	return *this;
}

double BlackScholesModel::drift_term(const double & time, const double & asset_price) const
{
	return _drift * asset_price;
}

double BlackScholesModel::diffusion_term(const double & time, const double & asset_price) const
{
	return _volatility * asset_price;
}

HestonModel::HestonModel(const double& initial_spot, const double& initial_variance, const double& drift, const double& mean_reversion_speed, const double& mean_reversion_level, const double& vol_of_vol, const double& correlation)
	: Model(initial_spot), _initial_variance(initial_variance), _drift(drift), _mean_reversion_speed(mean_reversion_speed), _mean_reversion_level(mean_reversion_level), _vol_of_vol(vol_of_vol), _correlation(correlation)
{
}

HestonModel::HestonModel(const HestonModel& model)
	: Model(model._initial_spot), _initial_variance(model._initial_variance), _drift(model._drift), _mean_reversion_speed(model._mean_reversion_speed), _mean_reversion_level(model._mean_reversion_level), _vol_of_vol(model._vol_of_vol), _correlation(model._correlation)
{
}

HestonModel& HestonModel::operator=(const HestonModel& model)
{
	if (this != &model) {
		Model::operator=(model);
		_initial_variance = model._initial_variance;
		_drif = model._drift;
		_mean_reversion_speed = model._mean_reversion_speed;
		_mean_reversion_level = model._mean_reversion_level;
		_vol_of_vol = model._vol_of_vol;
		_correlation = model._correlation;
	}
	return *this;
}

double HestonModel::initial_spot() const
{
	return _initial_spot;
}

double HestonModel::initial_variance() const
{
	return _initial_variance;
}

double HestonModel::drift() const
{
	return _drift;
}

double HestonModel::mean_reversion_speed() const
{
	return _mean_reversion_speed;
}

double HestonModel::mean_reversion_level() const
{
	return _mean_reversion_level;
}

double HestonModel::vol_of_vol() const
{
	return _vol_of_vol;
}

double HestonModel::correlation() const
{
	return _correlation;
}
HestonModel* HestonModel::clone() const
{
	return new HestonModel(*this);
}