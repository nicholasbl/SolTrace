#ifndef SOLTRACE_BINNER_H
#define SOLTRACE_BINNER_H

#include <cstdint>
#include <map>
#include <memory>

#include <element.hpp>
#include <vector3d.hpp>

#include "element_statistics.hpp"
#include "records.hpp"

namespace SolTrace::Result
{
    class Binner
    {
    public:
        Binner() {}
        ~Binner() {}

        // void generate_bins(SolTrace::Data::element_ptr el,
        //                    bin_data_ptr data) const;

        // bin_id find_bin_interval_uniform(double x,
        //                                  double a,
        //                                  double b,
        //                                  double dx) const;

        // void put_in_bins_uniform(SolTrace::Data::element_ptr el,
        //                          RayEvent event,
        //                          const element_record_ptr erec,
        //                          bin_data_ptr data) const;

        // void put_in_bins_nonuniform(SolTrace::Data::element_ptr el,
        //                             RayEvent event,
        //                             const element_record_ptr erec,
        //                             bin_data_ptr data) const;

        // void put_in_bins(SolTrace::Data::element_ptr el,
        //                  RayEvent event,
        //                  const element_record_ptr erec,
        //                  bin_data_ptr data) const;

        void bin_rays(SolTrace::Data::element_ptr el,
                      RayEvent event,
                      const element_record_ptr erec,
                      bin_data_ptr data) const;

        bin_data_ptr bin_rays(SolTrace::Data::element_ptr el,
                              RayEvent event,
                              const element_record_ptr erec,
                              uint_fast64_t nx,
                              uint_fast64_t ny) const;

        element_stats_ptr compute_element_stats(SolTrace::Data::element_ptr el,
                                                RayEvent event,
                                                const element_record_ptr erec,
                                                uint_fast64_t nx,
                                                uint_fast64_t ny) const;

        void compute_element_stats(SolTrace::Data::element_ptr el,
                                   RayEvent event,
                                   const element_record_ptr erec,
                                   element_stats_ptr estat) const;

    private:
    };

} // namespace SolTrace::Result

#endif
