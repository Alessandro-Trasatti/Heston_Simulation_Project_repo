#pragma once

#include "Model.h"
#include <vector>


using Vector = std::vector<double>;

class PathSimulator
{
public:
	virtual PathSimulator* clone() const = 0;

	// 2 constructors with parameter
	PathSimulator(const Model& model, const double& maturity, const size_t& size);
	PathSimulator(const Model& model, const Vector& time_points);

	// copy constructor
	PathSimulator(const PathSimulator& path_simulator);

	// assignment operator
	PathSimulator& operator=(const PathSimulator& path_simulator);

	// destructor
	virtual ~PathSimulator();

	Vector path() const;

	// useful getter
	double expiry() const;

protected:
	virtual double next_step(const size_t& time_idx, const double& asset_price) const = 0;

	const Model* _model;
	Vector _time_points;
};

class EulerPathSimulator : public PathSimulator
{
public:
	EulerPathSimulator* clone() const override;
	EulerPathSimulator(const Model& model, const double& maturity, const size_t& size);
	EulerPathSimulator(const Model& model, const Vector& time_points);

private:
	double next_step(const size_t& time_idx, const double& asset_price) const override;
};