#include "binner.hpp"

#include <cmath>
#include <sstream>

#include <simulation_data_export.hpp>

#include "bin_backend.hpp"

namespace SolTrace::Result
{

    void Binner::bin_rays(element_ptr el,
                          RayEvent event,
                          const element_record_ptr erec,
                          bin_data_ptr data) const
    {
        ApertureType apt = el->get_aperture()->get_type();
        SurfaceType sft = el->get_surface()->get_type();

        if (apt == ApertureType::RECTANGLE && sft == SurfaceType::FLAT)
        {
            bin_flat_rectangle(el, event, erec, data);
        }
        else if (apt == ApertureType::RECTANGLE && sft == SurfaceType::CYLINDER)
        {
            bin_cylinder_rectangle(el, event, erec, data);
        }
        else
        {
            std::stringstream ss;
            ss << "Binning request for unimplemented aperture-surface combination: "
               << "Aperture = " << SolTrace::Data::ApertureTypeMap.at(apt)
               << ", Surface = " << SolTrace::Data::SurfaceTypeMap.at(sft);
            throw std::invalid_argument(ss.str());
        }

        return;
    }

    bin_data_ptr Binner::bin_rays(element_ptr el,
                                  RayEvent event,
                                  const element_record_ptr erec,
                                  uint_fast64_t nx,
                                  uint_fast64_t ny) const
    {
        bin_data_ptr data = make_bin_data(nx, ny);
        this->bin_rays(el, event, erec, data);
        return data;
    }

    element_stats_ptr Binner::compute_element_stats(element_ptr el,
                                                    RayEvent event,
                                                    const element_record_ptr erec,
                                                    uint_fast64_t nx,
                                                    uint_fast64_t ny) const
    {
        bin_data_ptr data = this->bin_rays(el, event, erec, nx, ny);
        element_stats_ptr est = make_element_stats(el->get_id(), event, data);
        return est;
    }

    void Binner::compute_element_stats(SolTrace::Data::element_ptr el,
                                       RayEvent event,
                                       const element_record_ptr erec,
                                       element_stats_ptr estat) const
    {
        if (el->get_id() != estat->elid)
        {
            estat->data->bins_initialzed = false;
        }
        this->bin_rays(el, event, erec, estat->data);
        return;
    }

} // namespace SolTrace::Result
