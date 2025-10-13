#include "simdata_bridge.h"
#include "simulation_data/simulation_data.hpp"
#include "types.h"
#include "ray_source.hpp"
#include "sun.hpp"
#include "stage_element.hpp"
#include "single_element.hpp"
#include "virtual_element.hpp"
#include <optix_runner.hpp>
#include <native_runner.hpp>
#include "simdata_io.hpp"

// Private

SolTrace::Data::OpticalProperties make_optics(TOpticalProperties optics_legacy, int interaction_type_int)
{
    // TODO: Not using any transmissivity or reflectivity tables

    // Get interaction type
    SolTrace::Data::InteractionType interaction_type = SolTrace::Data::int_to_interaction(interaction_type_int);

    // Get error distribution type
    SolTrace::Data::DistributionType dist_type = SolTrace::Data::char_to_distribution(optics_legacy.DistributionType);

    double transmissivity = optics_legacy.Transmissivity;
    double reflectivity = optics_legacy.Reflectivity;
    double rms_slope = optics_legacy.RMSSlopeError;
    double rms_spec = optics_legacy.RMSSpecError;
    double refrac_real = optics_legacy.RefractiveIndex[0];  // TODO: verify this is how refractiveindex works (there are 4 values)
    double refrac_imag = optics_legacy.RefractiveIndex[1];

    // Make new optical properties
    SolTrace::Data::OpticalProperties optics = SolTrace::Data::OpticalProperties(interaction_type,
        dist_type, transmissivity, reflectivity, rms_slope, rms_spec, refrac_real, refrac_imag);

    return optics;
}

int assign_raydata_from_hitpoints(const std::vector<float4>& hp_vec, const std::vector<int>& raynumber_vec,
    TSystem* sys)
{
    // Assign raydata to TSystem (for legacy GUI)
    sys->AllRayData.Clear();
    for (TStage* stage : sys->StageList)
        sys->AllRayData.Merge(stage->RayData);
    int i_element = 0;
    for (float4 element : hp_vec)
    {
        double stage_optix = element.x; // This is the DEPTH, NOT the stage in the soltrace sense
        double PosRaySurfStage[3] = { element.y, element.z, element.w };
        double CosRaySurfStage[3] = { 0,0,0 };  // Don't have cos reported from optix
        int element_number = 1; // Don't get element number from optix
        int raynumber = raynumber_vec[i_element];

        // Only add ray data if it is Not the sun ray ('stage' 0)
        if (stage_optix != 0)
        {
            sys->StageList[0]->RayData.Append(PosRaySurfStage, CosRaySurfStage, element_number,
                stage_optix, raynumber);
        }
        i_element++;
    }

    for (TStage* stage : sys->StageList)
        sys->AllRayData.Merge(stage->RayData);

    return 0;
}

// Public

int convert_tsystem_to_sim_data(TSystem* sys, const int seed, SolTrace::Data::SimulationData &sd)
{
	// Sun
	{
		// Check if sun is point source (not supported)
		if (sys->Sun.PointSource == true)
			return static_cast<int>(ConversionErrors::NOT_SUPPORTED_ERROR);

		// Make simdata Sun
		auto sun = SolTrace::Data::make_ray_source<SolTrace::Data::Sun>();

		// Set sun position
		// TODO: Is this valid if using Lat, day, hour?
		sun->set_position(sys->Sun.Origin[0], sys->Sun.Origin[1], sys->Sun.Origin[2]);

        // Set sun shape
        SolTrace::Data::DistributionType sun_shape = SolTrace::Data::char_to_distribution(sys->Sun.ShapeIndex);
        double Sigma = sys->Sun.Sigma;
        double HalfWidth = sys->Sun.Sigma;  // Gaussian and pillobox parameters are stored in Sun.Sigma
        sun->set_shape(sun_shape, Sigma, HalfWidth, sys->Sun.SunShapeAngle, sys->Sun.SunShapeIntensity);

        // Attach sun to simulation data
        sd.add_ray_source(sun);
	}

	// Stages
    {
        // Loop through stages
        int i_stage = 0;
        for (TStage* stage_legacy : sys->StageList)
        {
            // Make stage
            SolTrace::Data::stage_ptr stage = SolTrace::Data::make_stage(i_stage);
            stage->set_origin(stage_legacy->Origin[0], stage_legacy->Origin[1], stage_legacy->Origin[2]);
            stage->set_aim_vector(stage_legacy->AimPoint[0], stage_legacy->AimPoint[1], stage_legacy->AimPoint[2]);
            stage->set_zrot(stage_legacy->ZRot);
            stage->compute_coordinate_rotations();

            // Check if virtual
            bool is_virtual = stage_legacy->Virtual;

            // Add elements to stage
            int i_element = 0;
            for (TElement* el_legacy : stage_legacy->ElementList)
            {
                // Make element
                SolTrace::Data::element_ptr element;
                if (is_virtual == true)
                    element = SolTrace::Data::make_element<SolTrace::Data::VirtualElement>();
                else
                    element = SolTrace::Data::make_element<SolTrace::Data::SingleElement>();

                // Make aperture
                SolTrace::Data::ApertureType aperture_type = SolTrace::Data::char_to_aperture(el_legacy->ShapeIndex);
                if (aperture_type == SolTrace::Data::ApertureType::APERTURE_UNKNOWN)
                    return static_cast<int>(ConversionErrors::APERTURE_ERROR);
                std::vector<double> aperture_args = { el_legacy->ParameterA, el_legacy->ParameterB,
                    el_legacy->ParameterC, el_legacy->ParameterD, el_legacy->ParameterE,
                    el_legacy->ParameterF, el_legacy->ParameterG, el_legacy->ParameterH };
                SolTrace::Data::aperture_ptr aperture_ptr = SolTrace::Data::Aperture::make_aperture_from_type(
                    aperture_type, aperture_args);
                if (aperture_ptr == nullptr)
                    return static_cast<int>(ConversionErrors::APERTURE_ERROR);
                element->set_aperture(aperture_ptr);

                // Make surface
                SolTrace::Data::SurfaceType surface_type = SolTrace::Data::char_to_surface(el_legacy->SurfaceIndex);
                if (surface_type == SolTrace::Data::SurfaceType::SURFACE_UNKNOWN)
                    return static_cast<int>(ConversionErrors::SURFACE_ERROR);
                // TODO: validate surface args (more than just vertexcurv X and Y)
                std::vector<double> surface_args = { el_legacy->VertexCurvX, el_legacy->VertexCurvY };
                SolTrace::Data::surface_ptr surface_ptr = SolTrace::Data::make_surface_from_type(surface_type, surface_args);
                if (surface_ptr == nullptr)
                    return static_cast<int>(ConversionErrors::SURFACE_ERROR);
                element->set_surface(surface_ptr);

                // Set element position and orientation
                element->set_origin(el_legacy->Origin[0], el_legacy->Origin[1], el_legacy->Origin[2]);
                element->set_aim_vector(el_legacy->AimPoint[0], el_legacy->AimPoint[1], el_legacy->AimPoint[2]);
                element->set_zrot(el_legacy->ZRot);

                // Set optical properties
                if (is_virtual == false)
                {
                    SolTrace::Data::OpticalProperties optics_front = make_optics(el_legacy->Optics->Front, el_legacy->InteractionType);
                    SolTrace::Data::OpticalProperties optics_back = make_optics(el_legacy->Optics->Back, el_legacy->InteractionType);
                    element->set_front_optical_properties(optics_front);
                    element->set_back_optical_properties(optics_back);
                }

                // Set element name
                element->set_name(std::to_string(i_element));

                // Attach to stage
                if (!SolTrace::Data::Element::is_success(stage->add_element(element)))
                    return static_cast<int>(ConversionErrors::ELEMENT_ADD_ERROR);

                i_element++;
            }

            // Add stage to simulation data
            if (!SolTrace::Data::Element::is_success(sd.add_stage(stage)))
                return static_cast<int>(ConversionErrors::STAGE_ADD_ERROR);

            i_stage++;
        }
    }
	
    // Parameters
    {
        SolTrace::Data::SimulationParameters& par = sd.get_simulation_parameters();
        par.number_of_rays = sys->sim_raycount;
        par.max_number_of_rays = sys->sim_raymax;
        par.seed = seed;
        par.include_sun_shape_errors = sys->sim_errors_sunshape;
        par.include_optical_errors = sys->sim_errors_optical;
    }

	return static_cast<int>(ConversionErrors::SUCCESS);;
}

int run_native_runner(SolTrace::Data::SimulationData& sd, TSystem* sys)
{
    SolTrace::NativeRunner::NativeRunner runner;
    SolTrace::Runner::RunnerStatus sts = runner.initialize();
    sts = runner.setup_simulation(&sd);
    sts = runner.run_simulation();

    const SolTrace::NativeRunner::TSystem* tsys_native = runner.get_system();

    // Copy sun stats (needed for flux normalization)
    sys->Sun.MinXSun = tsys_native->Sun.MinXSun;
    sys->Sun.MaxXSun = tsys_native->Sun.MaxXSun;
    sys->Sun.MinYSun = tsys_native->Sun.MinYSun;
    sys->Sun.MaxYSun = tsys_native->Sun.MaxYSun;
    sys->SunRayCount = static_cast<st_uint_t>(tsys_native->SunRayCount);

    for (TStage* s : sys->StageList)
        s->RayData.Clear();
    sys->AllRayData.Clear();

    // Copy from native TOTAL (aggregate) ray list instead of per stage
    const auto nCount = tsys_native->RayData.Count();
    for (uint_fast64_t i = 0; i < nCount; ++i)
    {
        // Native TRayData::ray_t layout (Query API)
        double pos[3], dir[3];
        int element = 0, stageIdx1 = 0;
        unsigned int raynum = 0;

        // Native runner TRayData::Query signature includes RayEvent;
        // we only need the legacy fields, so pass nullptr for the event.
        if (!tsys_native->RayData.Query(
            (unsigned int)i,
            pos,            // pos
            dir,            // cos
            &element,       // element
            &stageIdx1,     // stage
            &raynum,        // ray number
            nullptr))       // event (ignored)
        {
            continue; // skip malformed entry
        }

        if (stageIdx1 < 1 || (size_t)stageIdx1 > sys->StageList.size())
            continue; // out-of-range stage index, skip

        // Append to that stage's RayData (stage-local coords assumed)
        sys->StageList[stageIdx1 - 1]->RayData.Append(
            pos,
            dir,
            element,        // negative if absorbed already encoded
            stageIdx1,      // 1-based
            raynum);
    }

    for (TStage* s : sys->StageList)
        sys->AllRayData.Merge(s->RayData);

    return -1;
}

int run_optix_runner(SolTrace::Data::SimulationData& sd, TSystem* sys)
{
    OptixRunner runner;
    SolTrace::Runner::RunnerStatus sts = runner.initialize();
    sts = runner.setup_simulation(&sd);
    sts = runner.run_simulation_core(false);

    std::vector<float4> hp_vec;
    std::vector<int> raynumber_vec;
    runner.get_hp_output(hp_vec, raynumber_vec);

    assign_raydata_from_hitpoints(hp_vec, raynumber_vec, sys);
    
    return 0;
}

int run_optix_file_runner(TSystem* sys, const char* file_name)
{
    // Directly run OptixCSP using stinput file (debug use ONLY)
    OptixCSP::SolTraceSystem sys_optix(sys->sim_raycount);
    bool ok = sys_optix.read_st_input(file_name);
    sys_optix.set_sun_angle(0.00465);
    sys_optix.initialize();
    sys_optix.run();

    std::vector<float4> hp_vec;
    std::vector<int> raynumber_vec;
    sys_optix.get_hp_output(hp_vec, raynumber_vec);

    assign_raydata_from_hitpoints(hp_vec, raynumber_vec, sys);

    return 0;
}
