#include "heliostat_field_test_template.hpp"

#include <embree_runner.hpp>
#include <optix_runner.hpp>

using SolTrace::EmbreeRunner::EmbreeRunner;

constexpr int N_rays_glob = 1e5;
constexpr int seed = 200;
constexpr bool save = true;
constexpr bool save_hits = false;

static void write_to_dict(std::string key_name, double val_a,
	double val_b, std::map<std::string, double>& dict_a,
	std::map<std::string, double>& dict_b)
{
	dict_a[key_name] = val_a;
	dict_b[key_name] = val_b;
}

static void CompareRunners(HeliostatFieldSimulationHelper<EmbreeRunner>& sim_embree,
	HeliostatFieldSimulationHelper<OptixRunner>& sim_optix, int N_rays,
	std::string hour)
{
	// Run cases
	sim_embree.seed = seed;
	sim_embree.sun_gen_type = SolTrace::Data::GenType::HALTON;
	sim_embree.create_heliostat_field();
	sim_embree.setup_simData();
	sim_embree.update_from_hour(hour);
	SimulationResult result_embree;
	sim_embree.simulate(&result_embree, N_rays);
	sim_embree.calculate_ray_counts(result_embree);
	sim_embree.calculate_sun_size(result_embree);
	sim_embree.calculate_receiver_flux_map(result_embree, 60, 23, true);

	if (save_hits)
	{
		std::string file_hits_embree = "embree_hits_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_embree.save_hit_pos_to_file(result_embree, file_hits_embree);
	}

	sim_optix.seed = seed;
	sim_optix.sun_gen_type = SolTrace::Data::GenType::HALTON;
	sim_optix.create_heliostat_field();
	sim_optix.setup_simData();
	sim_optix.update_from_hour(hour);
	SimulationResult result_optix;
	sim_optix.simulate(&result_optix, N_rays);
	sim_optix.calculate_ray_counts(result_optix);
	sim_optix.calculate_sun_size(result_optix);
	sim_optix.calculate_receiver_flux_map(result_optix, 60, 23, true);

	if (save_hits)
	{
		std::string file_hits_optix = "optix_hits_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_optix.save_hit_pos_to_file(result_optix, file_hits_optix);
	}

	// Error tolerances
	double err_frac = 0.001;
	double err_abs = err_frac * (double)N_rays;

	std::map<std::string, double> dict_embree;
	std::map<std::string, double> dict_optix;

	// Compare hits
	ASSERT_EQ(result_embree.get_number_of_records(), result_optix.get_number_of_records());
	EXPECT_NEAR(sim_embree.tot_helio_hits, sim_optix.tot_helio_hits, err_abs);
	EXPECT_NEAR(sim_embree.tot_helio_absorb_count, sim_optix.tot_helio_absorb_count, err_abs);
	EXPECT_NEAR(sim_embree.tot_reflect_count, sim_optix.tot_reflect_count, err_abs);
	EXPECT_NEAR(sim_embree.rec_absorb_count, sim_optix.rec_absorb_count, err_abs);
	EXPECT_NEAR(sim_embree.tot_helio_block_count, sim_optix.tot_helio_block_count, err_abs);
	EXPECT_NEAR(sim_embree.heat_shield_absorb_count, sim_optix.heat_shield_absorb_count, err_abs);
	EXPECT_NEAR(sim_embree.tot_rec_hits, sim_optix.tot_rec_hits, err_abs);
	EXPECT_NEAR(sim_embree.rec_direct_count, sim_optix.rec_direct_count, err_abs);
	EXPECT_NEAR(sim_embree.rec_via_helio_count, sim_optix.rec_via_helio_count, err_abs);

	write_to_dict("00_tot_helio_hits", sim_embree.tot_helio_hits, sim_optix.tot_helio_hits, dict_embree, dict_optix);
	write_to_dict("01_tot_helio_absorb_count", sim_embree.tot_helio_absorb_count, sim_optix.tot_helio_absorb_count, dict_embree, dict_optix);
	write_to_dict("02_tot_reflect_count", sim_embree.tot_reflect_count, sim_optix.tot_reflect_count, dict_embree, dict_optix);
	write_to_dict("03_rec_absorb_count", sim_embree.rec_absorb_count, sim_optix.rec_absorb_count, dict_embree, dict_optix);
	write_to_dict("04_tot_helio_block_count", sim_embree.tot_helio_block_count, sim_optix.tot_helio_block_count, dict_embree, dict_optix);
	write_to_dict("05_heat_shield_absorb_count", sim_embree.heat_shield_absorb_count, sim_optix.heat_shield_absorb_count, dict_embree, dict_optix);
	write_to_dict("06_tot_rec_hits", sim_embree.tot_rec_hits, sim_optix.tot_rec_hits, dict_embree, dict_optix);
	write_to_dict("07_rec_direct_count", sim_embree.rec_direct_count, sim_optix.rec_direct_count, dict_embree, dict_optix);
	write_to_dict("08_rec_via_helio_count", sim_embree.rec_via_helio_count, sim_optix.rec_via_helio_count, dict_embree, dict_optix);

	// Helio hits add up
	EXPECT_EQ(sim_embree.tot_helio_hits, sim_embree.tot_helio_absorb_count + sim_embree.tot_reflect_count);
	EXPECT_EQ(sim_optix.tot_helio_hits, sim_optix.tot_helio_absorb_count + sim_optix.tot_reflect_count);

	// Receiver hits add up
	EXPECT_EQ(sim_embree.tot_rec_hits, sim_embree.rec_direct_count + sim_embree.rec_via_helio_count);
	EXPECT_EQ(sim_optix.tot_rec_hits, sim_optix.rec_direct_count + sim_optix.rec_via_helio_count);

	// Reflectivity
	double refl_embree = (double)sim_embree.tot_reflect_count / (double)sim_embree.tot_helio_hits;
	double refl_optix = (double)sim_optix.tot_reflect_count / (double)sim_optix.tot_helio_hits;
	EXPECT_NEAR(refl_embree, 0.9, 0.001);
	EXPECT_NEAR(refl_optix, 0.9, 0.001);

	write_to_dict("09_reflectivity", refl_embree, refl_optix, dict_embree, dict_optix);

	// Sun Count
	write_to_dict("10_sun_count", sim_embree.nsun_rays, sim_optix.nsun_rays, dict_embree, dict_optix);

	// Fraction reflected hits that hit receiver
	double frac_via_helio_a = (double)sim_embree.rec_absorb_count / (double)sim_embree.tot_reflect_count;
	double frac_via_helio_b = (double)sim_optix.rec_absorb_count / (double)sim_optix.tot_reflect_count;
	EXPECT_NEAR(frac_via_helio_a, frac_via_helio_b, err_frac);

	write_to_dict("11_frac_via_helio", frac_via_helio_a, frac_via_helio_b, dict_embree, dict_optix);

	// Compare power per ray
	double power_tol = (5. / (double)N_rays) * 1e3;
	EXPECT_NEAR(sim_embree.power_per_ray, sim_optix.power_per_ray, power_tol);

	write_to_dict("12_power_per_ray", sim_embree.power_per_ray, sim_optix.power_per_ray, dict_embree, dict_optix);

	// Total power absorbed
	double tol = 5.e-3;
	double total_power_embree = (double)sim_embree.rec_absorb_count * sim_embree.power_per_ray * 1.e-3; // [kW]
	double total_power_optix = (double)sim_optix.rec_absorb_count * sim_optix.power_per_ray * 1.e-3; // [kW]
	EXPECT_NEAR(total_power_embree, total_power_optix, tol * total_power_embree);

	write_to_dict("13_total_power", total_power_embree, total_power_embree, dict_embree, dict_optix);

	// Peak flux
	double peak_tol = 0.25;
	double peak_flux_embree = sim_embree.PeakFlux / 1.e3;
	double peak_flux_optix = sim_optix.PeakFlux / 1.e3;
	EXPECT_NEAR(peak_flux_embree, peak_flux_optix, peak_tol * peak_flux_embree);

	write_to_dict("14_peak_flux", peak_flux_embree, peak_flux_optix, dict_embree, dict_optix);

	// RMS of flux values
	EXPECT_EQ(sim_embree.fluxGrid.nrows(), sim_optix.fluxGrid.nrows());
	EXPECT_EQ(sim_embree.fluxGrid.ncols(), sim_optix.fluxGrid.ncols());
	double rmse = 0.0;
	double average_flux_embree = 0.0;
	double average_flux_optix = 0.0;
	for (size_t r = 0; r < sim_embree.fluxGrid.nrows(); r++) {
		for (size_t c = 0; c < sim_embree.fluxGrid.ncols(); c++) {
			double flux_embree = sim_embree.fluxGrid.at(r, c) * sim_embree.zScale / 1.e3;
			double flux_optix = sim_optix.fluxGrid.at(r, c) * sim_optix.zScale / 1.e3;
			rmse += pow(flux_embree - flux_optix, 2);
			average_flux_embree += flux_embree;
			average_flux_optix += flux_optix;
		}
	}

	rmse = sqrt(rmse / (sim_embree.fluxGrid.nrows() * sim_embree.fluxGrid.ncols()));
	double rmse_tol = 0.08;  // 2% of peak flux // 0.04
	EXPECT_LE(rmse / peak_flux_embree, rmse_tol);

	// Average flux
	average_flux_embree /= (sim_embree.fluxGrid.nrows() * sim_embree.fluxGrid.ncols());
	average_flux_optix /= (sim_optix.fluxGrid.nrows() * sim_optix.fluxGrid.ncols());
	EXPECT_NEAR(average_flux_embree, average_flux_optix, rmse_tol);

	write_to_dict("15_average_flux", average_flux_embree, average_flux_optix, dict_embree, dict_optix);

	EXPECT_EQ(sim_embree.NotBinned, sim_optix.NotBinned);

	write_to_dict("16_not_binned", sim_embree.NotBinned, sim_optix.NotBinned, dict_embree, dict_optix);

	if (save)
	{
		std::string file_fluxmap_native = "embree_field_flux_" + SolTrace::Data::GenTypeMap.at(sim_embree.sun_gen_type)
			+ "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_embree.save_flux_map_to_file(file_fluxmap_native);

		std::string file_fluxmap_optix = "optix_field_flux_" + SolTrace::Data::GenTypeMap.at(sim_optix.sun_gen_type)
			+ "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_optix.save_flux_map_to_file(file_fluxmap_optix);

		std::string file_outputs_native = "embree_outputs_" + SolTrace::Data::GenTypeMap.at(sim_embree.sun_gen_type)
			+ "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_embree.save_outputs(file_outputs_native, dict_embree);

		std::string file_outputs_optix = "optix_outputs_" + SolTrace::Data::GenTypeMap.at(sim_optix.sun_gen_type)
			+ "_" + std::to_string(int(N_rays / 1e3)) + "k.csv";
		sim_optix.save_outputs(file_outputs_optix, dict_optix);
	}


	int xsagasdg = 0;
}


TEST(HeliostatFieldOptixEmbree, singleFacet_SlantFocused_NoSunShape)
{
	// Make embree
	HeliostatFieldSimulationHelper<EmbreeRunner> sim_embree;
	sim_embree.runner.disable_stages();
	sim_embree.runner.set_number_of_threads(10);
	sim_embree.use_sunshape_errors = false;
	sim_embree.use_optical_errors = false;
	sim_embree.initialize();

	// Make optix
	HeliostatFieldSimulationHelper<OptixRunner> sim_optix;
	sim_optix.use_sunshape_errors = false;
	sim_optix.use_optical_errors = false;
	sim_optix.initialize();

	CompareRunners(sim_embree, sim_optix, N_rays_glob, "8");
}

TEST(HeliostatFieldOptixEmbree, singleFacet_SlantFocused)
{
	// Make embree
	HeliostatFieldSimulationHelper<EmbreeRunner> sim_embree;
	sim_embree.runner.disable_stages();
	sim_embree.runner.set_number_of_threads(10);
	sim_embree.initialize();

	// Make optix
	HeliostatFieldSimulationHelper<OptixRunner> sim_optix;
	sim_optix.initialize();

	CompareRunners(sim_embree, sim_optix, N_rays_glob, "8");
}

TEST(HeliostatFieldOptixEmbree, optix_only)
{
	// Make optix
	int N_rays = 100e3;
	HeliostatFieldSimulationHelper<OptixRunner> sim_optix;
	sim_optix.use_sunshape_errors = false;
	sim_optix.use_optical_errors = false;
	sim_optix.initialize();
	sim_optix.seed = seed;
	sim_optix.sun_gen_type = SolTrace::Data::GenType::HALTON;
	sim_optix.create_heliostat_field();
	sim_optix.setup_simData();
	sim_optix.update_from_hour("8");
	SimulationResult result_b;
	sim_optix.simulate(&result_b, N_rays);
	sim_optix.calculate_ray_counts(result_b);
	sim_optix.calculate_sun_size(result_b);
	sim_optix.calculate_receiver_flux_map(result_b, 30, 30, false);
}


TEST(NSTTF, test_case)
{
	std::string filename = "C://Users//tbrown2//OneDrive - NREL//SolTrace Help//GPU Upgrade(10302 - 49.01.01)//NSTTF Sample Case//NSTTF_2025_03_31_11_15_00.json";
	SimulationData sd;
	sd.import_json_file(filename);

	EmbreeRunner runner_embree;
	runner_embree.setup_simulation(&sd);
	runner_embree.run_simulation();
	SimulationResult result_embree;
	runner_embree.report_simulation(&result_embree, 0);
	
	OptixRunner runner_optix;
	runner_optix.setup_simulation(&sd);
	runner_optix.run_simulation();
	SimulationResult result_optix;
	runner_optix.report_simulation(&result_optix, 0);

	int x = 0;

}