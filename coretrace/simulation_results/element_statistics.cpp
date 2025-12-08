#include "element_statistics.hpp"

#include <cstdint>

#include <simulation_data_export.hpp>

#include "records.hpp"

namespace SolTrace::Result
{
    void get_bin_id(const BinData &data,
                    const bin_id &xid,
                    const bin_id &yid,
                    bin_id &id)
    {
        // Row major ordering starting from the top, left-most bin
        // 0-based
        id = data.nx * xid + yid;
        return;
    }

    void get_bin_xyid(const BinData &data,
                      const bin_id &id,
                      bin_id &xid,
                      bin_id &yid)
    {
        yid = id % data.nx;
        // Intentional integer division...
        xid = id / data.nx;
        return;
    }

    void get_bin_id(const bin_data_ptr data,
                    const bin_id &xid,
                    const bin_id &yid,
                    bin_id &id)
    {
        return get_bin_id(*data, xid, yid, id);
    }

    void get_bin_xyid(const bin_data_ptr data,
                      const bin_id &id,
                      bin_id &xid,
                      bin_id &yid)
    {
        return get_bin_xyid(*data, id, xid, yid);
    }

    BinData::BinData(uint_fast64_t nx, uint_fast64_t ny)
        : bins_initialzed(false),
          nx(nx),
          ny(ny),
          uniform_bin_size(false)
    {
        // Initializes counts map
        this->clear_counts();
    }

    BinData::~BinData()
    {
        counts.clear();
        midpoints.clear();
        sizes.clear();
    }

    void BinData::clear_counts()
    {
        bin_id id;
        for (bin_id xid = 0; xid < this->nx; ++xid)
        {
            for (bin_id yid = 0; yid < this->ny; ++yid)
            {
                get_bin_id(*this, xid, yid, id);
                this->counts[id] = 0;
            }
        }
    }

    uint_fast64_t BinData::count_total()
    {
        uint_fast64_t total = 0;
        for (auto iter = this->counts.cbegin();
             iter != this->counts.cend();
             ++iter)
        {
            total += iter->second;
        }
        return total;
    }

    ElementStatistics::ElementStatistics(element_id id,
                                         RayEvent rev,
                                         bin_data_ptr bd)
        : elid(id),
          event(rev),
          data(bd)
    {
        return;
    }

    ElementStatistics::~ElementStatistics()
    {
        this->data = nullptr;
        return;
    }

} // namespace SolTrace::Result
