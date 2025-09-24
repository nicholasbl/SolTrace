#ifndef SOLTRACE_SIMULATION_RESULT_H
#define SOLTRACE_SIMULATION_RESULT_H

#include <cstdint>
#include <memory>
#include <vector>

#include "element.hpp"
#include "vector3d.hpp"

using ray_id = int_fast64_t;

struct InteractionRecord
{
    enum InteractionType
    {
        CREATE,
        ABSORB,
        REFLECT,
        TRANSMIT
    };

    int_fast64_t index;
    SolTrace::Data::element_id element;
    InteractionType type;
    SolTrace::Data::Vector3d location;

    InteractionRecord(SolTrace::Data::element_id el, InteractionType type,
                      const SolTrace::Data::Vector3d location);
    InteractionRecord(SolTrace::Data::element_id el, InteractionType type,
                      double x, double y, double z);
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
    SolTrace::Data::Vector3d cos_last;

    RayRecord(ray_id id);
    ~RayRecord();

    // Assumes that interactions are added in order
    void add_interaction_record(interaction_ptr ip);
    void set_last_cosines(const SolTrace::Data::Vector3d &clast);
    void set_last_cosines(double x, double y, double z);

    void get_direction_cosines(const interaction_ptr ip, SolTrace::Data::Vector3d &cos);
    void get_direction_cosines(int_fast64_t idx, SolTrace::Data::Vector3d &cos);
    uint_fast64_t get_number_of_interactions()
    {
        return this->interactions.size();
    }

    const interaction_ptr &operator[](int_fast64_t idx) const;
    // interaction_ptr &operator[](int_fast64_t idx);
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

class SimulationResult
{
public:
    SimulationResult();
    virtual ~SimulationResult();

    void add_ray_record(ray_record_ptr);
    uint_fast64_t get_number_of_records() const
    {
        return this->ray_history.size();
    }
    RayRecordContainer::const_iterator get_ray_record_iteratior()
    {
        return ray_history.cbegin();
    }
    bool is_at_end(RayRecordContainer::const_iterator citer)
    {
        return citer == this->ray_history.cend();
    }

    void write_csv_file(std::string csv_name);
    void write_csv_file(const char *csv_name);

    const ray_record_ptr &operator[](int_fast64_t idx) const;
    // ray_record_ptr &operator[](int_fast64_t idx);
    friend std::ostream &operator<<(std::ostream &os,
                                    const SimulationResult &simres);

private:
    RayRecordContainer ray_history;
};

#endif
