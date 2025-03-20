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
    this->set_simulation_time(t);
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const Date &d)
{
    this->set_simulation_date(d);
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const DateTime &dt)
{
    this->set_simulation_datetime(dt);
    return this->update_simulation_positions();
}