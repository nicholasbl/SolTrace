
#include <map>
// #include <algorithm>

#include "native_runner.hpp"
#include "simulation_parameters.hpp"
#include "simulation_data.hpp"
#include "trace.hpp"

NativeRunner::NativeRunner() : SimulationRunner(),
                               as_power_tower(false),
                               number_of_threads(1)
{
}

NativeRunner::~NativeRunner()
{
}

RunnerStatus NativeRunner::initialize()
{
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_simulation(const SimulationData *data)
{

    RunnerStatus sts;

    sts = this->setup_parameters(data);

    if (sts == RunnerStatus::SUCCESS)
        sts = this->setup_sun(data);

    if (sts == RunnerStatus::SUCCESS)
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
    // this->tsys.Sun.set_values(data->get_ray_source());
    ray_source_ptr sun = data->get_ray_source();
    vector_copy(this->tsys.Sun.Origin, sun->get_position());
    this->tsys.Sun.ShapeIndex = sun->get_shape();
    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_elements(const SimulationData *data)
{
    RunnerStatus sts = RunnerStatus::SUCCESS;
    auto my_map = std::map<int_fast64_t, tstage_ptr>();

    for (auto iter = data->get_const_iterator();
         !data->is_at_end(iter);
         ++iter)
    {
        element_ptr el = iter->second;
        if (el->is_enabled() && el->is_stage())
        {
            tstage_ptr stage = make_tstage(el, this->eparams);
            auto retval = my_map.insert(
                std::make_pair(el->get_stage(), stage));
            if (retval.second == false)
            {
                // TODO: Duplicate stage numbers. Need to make an error
                // message.
                sts = RunnerStatus::ERROR;
            }
        }
    }

    if (my_map.size() == 0)
    {
        // No stage elements found in the passed in data. However,
        // the runner requires stages. So make a single stage
        // and put everything there. Note that the coordinates are
        // set to correspond to global coordinates. This is necessary
        // so that the element coordinate setup in make_element are
        // correct.
        int_fast64_t element_number = 1;
        auto stage = make_tstage();
        stage->ElementList.reserve(data->get_number_of_elements());
        for (auto iter = data->get_const_iterator();
             !data->is_at_end(iter);
             ++iter)
        {
            element_ptr el = iter->second;
            if (el->is_enabled() && el->is_single())
            {
                telement_ptr tel = make_telement(el,
                                                 element_number,
                                                 this->eparams);
                stage->ElementList.push_back(tel);
                ++element_number;
                // stage->ElementList.insert(
                //     std::make_pair(tel->element_number, tel));
            }
        }
        my_map.insert(std::make_pair(0, stage));
    }

    // std::map (according to the documentation) is automatically
    // ordered by the keys so inserting into a map will sort the stages
    // and we can just transfer the pointers, in order, to the StageList
    // simply by pulling them out of the map.
    int_fast64_t last_stage_id = -1;
    for (auto iter = my_map.cbegin();
         iter != my_map.cend();
         ++iter)
    {
        assert(last_stage_id < iter->first);
        last_stage_id = iter->first;
        this->tsys.StageList.push_back(iter->second);
    }

    return sts;
}

RunnerStatus NativeRunner::update_simulation(const SimulationData *data)
{
    // TODO: Do a more efficient implementation of this?
    this->tsys.ClearAll();
    this->setup_simulation(data);
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
        this->as_power_tower);

    this->tsys.CollectResults();

    return trace_return ? RunnerStatus::SUCCESS : RunnerStatus::ERROR;
}

RunnerStatus NativeRunner::report_simulation(SimulationResult *result,
                                             int level)
{
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}
