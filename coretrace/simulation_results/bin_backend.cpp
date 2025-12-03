#include "bin_backend.hpp"

#include <memory>

#include <aperture.hpp>
#include <surface.hpp>
#include <vector3d.hpp>

using SolTrace::Data::aperture_ptr;
using SolTrace::Data::element_ptr;
using SolTrace::Data::surface_ptr;
using SolTrace::Data::Vector3d;

namespace SolTrace::Result
{

    bin_id find_bin_interval_uniform(double x,
                                     double a,
                                     double b,
                                     double dx)
    {
        return static_cast<uint_fast64_t>(floor((x - a) / dx));
    }

    void bin_flat_rectangle(element_ptr el,
                            RayEvent event,
                            const element_record_ptr erec,
                            bin_data_ptr data)
    {
        if (!data->bins_initialzed)
        {
            aperture_ptr ap = el->get_aperture();
            auto rect = std::dynamic_pointer_cast<SolTrace::Data::Rectangle>(ap);
            if (rect == nullptr)
            {
                throw std::invalid_argument("bin_flat_rectangle: Aperture must be rectangle");
            }

            surface_ptr surf = el->get_surface();
            auto flat = std::dynamic_pointer_cast<SolTrace::Data::Flat>(surf);
            if (flat == nullptr)
            {
                throw std::invalid_argument("bin_flat_rectangle: Surface must be flat");
            }

            data->lower_bounds[0] = rect->x_coord;
            data->lower_bounds[1] = rect->y_coord;
            data->upper_bounds[0] = rect->x_coord + rect->x_length;
            data->upper_bounds[1] = rect->y_coord + rect->y_length;

            data->uniform_bin_size = true;
            data->uniform_size[0] = rect->x_length / data->nx;
            data->uniform_size[1] = rect->y_length / data->ny;

            data->bins_initialzed = true;
        }

        interaction_ptr ip;
        bin_id xid, yid, id;
        Vector3d pos_local;

        for (auto iter = erec->interactions.cbegin();
             iter != erec->interactions.cend();
             ++iter)
        {
            ip = *iter;
            if (ip->event == event)
            {
                el->convert_global_to_local(pos_local, ip->location);
                xid = find_bin_interval_uniform(pos_local[0],
                                                data->lower_bounds[0],
                                                data->upper_bounds[0],
                                                data->uniform_size[0]);

                yid = find_bin_interval_uniform(pos_local[1],
                                                data->lower_bounds[1],
                                                data->upper_bounds[1],
                                                data->uniform_size[1]);

                get_bin_id(data, xid, yid, id);
                data->counts[id] += 1;
            }
        }

        return;
    }

    void bin_cylinder_rectangle(SolTrace::Data::element_ptr el,
                                RayEvent event,
                                const element_record_ptr erec,
                                bin_data_ptr data)
    {
        return;
    }

} // namespace SolTrace::Result
