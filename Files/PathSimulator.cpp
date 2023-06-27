#include "PathSimulator.h"

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

/*
to be changed, right now at each step we utilize the scheme on the log, we convert it to the asset and then we take the log again 
*/

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
// As the derived class doesn't add any attribute, the constructor is basically the same than the one of PathSimulator

BroadieKaya* BroadieKaya::clone() const
{
	return new BroadieKaya(*this);
}

BroadieKaya::BroadieKaya(const HestonModel& model, const double& maturity, const size_t& size, const MathTools& tools, const bool &tg, const double &gamma_1)
	: PathSimulator(model, maturity, size), _tools(tools), _tg(tg)
{
	if (gamma_1 > 1 || gamma_1 < 0) {
		throw "gamma must belong to [0,1]";
	}
	else {
		_gamma_1 = gamma_1;
		_gamma_2 = 1 - gamma_1;
		_delta_t = _time_points[1] - _time_points[0];
		std::cout << _delta_t << std::endl;
		_k_0 = - model.correlation() * model.mean_reversion_level() * model.mean_reversion_speed() / model.vol_of_vol();
		_k_1 = gamma_1 * _delta_t * (model.mean_reversion_speed() * model.correlation() / model.vol_of_vol() - 0.5) - model.correlation() / model.vol_of_vol();
		_k_2 = _gamma_2 * _delta_t * (model.mean_reversion_speed() * model.correlation() / model.vol_of_vol() - 0.5) + model.correlation() / model.vol_of_vol();
		_k_3 = gamma_1 * _delta_t * (1 - model.correlation() * model.correlation());
		_k_4 = _gamma_2 * _delta_t * (1 - model.correlation() * model.correlation());
		_r = model.drift();
	}
}

BroadieKaya::BroadieKaya(const HestonModel& model, const Vector& time_points, const MathTools& tools,const bool &tg, const double& gamma_1)
	: PathSimulator(model, time_points), _tools(tools), _tg(tg)
{
	if (gamma_1 > 1 || gamma_1 < 0) {
		throw "gamma must belong to [0,1]";
	}
	else {
		_gamma_1 = gamma_1;
		_gamma_2 = 1 - gamma_1;
		_delta_t = _time_points[1] - _time_points[0];
		_k_0 = -_delta_t * model.correlation() * model.mean_reversion_level() * model.mean_reversion_speed() / model.vol_of_vol();
		_k_1 = gamma_1 * _delta_t * (model.mean_reversion_speed() * model.correlation() / model.vol_of_vol() - 0.5) - model.correlation() / model.vol_of_vol();
		_k_2 = _gamma_2 * _delta_t * (model.mean_reversion_speed() * model.correlation() / model.vol_of_vol() - 0.5) + model.correlation() / model.vol_of_vol();
		_k_3 = gamma_1 * _delta_t * (1 - model.correlation() * model.correlation());
		_k_4 = _gamma_2 * _delta_t * (1 - model.correlation() * model.correlation());
		_r = model.drift();
	}
}

double BroadieKaya::truncature_gaussian(const double &variance, int n_iterations_secant_method) const
{
    double theta = _model.mean_reversion_level();
	double k = _model.mean_reversion_speed();
	double sigma_v = _model.vol_of_vol();
	double Delta_t = _time_points[1] - _time_points[0];
	double discounting_factor = std::exp(-k * Delta_t);

	// theta + (V - theta) * e^{-k Delta_t}
	double m = theta + (variance - theta) * discounting_factor;
	// ((V * sigma_v^2 e^{-k Delta_t}) / k) * (1 - e^{-k Delta_t}) + (theta * sigma_v^2) / (2 k) * (1- e^{-k Delta_t})^2
	double s_squared = (variance * sigma_v * sigma_v * discounting_factor / k) * (1- discounting_factor) + (theta * sigma_v * sigma_v/ (2 * k)) * (1- discounting_factor) * (1- discounting_factor);
	double psi = s_squared/(m * m);
	// Weird thing to store a function that we have to pass as an argument for the secant method.
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

double BroadieKaya::quadratic_exponential(const double& variance, const double &psi_threshold, int n_iterations_secant_method) const
{
	double next_variance;
	double theta = _model.mean_reversion_level();
	double k = _model.mean_reversion_speed();
	double sigma_v = _model.vol_of_vol();
	double Delta_t = _time_points[1] - _time_points[0];
	double discounting_factor = std::exp(-k * Delta_t);

	// theta + (V - theta) * e^{-k Delta_t}
	double m = theta + (variance - theta) * discounting_factor;
	// ((V * sigma_v^2 e^{-k Delta_t}) / k) * (1 - e^{-k Delta_t}) + (theta * sigma_v^2) / (2 k) * (1- e^{-k Delta_t})^2
	double s_squared = (variance * sigma_v * sigma_v * discounting_factor / k) * (1 - discounting_factor) + (theta * sigma_v * sigma_v / (2 * k)) * (1 - discounting_factor) * (1 - discounting_factor);
	// (s/m)^2
	double psi = s_squared / (m * m);
	if (psi < psi_threshold) {
		// (2 / psi) - 1 + sqrt{(2/psi)} * sqrt{(2/psi) - 1}
		double b_squared = (2 / psi) - 1 + std::sqrt(2 / psi) * std::sqrt((2 / psi) - 1);
		// m / (1 + b^2)
		double a = (m / (1 + b_squared));
		std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
		std::normal_distribution<double> distribution(0., 1.);
		double Z_V = distribution(generator);
		next_variance = a * (std::sqrt(b_squared) + Z_V) * (std::sqrt(b_squared) + Z_V);
	}
	else {
		// (psi - 1) / (psi + 1)
		double p = (psi - 1) / (psi + 1);
		// 2 / (m * (psi + 1))
		double beta = 2 / (m * (psi + 1));
		// Draw of a (0,1) uniform 
		std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
		std::uniform_real_distribution<double> distribution(0., 1.);
		double U_V = distribution(generator);
		next_variance = (U_V < p) ? 0 : std::log((1 - p) / (1 - U_V));
	}

	return next_variance;
}

Vector BroadieKaya::next_step(const size_t& time_idx, const double& asset_price, const double& variance) const
{
	Vector next_values;
	std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	std::normal_distribution<double> distribution(0., 1.);
	//according to the attribute tg, we choose the scheme for the variance
	double next_variance = (_tg) ? truncature_gaussian(variance) : quadratic_exponential(variance);
	double log_asset_price = std::log(asset_price);
	log_asset_price += _k_0 + _k_1 * variance + _k_2 * next_variance + std::sqrt(_k_3 * variance + _k_4 * next_variance) * distribution(generator);
	next_values.push_back(std::exp(log_asset_price));
	next_values.push_back(next_variance);
	return next_values;
}

/*
Vector BroadieKaya::next_step(const size_t &time_idx, const double &asset_price, const double &variance) const
{
	Vector next_values;
	double theta = _model.mean_reversion_level();
	double k = _model.mean_reversion_speed();
	double r = _model.drift();
	double sigma_v = _model.vol_of_vol();
	double rho = _model.correlation();
	double delta_t = _time_points[1] - _time_points[0];
	std::mt19937 generator = std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());
	std::normal_distribution<double> distribution(0., 1.);
	//according to the attribute tg, we choose the scheme for the variance
	double next_variance = (_tg) ? truncature_gaussian(variance) : quadratic_exponential(variance);
	double log_asset_price = std::log(asset_price);
	// (rho / sigma_v) * (V_{t + Delta t} - V_t - k * theta * delta)
	double integrale_var = _tools.trapezoidalMethod(variance, next_variance, delta_t);
	log_asset_price += r * delta_t + (rho / sigma_v) * (next_variance - variance - k * theta * delta_t);
	// Delta t * (((k * rho) / sigma_v) - (1/2)) * int_t^{t+ Delta t} V_s ds
	log_asset_price += ((k * rho) / sigma_v - 0.5) * integrale_var;
	// sqrt(1 + rho^2) * int_{t}^{t + \Delta t} sqrt(V_s) dWs
	log_asset_price += std::sqrt(1 - (rho * rho)) * std::sqrt(integrale_var) * distribution(generator);
	next_values.push_back(std::exp(log_asset_price));
	next_values.push_back(next_variance);
	return next_values;
}
*/