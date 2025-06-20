
#include "simulation_runner/optix_runner/optix_runner.hpp"
#include "simulation_data/simulation_parameters.hpp"
#include "simulation_data/simulation_data.hpp"


OptixRunner::OptixRunner() : SimulationRunner(),
                             m_simdata(nullptr),    
                             sys(10000) {}


OptixRunner::~OptixRunner()
{

}

RunnerStatus OptixRunner::initialize()
{
    // add elements to sys using data structure from SimulationData

    // set number of rays 

    // set sun vector, and other sun properties

    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::setup_simulation(const SimulationData *data)
{

    RunnerStatus sts;

    // this->simdata = data;

    this->setup_parameters(data);
    this->setup_sun(data);
    sts = this->setup_elements(data);

    sys.initialize();

    // std::cout << "Number of stages: " << this->tsys.StageList.size()
    //           << std::endl;

    return sts;
}

RunnerStatus OptixRunner::setup_parameters(const SimulationData *data)
{
    // Get Parameter data
    // TODO: Check that these parameters are used as expected
    const SimulationParameters &sim_params = data->get_simulation_parameters();


    //this->tsys.sim_errors_sunshape = sim_params.include_sun_shape_errors;
    //this->tsys.sim_errors_optical = sim_params.include_optical_errors;
    //this->tsys.sim_raycount = sim_params.number_of_rays;
    //this->tsys.sim_raymax = sim_params.max_number_of_rays;
    //this->tsys.seed = sim_params.seed;
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::setup_sun(const SimulationData *data)
{
    // Get RaySource data (this runner assumes there is only the Sun)
    assert(data->get_number_of_ray_sources() == 1);

    sys.set_sun_vector(0, 0, 10);

    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::setup_elements(const SimulationData *data)
{
    for (auto iter = data->get_const_iterator();
         !data->is_at_end(iter);
         ++iter)
    {
        element_ptr el = iter->second;
        if (el->is_enabled())
        {

			 std::cout << "adding elements " << el->get_name() << std::endl;
        }
    }
    // std::cout << "Number of stages: " << sys.StageList.size() << std::endl;
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::update_simulation(const SimulationData *data)
{
    this->setup_simulation(data);
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::run_simulation()
{

    sys.run();
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::report_simulation(SimulationResult *result,
                                             int level)
{
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}
