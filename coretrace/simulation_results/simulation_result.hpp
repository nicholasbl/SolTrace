#ifndef SOLTRACE_SIMULATION_RESULT_H
#define SOLTRACE_SIMULATION_RESULT_H

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <glm/vec3.hpp>

#include "element.hpp"

// SimulationResult headers
#include "records.hpp"

namespace SolTrace::Result
{
    using ray_id = int_fast64_t;

    enum class RayEvent
    {
        CREATE = 1,
        ABSORB = 2,
        REFLECT = 3,
        TRANSMIT = 4,
        VIRTUAL = 5,
        EXIT = 6,
        UNKNOWN = 1000
    };

    const std::map<RayEvent, std::string> REV_TO_STR{
        {RayEvent::CREATE, "CREATE"},
        {RayEvent::ABSORB, "ABSORB"},
        {RayEvent::REFLECT, "REFLECT"},
        {RayEvent::TRANSMIT, "TRANSMIT"},
        {RayEvent::VIRTUAL, "VIRTUAL"},
        {RayEvent::EXIT, "EXIT"},
        {RayEvent::UNKNOWN, "UNKNOWN"}};

    const std::string &ray_event_string(RayEvent rev);

    struct InteractionRecord
    {
        // int_fast64_t index;
        SolTrace::Data::element_id element;
        RayEvent event;
        glm::dvec3 location;
        glm::dvec3 direction;

        InteractionRecord(SolTrace::Data::element_id el, RayEvent rev,
                          const glm::dvec3 &location,
                          const glm::dvec3 &direction);
        InteractionRecord(SolTrace::Data::element_id el, RayEvent rev,
                          double px, double py, double pz,
                          double dx, double dy, double dz);
        ~InteractionRecord();

        friend std::ostream &operator<<(std::ostream &os,
                                        const InteractionRecord &rec);
    };

    using interaction_ptr = std::shared_ptr<InteractionRecord>;
    template <typename... Args>
    inline auto make_interaction_record(Args &&...args)
    {
        return std::make_shared<InteractionRecord>(std::forward<Args>(args)...);
    }

    struct RayRecord
    {
        ray_id id;
        std::vector<interaction_ptr> interactions;

        RayRecord(ray_id id);
        ~RayRecord();

        // Assumes that interactions are added in order
        void add_interaction_record(interaction_ptr ip);

        SolTrace::Data::element_id get_element(const interaction_ptr ip)
        {
            return ip->element;
        }
        SolTrace::Data::element_id get_element(int_fast64_t idx)
        {
            return get_element(this->interactions[idx]);
        }
        RayEvent get_event(const interaction_ptr ip)
        {
            return ip->event;
        }
        RayEvent get_event(int_fast64_t idx)
        {
            return get_event(this->interactions[idx]);
        }

        void get_position(const interaction_ptr ip, glm::dvec3 &pos);
        void get_position(int_fast64_t idx, glm::dvec3 &pos);
        void get_direction(const interaction_ptr ip, glm::dvec3 &cos);
        void get_direction(int_fast64_t idx, glm::dvec3 &cos);

        uint_fast64_t get_number_of_interactions()
        {
            return this->interactions.size();
        }

        const interaction_ptr &operator[](int_fast64_t idx) const;
        friend std::ostream &operator<<(std::ostream &os,
                                        const RayRecord &rec);
    };

    using ray_record_ptr = std::shared_ptr<RayRecord>;
    template <typename... Args>
    inline auto make_ray_record(Args &&...args)
    {
        return std::make_shared<RayRecord>(std::forward<Args>(args)...);
    }

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

        // Sun results
        void set_sun_ray_count(uint_fast64_t ray_count) { this->sun_ray_count = ray_count; }
        uint_fast64_t get_sun_ray_count() { return this->sun_ray_count; }
        void set_sun_dimensions(double width, double height) { this->sun_width = width; this->sun_height = height; }
        void get_sun_dimensions(double& width, double& height) { width = this->sun_width; height = this->sun_height; }
        void set_sun_A_box(double A) { this->A_sun_box = A; }
        double get_sun_A_box() { return this->A_sun_box; }

    private:
        RayRecordContainer ray_history;
        ElementRecordContainer element_view;

        void add_element_view(const ray_record_ptr rp);

        // Sun results
        uint_fast64_t sun_ray_count = 0;
        double sun_width = 0;
        double sun_height = 0;
        double A_sun_box = 0;
        
    };

} // namespace SolTrace::Result

#endif
