
#include "simulation_runner/optix_runner/optix_runner.hpp"
#include "simulation_data/simulation_parameters.hpp"
#include "simulation_data/simulation_data.hpp"
#include "simulation_data/simulation_data_export.hpp"

using SolTrace::Runner::RunnerStatus;
using SolTrace::Runner::SimulationRunner;

using SolTrace::Result::SimulationResult;

OptixRunner::OptixRunner() : SimulationRunner(),
                             m_simdata(nullptr),
                             m_sys(10000, 10000*10) {}

OptixRunner::~OptixRunner()
{
    this->m_sys.clean_up();
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
    m_sys.set_number_of_rays(sim_params.number_of_rays, sim_params.max_number_of_rays);
    m_sys.set_seed(static_cast<uint64_t>(sim_params.seed));
    
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

    m_sys.set_sun_vector(ToVec3d(data->get_ray_source()->get_position()));

    //  TODO: sun angle and sun models

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
            // Skip if element is not a single (i.e. stage, composite)
            if (el->is_single() == false)
                continue;
            
            auto optix_el = std::make_shared<OptixCSP::CspElement>();  
            Vector3d origin = el->get_origin_global();  
            OptixCSP::Vec3d origin_vec(origin[0], origin[1], origin[2]);  
            optix_el->set_origin(ToVec3d(origin));  
            optix_el->set_aim_point(ToVec3d(el->get_aim_vector_global()));
            
            // Safely narrow element id to int32_t
            const auto id = el->get_id(); // int
            if (id < std::numeric_limits<int32_t>::min() || id > std::numeric_limits<int32_t>::max()) {
                throw std::overflow_error("Element id out of int32_t range");
            }
            optix_el->set_id(static_cast<int32_t>(id));

            // TODO: check zrot, radiance or degree here?

            // Add optical properties
            OpticalProperties* opt_front = el->get_front_optical_properties();
            OpticalProperties* opt_back = el->get_back_optical_properties();
            optix_el->set_optics_front(opt_front->my_type == InteractionType::REFRACTION, opt_front->reflectivity,
                opt_front->transmitivity, opt_front->slope_error, opt_front->specularity_error);
            optix_el->set_optics_back(opt_back->my_type == InteractionType::REFRACTION, opt_back->reflectivity,
                opt_back->transmitivity, opt_back->slope_error, opt_back->specularity_error);

            std::cout << "adding elements " << el->get_name() << std::endl;
            std::cout << "Origin: " << origin[0] << ", " << origin[1] << ", " << origin[2] << std::endl;

            if (el->get_surface() != nullptr)
            {
                std::cout << "surface type: " << el->get_surface()->get_type() << std::endl;

                switch (el->get_surface()->get_type())
                {
                    case SurfaceType::FLAT:
                    {
                        auto surface = std::make_shared<OptixCSP::SurfaceFlat>();
                        optix_el->set_surface(surface);

                        break;  
                    }  
                    case SurfaceType::PARABOLA: {  
                        auto el_surface = std::dynamic_pointer_cast<Parabola>(el->get_surface());
                        double fx = el_surface->focal_length_x;
                        double fy = el_surface->focal_length_y;

                        double cx = 1. / (2. * fx);
                        double cy = 1. / (2. * fy);

                        auto optix_surface = std::make_shared<OptixCSP::SurfaceParabolic>();  
                        optix_surface->set_curvature(cx, cy);
                        optix_el->set_surface(optix_surface);

                        break;  
                    }  
                    case SurfaceType::CYLINDER: {  
                        auto el_surface = std::dynamic_pointer_cast<Cylinder>(el->get_surface());
                        
                        auto surface = std::make_shared<OptixCSP::SurfaceCylinder>();  
                        surface->set_half_height(2.);   // TODO this needs to come from the aperture
						surface->set_radius(el_surface->radius);
                        optix_el->set_surface(surface);

                    break;
                }
                default:
                    std::cerr << "Unsupported surface type in OptixCSP" << std::endl;
                    break;
                }

                auto soltrace_aperture_type = el->get_aperture()->get_type();

                switch (soltrace_aperture_type)
                {

                case ApertureType::RECTANGLE:
                {

                    auto el_aperture = std::dynamic_pointer_cast<Rectangle>(el->get_aperture());

                    // TODO: account for x and y coord?
                    auto aperture = std::make_shared<OptixCSP::ApertureRectangle>(el_aperture->x_length, el_aperture->y_length);
                    optix_el->set_aperture(aperture);
                    break;
                }

                default:
                    std::cerr << "Unsupported aperture type in OptixCSP" << std::endl;
                    break;
                }

                m_sys.add_element(optix_el);
            }
            std::cout << "=====================================================" << std::endl;
        }
    }
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
    return run_simulation_core(false);
}

RunnerStatus OptixRunner::run_simulation_core(bool write_output)
{

    m_sys.run();

    if (write_output)
        m_sys.write_hp_output("output.txt");
    
    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::status_simulation(double *progress)
{
    // TODO: Implement this
    return RunnerStatus::SUCCESS;
}

// Temporary function to get hit points
RunnerStatus OptixRunner::get_hp_output(std::vector<float4>& hp_vec, std::vector<int>& raynumber_vec, 
    std::vector<int>& element_id_vec)
{
    // for different levels of reporting, populate result accordingly 
    // 
    std::vector<uint8_t> hit_type_vec;
    m_sys.get_hp_output(hp_vec, raynumber_vec, element_id_vec, hit_type_vec);
    return RunnerStatus::SUCCESS;
}

SolTrace::Result::RayEvent hit_type_to_ray_event(OptixCSP::HitType hit_type)
{
    if (hit_type == OptixCSP::HitType::HIT_UNASSIGNED
        || hit_type == OptixCSP::HitType::HIT_UNKNOWN)
        return SolTrace::Result::RayEvent::UNKNOWN;

    return static_cast<SolTrace::Result::RayEvent>(hit_type);
}

RunnerStatus OptixRunner::report_simulation(SimulationResult *result,
                                            int level)
{
    // Declare results
    RunnerStatus retval = RunnerStatus::SUCCESS;
    std::map<unsigned int, SolTrace::Result::ray_record_ptr> ray_records;
    std::map<unsigned int, SolTrace::Result::ray_record_ptr>::iterator iter;

    // Get results from optixcsp
    std::vector<float4> hp_vec;
    std::vector<int> raynumber_vec;
    std::vector<int32_t> element_id_vec;
    std::vector<uint8_t> hit_type_vec;
    m_sys.get_hp_output(hp_vec, raynumber_vec, element_id_vec, hit_type_vec);

    // Check sizes
    if (!(hp_vec.size() == raynumber_vec.size()
        && raynumber_vec.size() == element_id_vec.size()
        && element_id_vec.size() == hit_type_vec.size()))
    {
        return RunnerStatus::ERROR;
    }

    // Loop through data, populating ray records
    // Assumes ray data is grouped serially
    size_t ndata = hp_vec.size();
    int raynum_prev = -1;
    int raynum = 0;
    SolTrace::Result::ray_record_ptr rec = nullptr;
    SolTrace::Result::interaction_ptr intr = nullptr;
    for (size_t ii = 0; ii < ndata; ++ii)
    {
        // Collect results for record
        raynum = raynumber_vec[ii];
        Vector3d pos = Vector3d(hp_vec[ii].y, hp_vec[ii].z, hp_vec[ii].w);  // x is depth
        Vector3d cos = Vector3d(0, 0, 0);   // TODO: calculate directions
        int32_t element_id = element_id_vec[ii];
        uint8_t hit_type = hit_type_vec[ii];
        SolTrace::Result::RayEvent rev = hit_type_to_ray_event(static_cast<OptixCSP::HitType>(hit_type));

        // Make new ray record if necessary
        iter = ray_records.find(raynum);
        if (iter == ray_records.end())
        {
            rec = SolTrace::Result::make_ray_record(raynum);
            result->add_ray_record(rec);
            ray_records[raynum] = rec;
            assert(rev == SolTrace::Result::RayEvent::CREATE);
        }
        else
        {
            rec = iter->second;
        }

        // Make interaction record
        intr = SolTrace::Result::make_interaction_record(element_id, rev, pos, cos);
        rec->add_interaction_record(intr);
    }

    // Attach other results
    result->set_sun_ray_count(this->get_N_sun_rays());
    result->set_sun_dimensions(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
    result->set_sun_A_box(this->get_sun_plane_area());

    return RunnerStatus::SUCCESS;
}

RunnerStatus OptixRunner::cancel_simulation()
{
    RunnerStatus sts = RunnerStatus::ERROR;

    // TODO: Implement actual cancel

    return sts;
}

OptixCSP::Vec3d OptixRunner::ToVec3d(Vector3d v)
{

    OptixCSP::Vec3d vec(v[0], v[1], v[2]);
    return vec;
}
