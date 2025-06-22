
#include "simulation_runner/optix_runner/optix_runner.hpp"
#include "simulation_data/simulation_parameters.hpp"
#include "simulation_data/simulation_data.hpp"

#include "core/vec3d.h"
#include "core/CspElement.h"
#include "core/surface.h"
#include "core/aperture.h"

OptixRunner::OptixRunner() : SimulationRunner(),
                             m_simdata(nullptr),    
                             m_sys(10000) {}


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

    m_sys.initialize();

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

    m_sys.set_sun_vector(OptixCSP::Vec3d(0,0,10));

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

            auto optix_el = std::make_shared<OptixCSP::CspElement>();
			Vector3d origin = el->get_origin_global();

			OptixCSP::Vec3d origin_vec(origin[0], origin[1], origin[2]);

			optix_el->set_origin(ToVec3d(origin));

			optix_el->set_aim_point(ToVec3d(el->get_aim_vector_global()));
            
            std::cout << "adding elements " << el->get_name() << std::endl;
			std::cout << "Origin: " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;

            if (el->get_surface() != nullptr) {
                std::cout << "surface type: " << el->get_surface()->get_type() << std::endl;

                auto surface = std::make_shared<OptixCSP::SurfaceFlat>();

                optix_el->set_surface(surface);
                auto aperture = std::make_shared<OptixCSP::ApertureRectangle>(2,2);
                optix_el->set_aperture(aperture);
                
				m_sys.add_element(optix_el);

            }
			std::cout << "=====================================================" << std::endl;

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

    m_sys.run();
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::report_simulation(SimulationResult *result,
                                             int level)
{
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}


OptixCSP::Vec3d OptixRunner::ToVec3d(Vector3d v) {

    OptixCSP::Vec3d vec(v[0], v[1], v[2]);
    return vec;
}
