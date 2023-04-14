#pragma once
#ifndef HESTONMODEL_H
#define HESTONMODEL_H

#include <iostream>

class HestonModel
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
    //Assignement operator
    HestonModel& operator=(const HestonModel& model);
    // Getter methods
    double initial_spot() const;
    double initial_variance() const;
    double drift() const;
    double mean_reversion_speed() const;
    double mean_reversion_level() const;
    double vol_of_vol() const;
    double correlation() const;
    // Drift terms for both the spot and the variance process.
    double drift_term_spot(const double& asset_price) const;
    double drift_term_variance(const double& variance) const;

    // Diffusion terms for both the spot and the variance process.
    double diffusion_term_spot(const double& asset_price, const double &variance) const;
    double diffusion_term_variance(const double& asset_price, const double &variance) const;
private:
    double _initial_spot;          // S_0  
    double _initial_variance;      // V_0
    double _drift;                 // r, the rate
    double _mean_reversion_speed;  // \kappa
    double _mean_reversion_level;  // \theta
    double _vol_of_vol;            // \sigma_V
    double _correlation;           // \rho
};
#endif