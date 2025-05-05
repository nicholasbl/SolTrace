#include "simulation_data.hpp"

SimulationData::SimulationData()
{
    return;
}

SimulationData::~SimulationData()
{
    return;
}

int SimulationData::update_simulation_positions()
{
    int sts = 0;
    for (auto iter = this->my_elements.get_iterator();
         !this->my_elements.is_at_end(iter);
         ++iter)
    {
        ;
    }
    return sts;
}

int SimulationData::update_simulation_positions(const Time &t)
{
    this->my_parameters.sim_dt.my_time = t;
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const Date &d)
{
    this->my_parameters.sim_dt.my_date = d;
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const DateTime &dt)
{
    this->my_parameters.sim_dt.set(dt);
    return this->update_simulation_positions();
}

int SimulationData::import_from_file(const char *file_name)
{
    int sts = 0;
    // TODO: Implement this
    return sts;
}

int SimulationData::import_from_file(const std::string file_name)
{
    return this->import_from_file(file_name.c_str());
}
