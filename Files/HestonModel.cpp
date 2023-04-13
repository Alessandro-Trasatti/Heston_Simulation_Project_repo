#include "HestonModel.h"
#include <cmath> // Needed for mathematical functions like sqrt

#
HestonModel::HestonModel(const double& initial_spot, const double& initial_variance, const double& drift, const double& mean_reversion_speed, const double& mean_reversion_level, const double& vol_of_vol, const double& correlation)
    : _initial_spot(initial_spot), _initial_variance(initial_variance), _drift(drift), _mean_reversion_speed(mean_reversion_speed), _mean_reversion_level(mean_reversion_level), _vol_of_vol(vol_of_vol), _correlation(correlation)
{
}
double HestonModel::initial_spot() const
{
    return _initial_spot;
}
double HestonModel::initial_variance() const
{
    return _initial_variance;
}
double HestonModel::drift() const
{
    return _drift;
}
double HestonModel::mean_reversion_speed() const
{
    return _mean_reversion_speed;
}
double HestonModel::mean_reversion_level() const
{
    return _mean_reversion_level;
}
double HestonModel::vol_of_vol() const
{
    return _vol_of_vol;
}
double HestonModel::correlation() const
{
    return _correlation;
}
double HestonModel::drift_term_spot(const double& asset_price) const
{
    return _drift * asset_price;
}
double HestonModel::diffusion_term_spot(const double& asset_price, const double& variance) const
{
    if (variance < 0) {
        throw std::invalid_argument("received negative value for the variance");
    }
    else {
        return sqrt(variance) * asset_price;
    }
}
double HestonModel::drift_term_variance(const double& variance) const
{
    if (variance < 0) {
        throw std::invalid_argument("received negative value for the variance");
    }
    else {
        return _mean_reversion_speed * (_mean_reversion_level - variance);
    }
}
double HestonModel::diffusion_term_variance(const double& asset_price, const double& variance) const
{
    if (variance < 0) {
        throw std::invalid_argument("received negative value for the variance");
    }
    else {
        return _vol_of_vol * sqrt(variance);
    }
}