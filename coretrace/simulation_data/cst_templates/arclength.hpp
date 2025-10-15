#ifndef SOLTRACE_ARCLENGTH_H
#define SOLTRACE_ARCLENGTH_H

/**
 * @file arclength.hpp
 * @brief Arc length calculation utilities
 *
 * Provides functions for calculating arc lengths along curved surfaces,
 * primarily used in CST template geometry generation and surface parameterization.
 * Includes functions for parabolic arc length calculations and coordinate determination.
 */

namespace SolTrace::Data {

/**
 * @brief Calculate arc length along parabola from origin to x
 * @param cx Parabola coefficient (focal parameter)
 * @param x End x coordinate
 * @return Arc length from origin to point (x, cx*x^2)
 */
double parabolic_arc_length(double cx,
                            double x);

/**
 * @brief Calculate arc length along parabola between two x coordinates
 * @param cx Parabola coefficient (focal parameter)
 * @param x0 Starting x coordinate
 * @param x1 Ending x coordinate
 * @param dx Integration step size for numerical calculation
 * @return Arc length between points (x0, cx*x0^2) and (x1, cx*x1^2)
 */
double parabolic_arc_length(double cx,
                            double x0,
                            double x1,
                            double dx = 1e-6);

/**
 * @brief Determine x coordinate given arc length along parabola
 * @param cx Parabola coefficient (focal parameter)
 * @param x0 Starting x coordinate
 * @param arc_length Desired arc length from starting point
 * @param dx Step size for numerical root finding
 * @return X coordinate at specified arc length from starting point
 */
double parabolic_determine_x_coordinate(double cx,
                                        double x0,
                                        double arc_length,
                                        double dx = 1e-6);

} // namespace SolTrace::Data

#endif
