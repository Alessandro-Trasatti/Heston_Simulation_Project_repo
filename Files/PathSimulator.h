#pragma once

#include "HestonModel.h"
#include "MathTools.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <chrono>
#include <random>

using namespace std::placeholders;
using Vector = std::vector<double>;
using Matrix = std::vector<std::vector<double>>;

class PathSimulator
{
public:
	virtual PathSimulator* clone() const = 0;

	// Two constructors with parameter

	// Given a maturity and a size, which models the number of points, it creates a path evaluated on a grid of equispaced points between 0 and T
	PathSimulator(const HestonModel& model, const double& maturity, const size_t& size);
	// Creates a path evaluated on a grid constituted by the time points given as input
	PathSimulator(const HestonModel& model, const Vector& time_points);

	// Copy constructor
	PathSimulator(const PathSimulator& path_simulator);

	// assignment operator
	PathSimulator& operator=(const PathSimulator& path_simulator);

	// destructor
	virtual ~PathSimulator() = default;
	
	// Returns the path of the spot and the of the vol; it is now a Matrix, where path[k][0] = S_{t_k} and path[k][1] = V_{t_k}
	Matrix path() const;

	// Useful getter
	double expiry() const;

protected:
	// Codes the way with which we move to the next step of a general (virtual) discretization scheme - returns the variance and the spot
	virtual Vector next_step(const size_t& time_idx, const double& asset_price, const double &variance) const = 0;


	// If you put a const here you can't define an assignement operator and write "_model = path_simulator._model"
	HestonModel _model;
	Vector _time_points;
};

class EulerPathSimulatorModified : public PathSimulator
{
public:
	EulerPathSimulatorModified* clone() const override;
	EulerPathSimulatorModified(const HestonModel& model, const double& maturity, const size_t& size);
	EulerPathSimulatorModified(const HestonModel& model, const Vector& time_points);

private:
	//returns the variance and the spot using the truncated Euler discretization scheme.
	Vector next_step(const size_t& time_idx, const double& asset_price, const double &variance) const override;
};

class BroadieKaya : public PathSimulator
{
public:
	BroadieKaya* clone() const override;
	BroadieKaya(const HestonModel& model, const double& maturity, const size_t& size, const MathTools& tools, const bool &tg);
	BroadieKaya(const HestonModel& model, const Vector& time_points, const MathTools& tools, const bool &tg);
	double truncature_gaussian(const double& variance, int n_iterations_secant_method = 10) const;
	double quadratic_exponential(const double& variance, const double& psi_threshold = 1.5, int n_iterations_secant_method = 10) const;
private:
	// This attribute contains all the needed mathematical functions and algorithms to carry out the schemes.
	MathTools _tools;
	bool _tg;
	// Returns the variance and the spot using the BroadieKaya discretization scheme.
	Vector next_step(const size_t& time_idx, const double& asset_price, const double& variance) const override;
};