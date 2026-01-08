#ifndef SOLTRACE_EMBREE_TYPES_H
#define SOLTRACE_EMBREE_TYPES_H

#include <memory>

#include <aperture.hpp>
#include <surface.hpp>

#include <native_runner_types.hpp>

namespace SolTrace::EmbreeRunner
{
    struct MElement
    {
        SolTrace::NativeRunner::tstage_ptr tstage;
        SolTrace::NativeRunner::telement_ptr telem;
        SolTrace::Data::aperture_ptr ap;
        SolTrace::Data::surface_ptr surf;
    };

    using melement_ptr = std::shared_ptr<MElement>;

    melement_ptr make_melement(
        SolTrace::Data::element_ptr el,
        int_fast64_t el_num,
        const SolTrace::NativeRunner::ElementParameters &eparams);

} // namespace SolTrace::EmbreeRunner

#endif
