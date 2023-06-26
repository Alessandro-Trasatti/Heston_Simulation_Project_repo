#include<iostream>
#include<vector>
#include<functional>
#include<cmath>

using Vector = std::vector<double>;

class GaussLegendreQuadrature
{
public:

    /*Per il momento non facciamo l'implementazione generale, ma assumiamo
    di lavorare con un numero di nodi pari a 3. Conosciamo anche il 
    valore reale dei pesi (PDF Tesi Michele Martinelli). Quindi, per 
    ora non abbiamo bisogno del costruttore con parametro*/
    //GaussLegendreQuadrature(const size_t& number_sample_points);

    // Integration of f over the real interval ]- infinity, + infinity[
    double integrate(std :: function<double(double)> func) const;


private:

    size_t _number_sample_points = 3;

    // Those data members are constructed in the constructor
    Vector _weights = {0.5556, 0.8889, 0.5556}; // (w_i)
    Vector _points = {-0.7746, 0.0001 , 0.7746}; // (x_i)
};