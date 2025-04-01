#include "simulation_data.hpp"

SimulationData::SimulationData()
{
    return;
}

SimulationData::~SimulationData()
{
    return;
}

void SimulationData::set_simulation_date(const Date &d)
{
    return;
}

const Date& SimulationData::get_simulation_date() const
{
    return this->my_parameters.get_simulation_date();
}

void SimulationData::set_simulation_datetime(const DateTime &dt)
{
    return;
}

const DateTime& SimulationData::get_simulation_datetime() const
{
    return this->my_parameters.get_simulation_datetime();
}

void SimulationData::set_simulation_time(const Time &t)
{
    return;
}

const Time& SimulationData::get_simulation_time() const
{
    return this->my_parameters.get_simulation_time();
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