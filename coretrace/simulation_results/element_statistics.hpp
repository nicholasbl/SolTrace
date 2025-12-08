#ifndef SOLTRACE_ELEMENT_STATISTICS_H
#define SOLTRACE_ELEMENT_STATISTICS_H

#include <memory>

#include "records.hpp"

namespace SolTrace::Result
{
    using bin_id = uint_fast64_t;

    struct BinData
    {
        BinData(uint_fast64_t nx, uint_fast64_t ny);
        ~BinData();
        void clear_counts();
        uint_fast64_t count_total();

        bool bins_initialzed;

        uint_fast64_t nx;
        uint_fast64_t ny;

        SolTrace::Data::Vector3d lower_bounds;
        SolTrace::Data::Vector3d upper_bounds;

        bool uniform_bin_size;
        SolTrace::Data::Vector3d uniform_size;

        std::map<bin_id, uint_fast64_t> counts;
        std::map<bin_id, SolTrace::Data::Vector3d> midpoints;
        std::map<bin_id, SolTrace::Data::Vector3d> sizes;
    };

    using bin_data_ptr = std::shared_ptr<BinData>;
    template <typename... Args>
    inline auto make_bin_data(Args &&...args)
    {
        return std::make_shared<BinData>(std::forward<Args>(args)...);
    }

    void get_bin_id(const BinData &data,
                    const bin_id &xid,
                    const bin_id &yid,
                    bin_id &id);

    void get_bin_xyid(const BinData &data,
                      const bin_id &id,
                      bin_id &xid,
                      bin_id &yid);

    void get_bin_id(const bin_data_ptr data,
                    const bin_id &xid,
                    const bin_id &yid,
                    bin_id &id);

    void get_bin_xyid(const bin_data_ptr data,
                      const bin_id &id,
                      bin_id &xid,
                      bin_id &yid);

    struct ElementStatistics
    {
        ElementStatistics(SolTrace::Data::element_id id,
                          RayEvent rev,
                          bin_data_ptr bd);
        ~ElementStatistics();
        SolTrace::Data::element_id elid;
        RayEvent event;
        bin_data_ptr data;

        // TODO: Various functions for various statistics...
    };

    using element_stats_ptr = std::shared_ptr<ElementStatistics>;
    template <typename... Args>
    inline auto make_element_stats(Args &&...args)
    {
        return std::make_shared<ElementStatistics>(std::forward<Args>(args)...);
    }

} // namespace SolTrace::Result

#endif
