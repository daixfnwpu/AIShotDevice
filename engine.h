#ifndef ENGINE_H
#define ENGINE_H
std::pair<double, double> initdistanceandtrajectory(double r, double v0, double theta0, double r0,double shotdistance);
double run(std::pair<double, double>  tpos,double theta0,double distancehandtoeye, double eyetoaxis, double shotdistance, double shotdoorwidth=0.04, double powerscala = 1 ) ;
#endif