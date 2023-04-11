#include "PathSimulator.h"

#include <chrono>
#include <random>

PathSimulator::PathSimulator(const Model & model, const double & maturity, const size_t & size)
	: _model(model.clone())
{
	for (size_t idx = 0; idx < size; idx++)
		_time_points.push_back(idx * maturity / (size - 1));
}

PathSimulator::PathSimulator(const Model & model, const Vector & time_points)
	: _model(model.clone()), _time_points(time_points)
{
}

PathSimulator::PathSimulator(const PathSimulator & path_simulator)
	: _model((path_simulator._model)->clone()), _time_points(path_simulator._time_points)
{
}

PathSimulator & PathSimulator::operator=(const PathSimulator & path_simulator)
{
	if (this != &path_simulator)
	{
		_model = (path_simulator._model)->clone();
		_time_points = path_simulator._time_points;
	}
	return *this;
}

PathSimulator::~PathSimulator()
{
	delete _model;
}

Vector PathSimulator::path() const
{
	Vector path;
	path.push_back(_model->init_value()); // (*_model).init_value()

	for (size_t time_idx = 1; time_idx < _time_points.size(); time_idx++)
	{
		// next step : S_{t_{k+1}} = next_step(t_k, S_{t_k})
		double asset_price = path[time_idx - 1];
		double next_value = next_step(time_idx - 1, asset_price);
		path.push_back(next_value);
	}
	return path;

}

double PathSimulator::expiry() const
{
	return _time_points[_time_points.size() - 1];
}

EulerPathSimulator * EulerPathSimulator::clone() const
{
	return new EulerPathSimulator(*this);
}

EulerPathSimulator::EulerPathSimulator(const Model & model, const double & maturity, const size_t & size)
	: PathSimulator(model, maturity, size)
{
}

EulerPathSimulator::EulerPathSimulator(const Model & model, const Vector & time_points)
	: PathSimulator(model, time_points)
{
}

double EulerPathSimulator::next_step(const size_t & time_idx, const double & asset_price) const
{
	// S_{t_k}
	double next_asset_price = asset_price;                                                      
	
	// DT = t_{k+1} - t_k
	double delta_time = _time_points[time_idx + 1] - _time_points[time_idx];

	// + DT * mu(t_k, S_{t_k})
	next_asset_price += delta_time * _model->drift_term(_time_points[time_idx], asset_price);  

	std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	std::normal_distribution<double> distribution(0., 1.);
	double delta_brownian = std::sqrt(delta_time) * distribution(generator);

	// + sigma(t_k, S_{t_k}) * sqrt(DT) * N(0,1)
	next_asset_price += _model->diffusion_term(_time_points[time_idx], asset_price) * delta_brownian; 

	return next_asset_price;
}
