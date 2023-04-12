#pragma once

// Defines an abstract class of a general diffusive model
class Model
{
public:
	// Parameter constructor
	Model(const double& initial_value);

	// Copy constructor
	Model(const Model& model);

	// Clone method - This needs to be implemented as Model is an abstract class and the class PathSimulator has a pointer to an object of type model as attribute 
	virtual Model* clone() const = 0;

	// Assignment operator
	Model& operator=(const Model& model);

	// Destructor - Bound to be virtual as other classes will inherit from this
	virtual ~Model() = default;

	// getter of S0
	double init_value() const;

	// \mu(t,S_t)
	virtual double drift_term(const double& time, const double& asset_price) const = 0;

	// \sigma(t,S_t)
	virtual double diffusion_term(const double& time, const double& asset_price) const = 0;

protected:
	double _initial_value;
};

class HestonModel : public Model
{
public:
	// Parameter constructor
	HestonModel(
		const double& initial_spot,
		const double& initial_variance,
		const double& drift,
		const double& mean_reversion_speed,
		const double& mean_reversion_level,
		const double& vol_of_vol,
		const double& correlation);
	// Copy constructor
	HestonModel(const HestonModel& model);

	// Clone method
	HestonModel* clone() const override; // TO OVERRIDE WE NEED TO INHERIT THE CLASS

	// Assignment operator
	HestonModel& operator=(const HestonModel& model);

	// Destructor
	~HestonModel() = default;

	// Getter methods
	double initial_spot() const;
	double initial_variance() const;
	double drift() const;
	double mean_reversion_speed() const;
	double mean_reversion_level() const;
	double vol_of_vol() const;
	double correlation() const;

	double drift_term(const double& time, const double& asset_price) const override;
	double diffusion_term(const double& time, const double& asset_price) const override;

private:
	double _initial_spot; // S0
	double _initial_variance; // V_0
	double _drift; // rate r
	double _mean_reversion_speed; // kappa
	double _mean_reversion_level; // theta
	double _vol_of_vol; // sigma_V
	double _correlation; // rho
};
