#ifndef SOLTRACE_BIN_BACKEND_H
#define SOLTRACE_BIN_BACKEND_H

// #include <aperture.hpp>
// #include <surface.hpp>
#include <element.hpp>

#include "element_statistics.hpp"

namespace SolTrace::Result
{

    void bin_flat_rectangle(SolTrace::Data::element_ptr el,
                            RayEvent event,
                            const element_record_ptr erec,
                            bin_data_ptr data);

    void bin_cylinder_rectangle(SolTrace::Data::element_ptr el,
                                RayEvent event,
                                const element_record_ptr erec,
                                bin_data_ptr data);

} // namespace SolTrace::Result

#endif
