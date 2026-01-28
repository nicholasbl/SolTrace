#include <gtest/gtest.h>

#include <optix_runner.hpp>
#include <simulation_data.hpp>
#include <simulation_data_export.hpp>
#include <simulation_result_export.hpp>

using SolTrace::Runner::RunnerStatus;

void make_default_sd(SimulationData& sd, element_ptr& plate)
{
	sd.clear();

	// Sun
	auto sun = make_ray_source<Sun>();
	sun->set_position(0, 0, 100);
	sd.add_ray_source(sun);

	// Make stage
	auto stage = make_stage(0);
	stage->set_origin(0, 0, 0);
	stage->set_aim_vector(0, 0, 1);
	stage->set_name("stage");

	// Make reflective flat plate
	plate = make_element<SingleElement>();
	plate->set_origin(0, 0, 50);
	plate->set_aim_vector(0, 0, 100);	// Face up towards sun
	plate->set_surface(make_surface<Flat>());
	plate->set_aperture(make_aperture<Rectangle>(5, 5));
	InteractionType itype = InteractionType::REFLECTION;
	DistributionType dtype = DistributionType::UNKNOWN;	// No errors
	double transmissivity = 0;
	double reflectivity = 1;
	double slope_err = 0;	// Error not supported
	double spec_err = 0;
	double ri_front = 0;	// Refraction not supported
	double ri_back = 0;
	OpticalProperties plate_optics = OpticalProperties(itype, dtype, transmissivity,
		reflectivity, slope_err, spec_err, ri_front, ri_back);
	plate->set_front_optical_properties(plate_optics);
	plate->set_back_optical_properties(plate_optics);
	plate->set_name("plate");

	// Add element to stage
	stage->add_element(plate);

	// Add stage to sd
	sd.add_stage(stage);

	// Set parameters
	SimulationParameters& params = sd.get_simulation_parameters();
	params.number_of_rays = 10000;
	params.max_number_of_rays = params.number_of_rays * 100;
	params.include_optical_errors = false;
	params.include_sun_shape_errors = false;
	params.seed = 123;
}

void count_hits(const SimulationResult& result,
	int& absorbed_count, int& transmitted_count,
	int& reflected_count)
{
	absorbed_count = 0;
	transmitted_count = 0;
	reflected_count = 0;
	int n_records = result.get_number_of_records();

	for (int i = 0; i < n_records; i++)
	{
		ray_record_ptr rec = result[i];

		int n_interactions = rec->get_number_of_interactions();
		for (int j = 0; j < n_interactions; j++)
		{
			RayEvent rev = rec->get_event(j);

			if (rev == RayEvent::ABSORB)
				absorbed_count++;
			else if (rev == RayEvent::TRANSMIT)
				transmitted_count++;
			else if (rev == RayEvent::REFLECT)
				reflected_count++;
		}
	}

	return;
}

TEST(FlatOptixOptical, Transmissivity)
{
	// Make default simulation data
	SimulationData sd;
	element_ptr plate;
	make_default_sd(sd, plate);

	// Set plate properties (transmissive)
	double transmissivity = 0.8;
	plate->get_front_optical_properties()->my_type = InteractionType::REFRACTION;
	plate->get_front_optical_properties()->transmitivity = transmissivity;
	plate->get_back_optical_properties()->my_type = InteractionType::REFRACTION;
	plate->get_back_optical_properties()->transmitivity = transmissivity;

	// Run simulation
	OptixRunner runner;
	RunnerStatus sts = runner.initialize();
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);
	sts = runner.setup_simulation(&sd);
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);
	sts = runner.run_simulation();
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);

	// Collect results
	SimulationResult result;
	sts = runner.report_simulation(&result, 0);
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);

	// Calculate transmissivity
	int absorbed_count, transmitted_count, reflected_count;
	count_hits(result, absorbed_count, transmitted_count, reflected_count);
	int total_hits = absorbed_count + transmitted_count + reflected_count;
	double trans_calc = (double)transmitted_count / (double)total_hits;
	ASSERT_NEAR(trans_calc, transmissivity, 0.01);

}

TEST(FlatOptixOptical, Reflectivity)
{
	// Make default simulation data
	SimulationData sd;
	element_ptr plate;
	make_default_sd(sd, plate);
	
	// Set plate properties (reflective)
	double reflectivity = 0.6;
	plate->get_front_optical_properties()->my_type = InteractionType::REFLECTION;
	plate->get_front_optical_properties()->reflectivity = reflectivity;
	plate->get_back_optical_properties()->my_type = InteractionType::REFLECTION;
	plate->get_back_optical_properties()->reflectivity = reflectivity;

	// Run simulation
	OptixRunner runner;
	RunnerStatus sts = runner.initialize();
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);
	sts = runner.setup_simulation(&sd);
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);
	sts = runner.run_simulation();
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);

	// Collect results
	SimulationResult result;
	sts = runner.report_simulation(&result, 0);
	EXPECT_EQ(sts, RunnerStatus::SUCCESS);

	// Calculate transmissivity
	int absorbed_count, transmitted_count, reflected_count;
	count_hits(result, absorbed_count, transmitted_count, reflected_count);
	int total_hits = absorbed_count + transmitted_count + reflected_count;
	double refl_calc = (double)reflected_count / (double)total_hits;
	ASSERT_NEAR(refl_calc, reflectivity, 0.01);
}

