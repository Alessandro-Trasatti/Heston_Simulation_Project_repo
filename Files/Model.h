#pragma once

// An abstract class is a class which contains at least one virtual pure method
class Model
{
public:
	Model(const double& initial_value);

	// Copy constructor
	Model(const Model& model);

	// clone method -> abstract class that is used by another class through a pointer
	virtual Model* clone() const = 0;

	// Assignment operator
	Model& operator=(const Model& model);

	// Destructor
	virtual ~Model() = default;

	// getter
	double init_value() const;

	virtual double drift_term(const double& time, const double& asset_price) const = 0;
	virtual double diffusion_term(const double& time, const double& asset_price) const = 0;

protected:
	double _initial_value;
};

// Public Inheritance :
// public -> public
// protected -> private 
// private -> unaccessible

class BlackScholesModel : public Model
{
public:
	BlackScholesModel(const double& initial_value, const double& drift, const double& volatility);

	BlackScholesModel(const BlackScholesModel& model);

	BlackScholesModel* clone() const override;

	BlackScholesModel& operator=(const BlackScholesModel& model);

	~BlackScholesModel() = default;

	double drift_term(const double& time, const double& asset_price) const override;
	double diffusion_term(const double& time, const double& asset_price) const override;

private:
	double _drift; // mu
	double _volatility; // sigma
};

class HestonModel : public Model
{
public:
	HestonModel(
		const double& initial_spot,
		const double& initial_variance,
		const double& drift,
		const double& mean_reversion_speed,
		const double& mean_reversion_level,
		const double& vol_of_vol,
		const double& correlation);
	HestonModel(const HestonModel& model);
	HestonModel* clone() const override;
	HestonModel& operator=(const HestonModel& model);
	~HestonModel() = default;
	// getter methods
	double initial_spot() const;
	double initial_variance() const;
	double drift() const;
	double mean_reversion_speed() const;
	double mean_reversion_level() const;
	double vol_of_vol() const;
	double correlation() const;
private:
	double _initial_variance; // V_0
	double _drift; // rate r
	double _mean_reversion_speed; // kappa
	double _mean_reversion_level; // theta
	double _vol_of_vol; // sigma_V
	double _correlation; // rho
};
