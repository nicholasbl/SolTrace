/**
 * @file error_distributions.hpp
 * @brief Error distribution types for optical modeling
 *
 * Defines various error distribution types (Gaussian, Pillbox, User-defined)
 * used for modeling optical errors in ray tracing simulations. These distributions
 * are applied to surface slopes, specularity errors, and other optical imperfections.
 */

#ifndef SOLTRACE_ERROR_DISTRIBUTIONS_H
#define SOLTRACE_ERROR_DISTRIBUTIONS_H

enum DistributionType
{
    GAUSSIAN,
    PILLBOX,
    USER_DEFINED
};

#endif
