#ifndef SOLTRACE_ARCLENGTH_H
#define SOLTRACE_ARCLENGTH_H

double parabolic_arc_length(double cx,
                            double x);
double parabolic_arc_length(double cx,
                            double x0,
                            double x1,
                            double dx = 1e-6);
double parabolic_determine_x_coordinate(double cx,
                                        double x0,
                                        double arc_length,
                                        double dx = 1e-6);

#endif
