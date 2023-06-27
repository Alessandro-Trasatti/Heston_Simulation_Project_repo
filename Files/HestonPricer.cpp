#include "HestonPricer.h"

//------------------------------Implementation of methods for the class HestonPricer--------------------------------------
HestonPricer :: HestonPricer(const HestonModel& model, const double& strike, const double& maturity)
                        : _model(model), _strike(strike), _maturity(maturity)
                    {
                    }

//------------------------------Implementation of methods for the (sub)class AnalyticalHestonPricer--------------------------------------
//Constructor with parameters
AnalyticalHestonPricer :: AnalyticalHestonPricer(const HestonModel& model, const double& strike,
                                                 const double& maturity, const size_t& gauss_legendre_sample_size)
                                : HestonPricer(model, strike, maturity), _gauss_legendre_sample_size(gauss_legendre_sample_size)
                            {

                            }

//clone method
AnalyticalHestonPricer* AnalyticalHestonPricer :: clone() const
                                                    {
                                                        return new AnalyticalHestonPricer(*this);
                                                    }

//Implementation of methods in Private
Complex AnalyticalHestonPricer :: func_u(const size_t & i) const
{
    double u = (i == 1) ? -1. : 1.;
    return Complex(u);
}
Complex AnalyticalHestonPricer :: func_y(const size_t & i, const double& omega) const
{
double y_real = (i == 1) ? -1. : 0.;
return Complex(y_real, omega);
}
Complex AnalyticalHestonPricer :: func_a(const size_t & i, const double& omega) const
{
    Complex kappa(_model.mean_reversion_speed());
    Complex rho(_model.correlation());
    Complex sigma_v(_model.vol_of_vol());
    Complex y = func_y(i, omega);

    return kappa - rho * sigma_v * y;
}

Complex AnalyticalHestonPricer :: func_delta(const size_t & i, const double& omega) const
{
    Complex a = func_a(i, omega);
    Complex sigma_v(_model.vol_of_vol());
    Complex u = func_u(i);
    Complex j(0., 1.);
    Complex omega_c(omega);
    return a * a + sigma_v * sigma_v * (u * j * omega_c + omega_c * omega_c);
}
Complex AnalyticalHestonPricer :: func_b(const size_t & i, const double& omega) const
{
    return Complex::square_root(func_delta(i, omega));
}

Complex AnalyticalHestonPricer :: func_g(const size_t & i, const double& omega) const
{
    Complex a = func_a(i, omega);
    Complex b = func_b(i, omega);
    return (a - b) / (a + b);
}

Complex AnalyticalHestonPricer :: func_D(const size_t & i, const double & tau, const double & omega) const
{
    //std:: cout << "In func_D entra\n\n";
    Complex tau_c(tau);
    Complex sigma_v(_model.vol_of_vol());
    Complex a = func_a(i, omega);
    Complex b = func_b(i, omega);
    Complex g = func_g(i, omega);
    Complex res = (a - b) * (Complex(1.) - Complex::exponential(Complex(-1.)* b*tau_c)) / 
                  (sigma_v*sigma_v * (Complex(1.) - g * Complex::exponential(Complex(-1.)* b*tau_c)));
    
    //std:: cout << "module in Func_D:" << res.module() << "\n\n";
    return res;
}

Complex AnalyticalHestonPricer :: func_C(const size_t & i, const double & tau, const double & omega) const
{
    //std:: cout << "In func_C entra\n\n";
    Complex first_addend(0, omega * _model.drift() * tau);

    Complex unit(1); //this is number one
    Complex tau_c(-tau);
    Complex numerator = unit - func_g(i, omega) * Complex :: exponential(func_b(i, omega) * tau_c);
    Complex denominator = unit - func_g(i, omega);
    Complex second_addend = (func_a(i, omega) - func_b(i, omega)) * tau - 
                            Complex(2) * Complex :: logarithm(numerator.operator/(denominator));

    //std:: cout << "Division in func_C:" << numerator.operator/(denominator).module() << "\n";
    Complex kappa(_model.mean_reversion_speed());
    Complex theta(_model.mean_reversion_level());
    Complex sigma_v(_model.vol_of_vol());

    //std:: cout << "module in Func_C: " << (first_addend + (kappa * theta) / (sigma_v * sigma_v) * second_addend).module() << "\n";
    return first_addend + (kappa * theta) / (sigma_v * sigma_v) * second_addend;
}

Complex AnalyticalHestonPricer :: func_Phi(const size_t& i, const double& tau, const double& x, const double& V, const double& omega) const
{

    //std:: cout << "In func_Phi entra\n\n";

    Complex exponent = func_C(i, tau, omega) + func_D(i, tau, omega) * Complex(V) + Complex(0, omega * x);

    return Complex :: exponential(exponent);
}

double AnalyticalHestonPricer :: func_integrand(const size_t& i, const double& tau, const double& x, const double& V, const double& omega) const
{
    //std :: cout << "\nIn Func_integrand entra\n";
    Complex exponent(0, -1 * omega * log(_strike));
    Complex integrand = (func_Phi(i, tau, x, V, omega) * Complex:: exponential(exponent)) / Complex(0, omega);

    //std:: cout << "Integrand_real_part: " << integrand.real_part() << "\n\n";
    return integrand.real_part();
}

double AnalyticalHestonPricer :: func_P(const size_t& i, const double& tau, const double& x, const double& V) const
{
    //std:: cout << "In Func_P entra\n\n";
    //GaussLegendreQuadrature glq(_gauss_legendre_sample_size);

    GaussLegendreQuadrature glq; //La classe Gauss Legendre per ora è gia inizializzata
    std::function<double(double)> func = [this, i, tau, x, V](double omega) {return func_integrand(i, tau, x, V, omega); };
    double integral = glq.integrate(func);

    //std:: cout <<  "Integral value:" << 0.5* (1. + integral / M_PI) << "\n\n";
    return 0.5* (1. + integral / M_PI);
}

double AnalyticalHestonPricer :: price() const
{
    //std:: cout << "\n\nEccolo il prezzo:\n\n";
    double S_0 = _model.initial_spot();
    double V_0 = _model.initial_variance();

    double first_addend = S_0 * func_P(1 , _maturity, log(S_0), V_0);
    double second_addend = _strike * exp(-1 * _model.drift() * _maturity) * func_P(2, _maturity, log(S_0), V_0);

    std:: cout << "Risultato: ";
    return first_addend - second_addend;
}