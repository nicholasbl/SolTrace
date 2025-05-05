#ifndef SOLTRACE_SIMULATION_DATA_H
#define SOLTRACE_SIMULATION_DATA_H

#include <cstdint>
#include <map>
#include <memory>

#include "container.hpp"
#include "datetime.hpp"
#include "element.hpp"
#include "ray_source.hpp"
#include "simulation_parameters.hpp"

// using element_ptr=std::shared_ptr<Element>;
// using ElementContainer=std::map<element_id, element_ptr>;

// using ray_source_ptr=std::shared_ptr<RaySource>;
// using RaySourceContainer=std::map<ray_source_id, ray_source_ptr>;

class SimulationData
{
public:
    SimulationData();
    virtual ~SimulationData();

    ray_source_id add_ray_source(ray_source_ptr src)
    {
        return this->my_sources.add_item(src);
    }
    auto remove_ray_source(ray_source_id id)
    {
        return this->my_sources.remove_item(id);
    }
    ray_source_ptr get_ray_source(ray_source_id id)
    {
        return this->my_sources.get_item(id);
    }
    bool replace_ray_source(ray_source_id id, ray_source_ptr src)
    {
        return this->my_sources.replace_item(id, src);
    }

    element_id add_element(element_ptr el)
    {
        return this->my_elements.add_item(el);
    }
    auto remove_element(element_id id)
    {
        return this->my_elements.remove_item(id);
    }
    element_ptr get_element(element_id id)
    {
        return this->my_elements.get_item(id);
    }
    bool replace_element(element_id id, element_ptr el)
    {
        return this->my_elements.replace_item(id, el);
    }

    uint64_t get_number_of_elements() const
    {
        return this->my_elements.get_number_of_items();
    }
    uint64_t get_total_number_of_elements() const
    {
        return this->my_elements.get_total_number_of_items();
    }

    ElementContainer::iterator get_iterator()
    {
        return this->my_elements.get_iterator();
    }
    ElementContainer::const_iterator get_const_iterator()
    {
        return this->my_elements.get_const_iterator();
    }
    bool is_at_end(ElementContainer::iterator iter)
    {
        return this->my_elements.is_at_end(iter);
    }
    bool is_at_end(ElementContainer::const_iterator citer)
    {
        return this->my_elements.is_at_end(citer);
    }

    void set_number_of_rays(uint_fast64_t nrays)
    {
        this->my_parameters.number_of_rays = nrays;
        return;
    }
    uint_fast64_t get_number_of_rays() const
    {
        return this->my_parameters.number_of_rays;
    }

    void set_tolerance(double tolerance)
    {
        this->my_parameters.tolerance = tolerance;
        return;
    }
    double get_tolerance() const
    {
        return this->my_parameters.tolerance;
    }

    void set_seed(int seed)
    {
        this->my_parameters.seed = seed;
        return;
    }
    int get_seed() const
    {
        return this->my_parameters.seed;
    }

    void set_latitude(double latitude)
    {
        this->my_parameters.latitude = latitude;
        return;
    }
    double get_latitude() const
    {
        return this->my_parameters.latitude;
    }

    void set_longitude(double longitude)
    {
        this->my_parameters.longitude = longitude;
        return;
    }
    double get_longitude() const
    {
        return this->my_parameters.longitude;
    }

    void set_simulation_date(const Date &d)
    {
        this->my_parameters.sim_dt.my_date = d;
    }
    const Date &get_simulation_date() const
    {
        return this->my_parameters.sim_dt.my_date;
    }

    void set_simulation_datetime(const DateTime &dt)
    {
        this->my_parameters.sim_dt = dt;
        return;
    }
    const DateTime &get_simulation_datetime() const
    {
        return this->my_parameters.sim_dt;
    }

    void set_simulation_time(const Time &t)
    {
        this->my_parameters.sim_dt.my_time = t;
        return;
    }
    const Time &get_simulation_time() const
    {
        return this->my_parameters.sim_dt.my_time;
    }

    SimulationParameters &get_simulation_parameters()
    {
        return this->my_parameters;
    }
    const SimulationParameters &get_simulation_parameters() const
    {
        return this->my_parameters;
    }

    int update_simulation_positions();
    int update_simulation_positions(const Time &);
    int update_simulation_positions(const Date &);
    int update_simulation_positions(const DateTime &);

    int import_from_file(const char *file_name);
    int import_from_file(const std::string file_name);

private:
    ElementContainer my_elements;
    RaySourceContainer my_sources;
    SimulationParameters my_parameters;
};

#endif
