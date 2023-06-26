#pragma once
#include<iostream>
#include<cmath>


class Complex
{
public:
    //Constructor with parameters 
    Complex(const double& real = 0, const double& imaginary = 0);

    //Getters
    double real_part() const;
    double imaginary_part() const;

    //Methods module and arguments of a complex number
    double module() const;
    double argument() const;

    //Methods for operations between complex numbers
    Complex operator+(const Complex& complex) const;
    Complex operator-(const Complex& complex) const;
    Complex operator*(const Complex& complex) const;
    Complex operator/(const Complex& complex) const;

    //Static Methods: exp,log,square root
    static Complex exponential(const Complex& complex);
    static Complex logarithm(const Complex& complex);
    static Complex square_root(const Complex& complex);



private:

    //method which return true if the complex number is nul, false otherwise
    bool isZero() const;

    double _real;
    double _imaginary;
};
