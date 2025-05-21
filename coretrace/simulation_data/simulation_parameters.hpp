#ifndef SOLTRACE_SIMULATION_PARAMETERS_H
#define SOLTRACE_SIMULATION_PARAMETERS_H

#include <cstdint>

#include "datetime.hpp"

struct SimulationParameters
{
public:
    // TODO: Figure out how to store time...
    DateTime sim_dt;

    bool include_sun_shape_errors;
    bool include_optical_errors;

    std::uint_fast64_t number_of_rays;
    std::uint_fast64_t max_number_of_rays;
    double tolerance;

    double latitude;
    double longitude;

    int seed;

    SimulationParameters() : number_of_rays(10000),
                             max_number_of_rays(1000000),
                             tolerance(0.0),
                             latitude(0.0),
                             longitude(0.0),
                             seed(0)
    {
    }
    ~SimulationParameters() {}
};


// class SimulationParameters
// {

// public:
//     SimulationParameters() : number_of_rays(10000),
//                              tolerance(0.0),
//                              latitude(0.0),
//                              longitude(0.0),
//                              seed(0)
//     {
//     }
//     ~SimulationParameters() {}

//     const Date &get_simulation_date() const
//     {
//         return sim_dt.get_date();
//     }

//     void set_simulation_date(const Date &d)
//     {
//         this->sim_dt.set_date(d);
//         return;
//     }

//     const Time &get_simulation_time() const
//     {
//         return sim_dt.get_time();
//     }

//     void set_simulation_time(const Time &t)
//     {
//         this->sim_dt.set_time(t);
//         return;
//     }

//     const DateTime &get_simulation_datetime() const
//     {
//         return sim_dt;
//     }

//     void set_simulation_datetime(const DateTime &dt)
//     {
//         this->sim_dt.set_datetime(dt);
//         return;
//     }

//     std::uint_fast64_t get_number_of_rays() const
//     {
//         return this->number_of_rays;
//     }
//     void set_number_of_rays(std::uint_fast64_t num_rays)
//     {
//         this->number_of_rays = num_rays;
//         return;
//     }

//     double get_tolerance() const { return this->tolerance; }
//     void set_tolerance(double tol)
//     {
//         this->tolerance = tol;
//         return;
//     }
//     double get_latitude() const { return this->latitude; }
//     void set_latitude(double lat)
//     {
//         this->latitude = lat;
//         return;
//     }
//     double get_longitude() const { return this->longitude; }
//     void set_longitude(double lon)
//     {
//         this->longitude = lon;
//         return;
//     }

//     int get_seed() const { return this->seed; }
//     void set_seed(int s)
//     {
//         this->seed = s;
//         return;
//     }

// private:
//     // TODO: Figure out how to store time...
//     DateTime sim_dt;

//     std::uint_fast64_t number_of_rays;
//     double tolerance;

//     double latitude;
//     double longitude;

//     int seed;
// };

#endif
