
#include "native_runner.hpp"

#include <exception>
#include <map>
// #include <algorithm>

#include "composite_element.hpp"
#include "element.hpp"
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
    // TODO: This should throw an error...
    // Get RaySource data (this runner assumes there is only the Sun)
    assert(data->get_number_of_ray_sources() == 1);
    // this->tsys.Sun.set_values(data->get_ray_source());
    ray_source_ptr sun = data->get_ray_source();
    vector_copy(this->tsys.Sun.Origin, sun->get_position());
    this->tsys.Sun.ShapeIndex = sun->get_shape();

    // Set sunshape data
    switch (sun->get_shape())
    {
        case DistributionType::GAUSSIAN:
            this->tsys.Sun.Sigma = sun->get_sigma();
			break;
		case DistributionType::PILLBOX:
			this->tsys.Sun.Sigma = sun->get_half_width();
            break;
        case DistributionType::USER_DEFINED:
			std::vector<double> angle, intensity;
            sun->get_user_data(angle, intensity);
            int npoints = angle.size();

            // Set user data
            this->tsys.Sun.MaxAngle = 0;
            this->tsys.Sun.MaxIntensity = 0;

            this->tsys.Sun.SunShapeAngle.resize(2 * npoints - 1);
            this->tsys.Sun.SunShapeIntensity.resize(2 * npoints - 1);

            for (int i = 0; i < npoints; i++)
            {
                this->tsys.Sun.SunShapeAngle[npoints + i - 1] = angle[i];
                this->tsys.Sun.SunShapeIntensity[npoints + i - 1] = intensity[i];

                if (angle[i] > this->tsys.Sun.MaxAngle) this->tsys.Sun.MaxAngle = angle[i];
                if (intensity[i] > this->tsys.Sun.MaxIntensity) this->tsys.Sun.MaxIntensity = intensity[i];
            }

            // fill negative angle side of array
            for (int i = 0; i < npoints - 1; i++)
            {
                this->tsys.Sun.SunShapeAngle[i] = -angle[npoints - i - 1];
                this->tsys.Sun.SunShapeIntensity[i] = intensity[npoints - i - 1];
            }
            
    }

    return RunnerStatus::SUCCESS;
}

RunnerStatus NativeRunner::setup_elements(const SimulationData *data)
{
    // TODO: Improve error messages from this function.

    RunnerStatus sts = RunnerStatus::SUCCESS;
    auto my_map = std::map<int_fast64_t, tstage_ptr>();
    // int_fast64_t current_stage_id = -1;
    tstage_ptr current_stage = nullptr;
    int_fast64_t element_number = 1;
    bool element_found_before_stage = false;

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

            // current_stage_id = stage->stage_id;

            // std::cout << "Created stage " << el->get_stage()
            //           << " with " << stage->ElementList.size() << " elements"
            //           << std::endl;

            if (retval.second == false)
            {
                // TODO: Duplicate stage numbers. Need to make an error
                // message.
                sts = RunnerStatus::ERROR;
            }

            current_stage = stage;
            element_number = 1;
        }
        else if (el->is_enabled() && el->is_single())
        {
            if (current_stage == nullptr)
            {
                // throw std::runtime_error("No stage to add element to");
                element_found_before_stage = true;
                continue;
            }
            else if (el->get_stage() != current_stage->stage_id)
            {
                throw std::runtime_error(
                    "Element does not match current stage");
            }

            telement_ptr elem = make_telement(iter->second,
                                              element_number,
                                              this->eparams);
            ++element_number;
            current_stage->ElementList.push_back(elem);
        }
    }

    if (my_map.size() != 0 && element_found_before_stage)
    {
        throw std::runtime_error("Element found without a stage");
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
        auto stage = make_tstage(this->eparams);
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

    if (sts == RunnerStatus::SUCCESS)
    {
        // std::cout << "Setting ZAperture..." << std::endl;
        // Compute and set ZAperture field in each element
        bool success = set_aperture_planes(&this->tsys);
        sts = success ? RunnerStatus::SUCCESS : RunnerStatus::ERROR;
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

bool NativeRunner::set_aperture_planes(TSystem *tsys)
{
    bool retval;

    for (auto iter = tsys->StageList.cbegin();
         iter != tsys->StageList.cend();
         ++iter)
    {
        retval = this->set_aperture_planes(*iter);
        if (!retval)
            break;
    }

    return retval;
}

bool NativeRunner::set_aperture_planes(tstage_ptr stage)
{
    bool retval;

    for (auto eiter = stage->ElementList.begin();
         eiter != stage->ElementList.end();
         ++eiter)
    {
        retval = aperture_plane(*eiter);
        if (!retval)
            break;
    }

    return retval;
}

bool NativeRunner::aperture_plane(telement_ptr Element)
{
    /*{Calculates the aperture plane of the element in element coord system.
    Applicable to rotationally symmetric apertures surfaces with small
    curvature: g, s, p, o, c, v, m, e, r, i.
      input - Element = Element record containing geometry of element
      output -
             - Element.ZAperture  where ZAperture is the distance from
               the origin to the plane.
    }*/

    Element->ZAperture =
        Element->icalc->compute_z_aperture(Element->aperture);

    return true;
}
