#ifndef SOLTRACE_TRACING_ERRORS_H
#define SOLTRACE_TRACING_ERRORS_H

#include "optical_properties.hpp"

#include "mtrand.hpp"
#include "native_runner_types.hpp"

void Errors(MTRand &myrng,
            double CosIn[3],
            int Source,
            TSun *Sun,
            // TElement *Element,
            // TOpticalProperties *OptProperties,
            const SolTrace::Data::OpticalProperties *OptProperties,
            double CosOut[3],
            double DFXYZ[3]);

void SurfaceNormalErrors(MTRand &myrng,
                         double CosIn[3],
                         const SolTrace::Data::OpticalProperties *OptProperties,
                         double CosOut[3]) noexcept(false); // throw(nanexcept);


#endif
