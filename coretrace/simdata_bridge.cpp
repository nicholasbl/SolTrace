#include "simdata_bridge.h"
#include "simulation_data/simulation_data.hpp"
#include "types.h"
#include "ray_source.hpp"
#include "sun.hpp"
#include "stage_element.hpp"
#include "single_element.hpp"
#include <optix_runner.hpp>
#include <native_runner.hpp>

// Private

// TODO: These helper functions (except make_optics) are copied from simdata_io.cpp

SolTrace::Data::DistributionType char_to_distribution(const char dist_char)
{
    switch (dist_char)
    {
        case ('g'):
        {
            return SolTrace::Data::DistributionType::GAUSSIAN;
        }
        case ('p'):
        {
            return SolTrace::Data::DistributionType::PILLBOX;
        }
        case ('d'):
        {
            return SolTrace::Data::DistributionType::USER_DEFINED;
        }
        default:
        {
            return SolTrace::Data::DistributionType::GAUSSIAN;
        }
    }
}

SolTrace::Data::ApertureType char_to_aperture(const char aperture_char)
{
    switch (aperture_char)
    {
        case ('c'):
        {
            return SolTrace::Data::ApertureType::CIRCLE;
        }
        case ('h'):
        {
            return SolTrace::Data::ApertureType::HEXAGON;
        }
        case ('t'):
        {
            return SolTrace::Data::ApertureType::EQUILATERAL_TRIANGLE;
        }
        case ('r'):
        {
            return SolTrace::Data::ApertureType::RECTANGLE;
        }
        case ('a'):
        {
            return SolTrace::Data::ApertureType::ANNULUS;
        }
        case ('l'):
        {
            return SolTrace::Data::ApertureType::SINGLE_AXIS_CURVATURE_SECTION;
        }
        case ('i'):
        {
            return SolTrace::Data::ApertureType::IRREGULAR_TRIANGLE;
        }
        case ('q'):
        {
            return SolTrace::Data::ApertureType::IRREGULAR_QUADRILATERAL;
        }
        default:
        {
            return SolTrace::Data::ApertureType::APERTURE_UNKNOWN;
        }
    }
}

SolTrace::Data::SurfaceType char_to_surface(const char surface_char)
{
    switch (surface_char)
    {
        case ('s'):
            return SolTrace::Data::SurfaceType::SPHERE;
        case ('p'):
            return SolTrace::Data::SurfaceType::PARABOLA;
        case ('o'):
            return SolTrace::Data::SurfaceType::HYPER;
        case ('g'):
            return SolTrace::Data::SurfaceType::GENERAL_SPENCER_MURTY;
        case ('f'):
            return SolTrace::Data::SurfaceType::FLAT;
        case ('c'):
            return SolTrace::Data::SurfaceType::CONE;
        case ('t'):
            return SolTrace::Data::SurfaceType::CYLINDER;
        case ('d'):
            return SolTrace::Data::SurfaceType::TORUS;
        default:
            return SolTrace::Data::SurfaceType::SURFACE_UNKNOWN;
    }
}

SolTrace::Data::InteractionType int_to_interaction(const int interaction_int)
{
    switch (interaction_int)
    {
        case (1):
        {
            return SolTrace::Data::InteractionType::REFRACTION;
        }
        case (2):
        {
            return SolTrace::Data::InteractionType::REFLECTION;
        }
        default:
        {
            return SolTrace::Data::InteractionType::REFLECTION;
        }
    }
}

SolTrace::Data::OpticalProperties make_optics(TOpticalProperties optics_legacy, int interaction_type_int)
{
    // TODO: Not using any transmissivity or reflectivity tables

    // Get interaction type
    SolTrace::Data::InteractionType interaction_type = int_to_interaction(interaction_type_int);

    // Get error distribution type
    SolTrace::Data::DistributionType dist_type = char_to_distribution(optics_legacy.DistributionType);

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

// Public

int convert_tsystem_to_sim_data(TSystem* sys, SolTrace::Data::SimulationData &sd)
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
        SolTrace::Data::DistributionType sun_shape = char_to_distribution(sys->Sun.ShapeIndex);
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

            // Add elements to stage
            int i_element = 0;
            for (TElement* el_legacy : stage_legacy->ElementList)
            {
                // Make element
                SolTrace::Data::element_ptr element = SolTrace::Data::make_element<SolTrace::Data::SingleElement>();

                // Make aperture
                SolTrace::Data::ApertureType aperture_type = char_to_aperture(el_legacy->ShapeIndex);
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
                SolTrace::Data::SurfaceType surface_type = char_to_surface(el_legacy->SurfaceIndex);
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
                SolTrace::Data::OpticalProperties optics_front = make_optics(el_legacy->Optics->Front, el_legacy->InteractionType);
                SolTrace::Data::OpticalProperties optics_back = make_optics(el_legacy->Optics->Back, el_legacy->InteractionType);
                element->set_front_optical_properties(optics_front);
                element->set_back_optical_properties(optics_back);

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
	

	return static_cast<int>(ConversionErrors::SUCCESS);;
}

int run_native_runner(SolTrace::Data::SimulationData& sd, TSystem* sys)
{
    SolTrace::NativeRunner::NativeRunner runner;
    SolTrace::Runner::RunnerStatus sts = runner.initialize();
    sts = runner.setup_simulation(&sd);
    sts = runner.run_simulation();

    // Hack to get RayData out of native runner
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
    const auto nCount = tsys_native->AllRayData.Count();
    for (uint_fast64_t i = 0; i < nCount; ++i)
    {
        // Native TRayData::ray_t layout (Query API)
        double pos[3], dir[3];
        int element = 0, stageIdx1 = 0;
        unsigned int raynum = 0;

        // Native runner TRayData::Query signature includes RayEvent;
        // we only need the legacy fields, so pass nullptr for the event.
        if (!tsys_native->AllRayData.Query(
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

    // Assign raydata to TSystem (for legacy GUI)
    sys->AllRayData.Clear();
    for (TStage* stage : sys->StageList)
        sys->AllRayData.Merge(stage->RayData);
    int i_element = 0;
    for (float4 element : hp_vec)
    {
        double unknown = element.x; // This is *probably* which element the ray hits
        double PosRaySurfStage[3] = {element.y, element.z, element.w};
        double CosRaySurfStage[3] = { 0,0,0 };  // Don't have cos reported from optix
        int element_number = 1; // Don't get element number from optix
        int stage_number = 1;   // Don't get stage number from optix
        int raynumber = raynumber_vec[i_element];
        sys->StageList[0]->RayData.Append(PosRaySurfStage, CosRaySurfStage, element_number,
            stage_number, raynumber);
        i_element++;
    }

    for (TStage* stage : sys->StageList)
        sys->AllRayData.Merge(stage->RayData);
    
    return 0;
}