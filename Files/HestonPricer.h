#include "HestonModel.h"
#include "Complex.h"
#include "GaussLegendreQuadrature.h"
#include<vector>

using Vector = std::vector<double>;
using Pair = std::pair<double, double>;
using PairVector = std::vector<Pair>;

class HestonPricer
{
public:
    HestonPricer(const HestonModel& model, const double& strike, const double& maturity);

    //virtual clone method
    virtual HestonPricer* clone() const = 0;

    //Method to compute option price. It will depend on the subclass. (It could be analytical or by MonteCrlo method).
    //So it is a virtual method
    virtual double price() const = 0;

protected: //To have subclasses
    HestonModel _model;

    //Strike and Maturity of the option
    double _strike;
    double _maturity;
};

class AnalyticalHestonPricer : public HestonPricer
{
public:

    AnalyticalHestonPricer(const HestonModel& model, const double& strike,
                           const double& maturity, const size_t& gauss_legendre_sample_size);

    //clone method
    AnalyticalHestonPricer* clone() const override;


    double price() const override;

private:

    Complex func_u(const size_t& i) const;
    Complex func_y(const size_t& i, const double& omega) const;
    Complex func_a(const size_t& i, const double& omega) const;
    Complex func_delta(const size_t& i, const double& omega) const;
    Complex func_b(const size_t& i, const double& omega) const;
    Complex func_g(const size_t& i, const double& omega) const;

    Complex func_C(const size_t& i, const double& tau, const double& omega) const;
    Complex func_D(const size_t& i, const double& tau, const double& omega) const;

    Complex func_Phi(const size_t& i, const double& tau, const double& x, const double& V, const double& omega) const;

    double func_P(const size_t& i, const double& tau, const double& x, const double& V) const;

    double func_integrand(const size_t& i, const double& tau, const double& x, const double& V, const double& omega) const;


    size_t _gauss_legendre_sample_size;
};