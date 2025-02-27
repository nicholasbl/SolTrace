#ifndef SOLTRACE_SIMULATION_DATA_H
#define SOLTRACE_SIMULATION_DATA_H

#include <map>

#include "datetime.hpp"
#include "element.hpp"
#include "ray_source.hpp"
#include "simulation_parameters.hpp"

class SimulationData
{
public:

    typedef uint64_t element_id;
    typedef std::map<element_id,Element*>::iterator iterator;
    typedef std::map<element_id,Element*>::const_iterator const_iterator;

    SimulationData();
    virtual ~SimulationData();

    void add_ray_source(RaySource *src);
    RaySource *get_ray_source() const;

    element_id add_element(Element *el);
    void remove_element(element_id id);
    Element *get_element(element_id id) const;
    void replace_element(element_id id, Element *el);

    // iterator get_iterator()
    // {
    //     return my_elements.begin();
    // }
    const_iterator get_iterator() const
    {
        return my_elements.begin();
    }
    // bool is_at_end(iterator iter) 
    // {
    //     return iter == my_elements.end();
    // }
    bool is_at_end(const_iterator iter) const
    {
        return iter == my_elements.end();
    }
    
    uint64_t get_number_of_elements() const;
    uint64_t get_total_number_of_elements() const;

    void set_number_of_rays(uint64_t nrays);
    uint64_t get_number_of_rays() const;

    void set_tolerance(double tolerance);
    double get_tolerance() const;

    void set_seed(int seed);
    int get_seed() const;

    void set_latitude(double latitude);
    double get_latitude() const;

    // TODO: Do we need these?
    void set_longitude(double longitude);
    double get_longitude() const;

    // TODO: How to handle date/time?
    void set_simulation_date(const Date*);
    const Date* get_simulation_date() const;
    void set_simulation_datetime(const DateTime*);
    const DateTime* get_simulation_datetime() const;
    void set_simulation_time(const Time*);
    const Time* get_simulation_time() const;

    int update_simulation_positions();
    int update_simulation_positions(const Time*);
    int update_simulation_positions(const Date*);
    int update_simulation_positions(const DateTime*);

    int dump_to_csv(const char *csv_name) const;
    int import_from_csv(const char *csv_name);
    int import_legacy_table(const char *table_file_name);

private:
    std::map<element_id, Element*> my_elements;
    RaySource *my_source;
    SimulationParameters my_parameters;

    element_id next_id;

};

#endif
