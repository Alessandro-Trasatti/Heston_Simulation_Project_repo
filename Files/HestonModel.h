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
    //clone
    HestonModel* clone() const;
    // Getter methods
    double initial_spot() const;
    double initial_variance() const;
    double drift() const;
    double mean_reversion_speed() const;
    double mean_reversion_level() const;
    double vol_of_vol() const;
    double correlation() const;
    // Computations of the drift terms for both the vol and the spot.
    double drift_term_spot(const double& asset_price) const;
    double diffusion_term_spot(const double& asset_price, const double &vol) const;
    double drift_term_vol(const double& asset_price) const;
    double diffusion_term_vol(const double& asset_price, const double &vol) const;
private:
    double _initial_spot;          // S_0  
    double _initial_variance;      // V_0
    double _drift;                 // r, the rate
    double _mean_reversion_speed;  // kappa
    double _mean_reversion_level;  // theta
    double _vol_of_vol;            // sigma_V
    double _correlation;           // rho
};
#endif