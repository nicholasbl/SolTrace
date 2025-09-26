#include "simulation_result.hpp"

#include <iostream>
#include <string>

#include "element.hpp"
#include "vector3d.hpp"

namespace SolTrace::Result {

using element_id = SolTrace::Data::element_id;
using Vector3d = SolTrace::Data::Vector3d;

const std::string& ray_event_string(const RayEvent rev)
{
    auto item = REV_TO_STR.find(rev);
    if (item != REV_TO_STR.cend())
    {
        return item->second;
    }
    else
    {
        return REV_TO_STR.find(RayEvent::UNKNOWN)->second;
    }
}

InteractionRecord::InteractionRecord(
    element_id el,
    RayEvent rev,
    const Vector3d location)
    : index(-1),
      element(el),
      event(rev),
      location(location)
{
}

InteractionRecord::InteractionRecord(
    element_id el,
    RayEvent rev,
    double x,
    double y,
    double z)
    : index(-1),
      element(el),
      event(rev),
      location(x, y, z)
{
}

InteractionRecord::~InteractionRecord()
{
}

std::ostream &operator<<(std::ostream &os, const InteractionRecord &rec)
{
    // TODO: Implement this
    os << "Element ID: " << rec.element
       << " Location: " << rec.location
       << " Event: " << ray_event_string(rec.event);
    return os;
}

RayRecord::RayRecord(ray_id id) : id(id)
{
}

RayRecord::~RayRecord()
{
    this->interactions.clear();
    return;
}

void RayRecord::add_interaction_record(interaction_ptr ip)
{
    ip->index = this->interactions.size();
    this->interactions.push_back(ip);
    return;
}

void RayRecord::set_last_cosines(const Vector3d &clast)
{
    this->cos_last = clast;
    return;
}

void RayRecord::set_last_cosines(double x, double y, double z)
{
    this->cos_last.set_values(x, y, z);
    return;
}

void RayRecord::get_direction_cosines(const interaction_ptr ip,
                                      Vector3d &cos)
{
    // this->get_direction_cosines(ip->index, cos);
    if (ip->index + 1 < this->interactions.size())
    {
        const interaction_ptr ip1 = this->interactions[ip->index + 1];
        vector_add(1.0, ip1->location, -1.0, ip->location, cos);
    }
    else
    {
        cos = this->cos_last;
    }
    make_unit_vector(cos);
    return;
}

void RayRecord::get_direction_cosines(int_fast64_t idx,
                                      Vector3d &cos)
{
    const interaction_ptr ip0 = this->interactions[idx];
    this->get_direction_cosines(ip0, cos);
    return;
}

const interaction_ptr &RayRecord::operator[](int_fast64_t idx) const
{
    // TODO: Probably should do bounds checking...
    return this->interactions[idx];
}

// interaction_ptr &RayRecord::operator[](int_fast64_t idx)
// {
//     // TODO: Probably should do bounds checking...
//     return this->interactions[idx];
// }

std::ostream &operator<<(std::ostream &os, const RayRecord &rec)
{
    // TODO: Implement this
    os << "Ray ID: " << rec.id
       << "\nInteractions:\n";
    for (uint_fast64_t k = 0; k < rec.interactions.size(); ++k)
    {
        os << k << " " << rec.interactions[k] << "\n";
    }
    os << "Last Cosines: " << rec.cos_last << "\n";
    return os;
}

SimulationResult::SimulationResult()
{
    return;
}

SimulationResult::~SimulationResult()
{
    this->ray_history.clear();
    return;
}

void SimulationResult::add_ray_record(ray_record_ptr rp)
{
    this->ray_history.push_back(rp);
    return;
}

void SimulationResult::write_csv_file(std::string csv_name)
{
    return this->write_csv_file(csv_name.c_str());
}

void SimulationResult::write_csv_file(const char *csv_name)
{
    // TODO: Implement this
    return;
}

const ray_record_ptr &SimulationResult::operator[](int_fast64_t idx) const
{
    // TODO: Probably should do bounds checking...
    return this->ray_history[idx];
}

// ray_record_ptr &SimulationResult::operator[](int_fast64_t idx)
// {
//     // TODO: Probably should do bounds checking...
//     return this->ray_history[idx];
// }

std::ostream &operator<<(std::ostream &os, const SimulationResult &simres)
{
    // TODO: Implement this
    os << "Simulation Results -- " << simres.ray_history.size() << " Rays\n";
    for (uint_fast64_t k = 0; k < simres.ray_history.size(); ++k)
    {
        os << "Ray: " << k << "\n"
           << simres.ray_history[k];
    }
    return os;
}

} // namespace SolTrace::Result
