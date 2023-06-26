#include "Complex.h"

//Constructors with parameters
Complex :: Complex(const double& real, const double& imaginary)
                : _real(real), _imaginary(imaginary)
                {
                }

//Getters 
double Complex :: real_part() const
                    {
                        return _real;
                    }

double Complex :: imaginary_part() const
                    {
                        return _imaginary;
                    }

//Other methods
bool Complex :: isZero() const
                    {
                        if(_real == 0 && _imaginary == 0)
                        {
                            return true;
                        }else{
                            return false;
                        }
                    }

double Complex :: module() const
                    {
                        return sqrt(pow(_real, 2) + pow(_imaginary, 2));
                    }

double Complex :: argument() const
                    {
                        if(isZero() == false)
                        {
                            return atan2(_imaginary, _real);
                        }else{
                            return 0;
                        }
                    }

//Other methods in public
//Operations between complex numbers

Complex Complex :: operator+(const Complex& complex) const
                    {
                        Complex sum(_real + complex.real_part(), _imaginary + complex.imaginary_part());

                        return sum;
                    }

Complex Complex :: operator-(const Complex& complex) const
                    {
                        Complex diff(_real - complex.real_part(), _imaginary - complex.imaginary_part());

                        return diff;
                    }

Complex Complex :: operator*(const Complex& complex) const
                    {
                        double real = _real * complex.real_part() - _imaginary * complex.imaginary_part();
                        double imaginary = _real * complex.imaginary_part() + _imaginary * complex.real_part();

                        Complex product(real, imaginary);

                        return product;
                    }

Complex Complex :: operator/(const Complex& complex) const
                    {
                        double real = _real * complex.real_part() + _imaginary * complex.imaginary_part();
                        double imaginary = _imaginary * complex.real_part() - _real * complex.imaginary_part();

                        if(complex.module() != 0)
                        {
                            Complex division(real / pow(complex.module(), 2), imaginary / pow(complex.module(), 2));

                            return division;
                        }else{
                            throw "Cannot divide by zero";
                        }
                    }

//Implementation of static methods exponential, logarithm, square_root
Complex Complex :: exponential(const Complex& complex)
{
    double real = exp(complex.real_part()) * cos(complex.imaginary_part());
    double img = exp(complex.real_part()) * sin(complex.imaginary_part());

    Complex expo(real, img);

    return expo;
}

Complex Complex :: logarithm(const Complex& complex)
{
    if(complex.module() == 0)
    {
        throw "Cannot calcule log(0)";
    }else{
        double real = log(complex.module());
        double img = complex.argument();

        Complex lg(real, img);

        return lg;
    }
}

Complex Complex :: square_root(const Complex& complex)
{
    double real = sqrt(complex.module()) * cos(complex.argument() / 2);
    double img = sqrt(complex.module()) * sin(complex.argument() / 2);

    Complex root(real, img);

    return root;
}