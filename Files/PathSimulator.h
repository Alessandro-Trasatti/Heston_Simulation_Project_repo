#pragma once

#include "HestonModel.h"
#include <vector>

using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

class PathSimulator
{
public:
	virtual PathSimulator* clone() const = 0;

	// 2 constructors with parameter
	PathSimulator(const HestonModel& model, const double& maturity, const size_t& size);
	PathSimulator(const HestonModel& model, const Vector& time_points);

	// copy constructor
	PathSimulator(const PathSimulator& path_simulator);

	// assignment operator
	PathSimulator& operator=(const PathSimulator& path_simulator);

	// destructor
	virtual ~PathSimulator();
	
	// return the path of the spot and the vol, it is now a Matrix, path[k][0] = S_{t_k}, path[k][1] = V_{t_k}
	Matrix path() const;

	// useful getter
	double expiry() const;

protected:
	//we now need the value of the vol to go to the next step
	virtual Vector next_step(const size_t& time_idx, const double& asset_price, const double &vol) const = 0;

	const HestonModel* _model;
	Vector _time_points;
};

class EulerPathSimulator : public PathSimulator
{
public:
	EulerPathSimulator* clone() const override;
	EulerPathSimulator(const HestonModel& model, const double& maturity, const size_t& size);
	EulerPathSimulator(const HestonModel& model, const Vector& time_points);

private:
	//returns the vol and the spot
	Vector next_step(const size_t& time_idx, const double& asset_price, const double &vol) const override;
};