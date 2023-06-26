#include "GaussLegendreQuadrature.h"

double GaussLegendreQuadrature :: integrate(std :: function<double(double)> func) const
                                    {
                                        double intg=0;
                                        for(size_t idx=0; idx < _weights.size(); idx ++)
                                        {
                                            intg += _weights[idx] * func(_points[idx] / (1 - pow(_points[idx], 2))) *
                                                    (1 + pow(_points[idx], 2)) / pow((1 - _points[idx] * _points[idx]), 2);
                                        }

                                        return intg;
                                    }