
#include "native_runner.hpp"
#include "simulation_parameters.hpp"
#include "simulation_data.hpp"
#include "trace.hpp"

NativeRunner::NativeRunner() : SimulationRunner(),
                               simdata(nullptr)
{
}

NativeRunner::~NativeRunner()
{
    if (this->simdata != nullptr)
    {
        this->simdata = nullptr;
    }
}

RunnerStatus NativeRunner::initialize()
{
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_simulation(const SimulationData *data)
{

    RunnerStatus sts;

    this->simdata = data;

    this->setup_parameters(data);
    this->setup_sun(data);
    sts = this->setup_elements(data);

    return sts;
}

RunnerStatus NativeRunner::setup_parameters(const SimulationData *data)
{
    // Get Parameter data
    // TODO: Check that these parameters are used as expected
    const SimulationParameters &sim_params = data->get_simulation_parameters();
    this->tsys.sim_errors_sunshape = sim_params.include_sun_shape_errors;
    this->tsys.sim_errors_optical = sim_params.include_optical_errors;
    this->tsys.sim_raycount = sim_params.number_of_rays;
    this->tsys.sim_raymax = sim_params.max_number_of_rays;
    this->tsys.seed = sim_params.seed;
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_sun(const SimulationData *data)
{
    // Get RaySource data (this runner assumes there is only the Sun)
    assert(data->get_number_of_ray_sources() == 1);
    this->tsys.Sun.set_values(data->get_ray_source());
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_elements(const SimulationData *data)
{
    auto sys = this->tsys;
    for (auto iter = data->get_const_iterator();
         !data->is_at_end(iter);
         ++iter)
    {
        element_ptr el = iter->second;
        if (el->is_enabled() && el->is_stage())
        {
            auto stage = make_tstage(el);
            sys.StageList.push_back(stage);
        }
    }
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::update_simulation(const SimulationData *data)
{
    this->setup_simulation(data);
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::run_simulation()
{
    bool trace_return = trace_native(
        &this->tsys,
        this->tsys.seed,
        this->tsys.sim_raycount,
        this->tsys.sim_raymax,
        this->tsys.sim_errors_sunshape,
        this->tsys.sim_errors_optical,
        this->as_power_tower
    );
    return trace_return ? RunnerStatus::SUCCESS : RunnerStatus::ERROR;
}

RunnerStatus NativeRunner::report_simulation(SimulationResult *result,
                                             int level)
{
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}
