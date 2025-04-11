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

    // element_id add_element(std::shared_ptr<Element> el);
    // void remove_element(element_id id);
    // std::shared_ptr<Element> get_element(element_id id);
    // void replace_element(element_id id, std::shared_ptr<Element> el);
    
    // uint64_t get_number_of_elements() const;
    // uint64_t get_total_number_of_elements() const;

    void set_number_of_rays(uint_fast64_t nrays);
    uint_fast64_t get_number_of_rays() const;

    void set_tolerance(double tolerance);
    double get_tolerance() const;

    void set_seed(int seed);
    int get_seed() const;

    void set_latitude(double latitude);
    double get_latitude() const;

    void set_longitude(double longitude);
    double get_longitude() const;

    // TODO: How to handle date/time?
    void set_simulation_date(const Date &);
    const Date & get_simulation_date() const;
    void set_simulation_datetime(const DateTime &);
    const DateTime & get_simulation_datetime() const;
    void set_simulation_time(const Time &);
    const Time & get_simulation_time() const;

    const SimulationParameters & get_simulation_parameters() const;

    int update_simulation_positions();
    int update_simulation_positions(const Time &);
    int update_simulation_positions(const Date &);
    int update_simulation_positions(const DateTime &);

    // int dump_to_csv(const char *csv_name) const;
    // int import_from_csv(const char *csv_name);
    // int import_legacy_table(const char *table_file_name);
    int import_from_file(const char *file_name);
    int import_from_file(const std::string file_name);

private:
    ElementContainer my_elements;
    RaySourceContainer my_sources;
    SimulationParameters my_parameters;

};

#endif
