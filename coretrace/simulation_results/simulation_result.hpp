#ifndef SOLTRACE_SIMULATION_RESULT_H
#define SOLTRACE_SIMULATION_RESULT_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

// SimulationData headers
#include "element.hpp"
#include "vector3d.hpp"

// SimulationResult headers
#include "records.hpp"

namespace SolTrace::Result
{
    using RayRecordContainer = typename std::vector<ray_record_ptr>;
    using ElementRecordContainer = typename std::map<SolTrace::Data::element_id,
                                                     element_record_ptr>;

    class SimulationResult
    {
    public:
        SimulationResult();
        virtual ~SimulationResult();

        // Functions for getting and analyzing results
        uint_fast64_t get_number_of_records() const
        {
            return this->ray_history.size();
        }

        const element_record_ptr get_element_record(SolTrace::Data::element_id elid) const;

        RayRecordContainer::const_iterator get_ray_record_iterator() const
        {
            return ray_history.cbegin();
        }
        bool is_at_end(RayRecordContainer::const_iterator citer) const
        {
            return citer == this->ray_history.cend();
        }
        ElementRecordContainer::const_iterator get_element_record_iterator() const
        {
            return element_view.cbegin();
        }
        bool is_at_end(ElementRecordContainer::const_iterator citer) const
        {
            return citer == this->element_view.cend();
        }

        // Functions for building up results (used by Runners)
        void add_ray_record(ray_record_ptr);

        // Functions for file IO
        void write_csv_file(std::string csv_name, int precision = 12) const;
        void write_csv_file(const char *csv_name, int precision = 12) const;

        // Legacy stuff -- TODO:
        // void results_to_legacy_csv(std::string csv_name,
        //                            SimulationData *data);

        // Operator overloads
        const ray_record_ptr &operator[](int_fast64_t idx) const;
        friend std::ostream &operator<<(std::ostream &os,
                                        const SimulationResult &simres);

    private:
        RayRecordContainer ray_history;
        ElementRecordContainer element_view;

        void add_element_view(const ray_record_ptr rp);
    };

} // namespace SolTrace::Result

#endif
