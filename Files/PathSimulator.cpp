#include "PathSimulator.h"

#include <chrono>
#include <random>

// Implementation of PathSimulator methods
PathSimulator::PathSimulator(const HestonModel & model, const double & maturity, const size_t & size)
	: _model(model) // Using the copy constructor of the class HestonModel
{
	for (size_t idx = 0; idx < size; idx++)
		_time_points.push_back(idx * maturity / (size - 1));
}

PathSimulator::PathSimulator(const HestonModel & model, const Vector & time_points)
	: _model(model), _time_points(time_points)
{
}

PathSimulator::PathSimulator(const PathSimulator & path_simulator)
	: _model(path_simulator._model), _time_points(path_simulator._time_points)
{
}

PathSimulator & PathSimulator::operator=(const PathSimulator & path_simulator)
{
	if (this != &path_simulator)
	{
		_model = path_simulator._model; // FIX! done
		_time_points = path_simulator._time_points;
	}
	return *this;
}

Matrix PathSimulator::path() const
{
	Matrix path;
	Vector current_values;
	current_values.push_back(_model.initial_spot());
	current_values.push_back(_model.initial_variance());
	path.push_back(current_values); // (*_model).initial_spot()

	for (size_t time_idx = 1; time_idx < _time_points.size(); time_idx++)
	{
		// next step : S_{t_{k+1}} = next_step(t_k, S_{t_k})
		double asset_price = path[time_idx - 1][0];
		double variance = path[time_idx - 1][1];
		current_values = next_step(time_idx - 1, asset_price, variance);
		path.push_back(current_values);
	}
	return path;

}

double PathSimulator::expiry() const
{
	return _time_points[_time_points.size() - 1];
}
// Implementation of EulerPathSimulatorModified methods
EulerPathSimulatorModified* EulerPathSimulatorModified::clone() const
{
	return new EulerPathSimulatorModified(*this);
}

EulerPathSimulatorModified::EulerPathSimulatorModified(const HestonModel & model, const double & maturity, const size_t & size)
	: PathSimulator(model, maturity, size)
{
}

EulerPathSimulatorModified::EulerPathSimulatorModified(const HestonModel & model, const Vector & time_points)
	: PathSimulator(model, time_points)
{
}

Vector EulerPathSimulatorModified::next_step(const size_t & time_idx, const double & asset_price, const double &variance) const
{
	Vector next_values;
	// DT = t_{k+1} - t_k
	double delta_time = _time_points[time_idx + 1] - _time_points[time_idx];

	//Euler on the variance
	
	// V_{t_k}
	double next_variance = variance;
	
	// + DT * k * (theta - S_{t_k})
	next_variance += delta_time * _model.drift_term_variance(std::max(variance,0.0)); // Here a += was missing, RIGHT?!

	// sqrt(DT) * (W_{t_{k+1}} - W_{t_k})
	std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	std::normal_distribution<double> distribution(0., 1.);
	double delta_brownian_variance = std::sqrt(delta_time) * distribution(generator);

	// CHECK!!!
	next_variance += _model.diffusion_term_variance(asset_price, std::max(variance, 0.0)) * delta_brownian_variance; // Here a += was missing, RIGHT?!
	
	// Euler on the spot
 
	// S_{t_k}
	double log_next_asset_price = log(asset_price);

	// drift of ln(S_t) -0.5 * V_{t_k} * delta_t
	log_next_asset_price -= 0.5 * delta_time * std::max(variance,0.0);

	//sqrt(DT) * (W_{t_{k+1}} - W_{t_k}) with a new brownian independant
	double delta_brownian_spot_independant = std::sqrt(delta_time) * distribution(generator);
	double delta_browian_log_spot = (_model.correlation() * delta_brownian_variance + std::sqrt(1 - _model.correlation() * _model.correlation()) * delta_brownian_spot_independant);

	// + sqrt(V(t_k, S_{t_k})) * sqrt(DT) * rho * (W^1_{t_{k+1}} - W^1_{t_k}) + sqrt(1 - rho**2) * (W^2_{t_{k+1}} - W^2_{t_k})
	log_next_asset_price += std::sqrt(std::max(variance,0.0)) * delta_browian_log_spot;
	//we go back to the asset price taking the exponential 
	next_values.push_back(std::exp(log_next_asset_price));
	next_values.push_back(next_variance);
	return next_values;
}
// Implementation of BroadieKaye methods
BroadieKaya* BroadieKaya::clone() const
{
	return new BroadieKaya(*this);
}

// As the derived class doesn't add any attribute, the constructor is basically the same than the one of PathSimulator

BroadieKaya::BroadieKaya(const HestonModel& model, const double& maturity, const size_t& size, const MathTools& tools)
	: PathSimulator(model, maturity, size), _tools(tools)
{
}

BroadieKaya::BroadieKaya(const HestonModel& model, const Vector& time_points, const MathTools& tools)
	: PathSimulator(model, time_points), _tools(tools)
{
}
/*
double BroadieKaya::eq_r(double r, double psi, MathTools tools) {
	return (r * tools.normalPDF(r)) + tools.normalCDF(r) * (1 + r * r) - (1 + psi) * (tools.normalPDF(r) + r * tools.normalCDF(r)) * (tools.normalPDF(r) + r * tools.normalCDF(r));
}
*/
double BroadieKaya::truncature_Gaussian(const double &variance, int n_iterations_secant_method)
{
    double theta = _model.mean_reversion_level();
	double k = _model.mean_reversion_speed();
	double sigma_v = _model.vol_of_vol();
	double Delta_t = _time_points[1] - _time_points[0];
	double discounting_factor = std::exp(-k * Delta_t);

	// Write formula of m
	double m = theta + (variance - theta) * discounting_factor;
	// Write formula of s^2
	double s_squared = (variance * sigma_v * sigma_v * discounting_factor / k) * (1- discounting_factor) + (theta * sigma_v * sigma_v/ (2 * k)) * (1- discounting_factor) * (1- discounting_factor);
	double psi = s_squared/(m * m);
	auto eqr = std::bind(&MathTools::eq_r, _tools, _1, _2);
	double r = _tools.secantMethod(n_iterations_secant_method, psi, eqr);
	double mu = m  * (r / (_tools.normalPDF(r) + r * _tools.normalCDF(r)));
	double sigma_tg = std::sqrt(s_squared) / (std::sqrt(psi) * (_tools.normalPDF(r) + r * _tools.normalCDF(r)));

	//Draw of a centered reduced gaussian
	std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	std::normal_distribution<double> distribution(0., 1.);
	double Z_V = distribution(generator);
	return std::max(mu + sigma_tg * Z_V, 0.0);
}

Vector BroadieKaya::next_step(const size_t &time_idx, const double &asset_price, const double &variance) const
{
	Vector next_values;
	return next_values;
}
