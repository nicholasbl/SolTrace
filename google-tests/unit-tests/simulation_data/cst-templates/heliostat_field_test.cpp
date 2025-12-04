#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector> 
#include <numeric> 

#include <gtest/gtest.h>

#include <native_runner.hpp>
#include <native_runner_types.hpp>
#include <simulation_data.hpp>
#include <simulation_result_export.hpp>
#include <stage_element.hpp>
#include <sun.hpp>

#include <../../hpvm.h>

#include <cst_templates/heliostat.hpp>
#include <cst_templates/utilities.hpp>

#include "common.hpp"
#include "count_absorbed_native.h"

using Heliostat = SolTrace::Data::Heliostat;

using SolTrace::Runner::RunnerStatus;
using SolTrace::NativeRunner::NativeRunner;
using SolTrace::NativeRunner::TRayData;
using SolTrace::NativeRunner::TSystem;
using SolTrace::NativeRunner::TSun;

class HeliostatFieldSimulation : public ::testing::Test {
public:
    const Vector3d zero = { 0.0, 0.0, 0.0 }; // Global origin
    const Vector3d khat = { 0.0, 0.0, 1.0 }; // Global z-axis

    double solar_azimuth = 0.0;
    double solar_elevation = 61.97;

    // Receiver parameters
    Vector3d rec_origin = { 0.0, 0.0, 180.33 };   // This is the receiver center
    double rec_radius = 15.45 / 2.0;
    double rec_height = 18.59;
    double rec_heat_shield_height = 3.2331;     // TODO: Above and below receiver aperture

    std::vector<std::shared_ptr<Heliostat>> heliostat_field;
    std::shared_ptr<SolTrace::Data::SingleElement> receiver;

protected:
    SimulationData simData;
    NativeRunner runner;
    SimulationResult result;

    // Sun outputs
    double sun_width;
    double sun_height;
    double A_sun_box;
    double power_per_ray;

    // Ray counts
    std::vector<uint_fast64_t> helio_hit_counts;
    std::vector<uint_fast64_t> reflect_counts;
    std::vector<uint_fast64_t> helio_absorb_counts;
    std::vector<uint_fast64_t> helio_block_counts;
    uint_fast64_t tot_helio_hits;
    uint_fast64_t tot_reflect_count;
    uint_fast64_t tot_helio_absorb_count;
    uint_fast64_t tot_helio_block_count;
    uint_fast64_t rec_absorb_count;
    uint_fast64_t miss_count;

    // Flux map
    HPM2D fluxGrid;
    std::vector<double> xValues, yValues;
    double binszx, binszy;
    double PeakFlux, PeakFluxUncertainty;
    double AveFlux, AveFluxUncertainty;
    double MinFlux, SigmaFlux, Uniformity;
    double Centroid[3];
    double zScale;
    size_t NumberOfRays;


    void SetUp() override {
        // Set parameters
        SimulationParameters& params = simData.get_simulation_parameters();
        params.number_of_rays = 1.e5;
        params.max_number_of_rays = params.number_of_rays * 100;
        params.include_optical_errors = true;
        params.include_sun_shape_errors = true;
        params.seed = 123;

        // Initial setup of receiver
        receiver = SolTrace::Data::make_element<SingleElement>();
        receiver->get_front_optical_properties()->set_ideal_absorption();
        receiver->get_back_optical_properties()->set_ideal_reflection();
        receiver->set_aperture(SolTrace::Data::make_aperture<SolTrace::Data::Rectangle>(rec_radius * 2.0, rec_height + 2. * rec_heat_shield_height));     // TODO: check if correct setup 
        receiver->set_surface(SolTrace::Data::make_surface<SolTrace::Data::Cylinder>(rec_radius));
        Vector3d v1 = { 0.0, 1.0, 0.0 }; // Pointing North
        Vector3d aim_point;
        vector_add(1.0, rec_origin, 1.0, v1, aim_point);
        receiver->set_reference_frame_geometry(rec_origin, aim_point, 0.0);
        receiver->set_name("Receiver");
        receiver->enable();
    }

    void create_heliostat_field(bool scatter_aimpoints) {
        // Define mirror optical properties
        OpticalProperties mirror;
        mirror.set_ideal_reflection();
        mirror.reflectivity = 0.9;
        mirror.slope_error = 2.0;       // mrad
        mirror.specularity_error = 0.0; // mrad
        mirror.error_distribution_type = DistributionType::GAUSSIAN;
        // Backside is absorbing
        OpticalProperties mirror_back;
        mirror_back.set_ideal_absorption();

        // Reading in field layout and aimpoints
        std::vector<double> x_coords;
        std::vector<double> y_coords;
        std::vector<double> aim_elevation;  // TODO: how to toggle between simple and advanced aiming?
        std::string field_file_path = std::string(PROJECT_DIR) + std::string("/round_robin_study/field_positions_aimpoints.csv");
        std::ifstream file(field_file_path);
        if (!file.is_open()) {
            std::cout << "Could not open field_positions_aimpoints.csv" << std::endl;
            return;
        }
        std::string line;
        int skip_lines = 1;
        int lines_skipped = 0;
        while (std::getline(file, line)) {
            if (lines_skipped < skip_lines) {
                lines_skipped++;
                continue;
            }
            std::stringstream ss(line);
            std::string item;
            std::vector<std::string> tokens;
            while (std::getline(ss, item, ',')) {
                tokens.push_back(item);
            }
            if (tokens.size() >= 4) {
                x_coords.push_back(std::stod(tokens[1]));
                y_coords.push_back(std::stod(tokens[2]));
                aim_elevation.push_back(std::stod(tokens[3]));
            }
        }

        // Generate heliostat field
        heliostat_field.clear();
        for (size_t i = 0; i < x_coords.size(); i++) {
            Vector3d heliostat_origin(x_coords[i], y_coords[i], 4.49);
            auto heliostat = SolTrace::Data::make_element<Heliostat>();
            heliostat->set_optics(mirror, mirror_back);
            heliostat->set_reference_frame_geometry(heliostat_origin, khat, 0.0);
            heliostat->set_aperture_size(10.38, 9.73);   // Width, Height
            heliostat->set_number_panels(1, 1);
            heliostat->set_gaps(0, 0);
            heliostat->set_focal_length(0.0);
            heliostat->set_canting(Heliostat::NONE, 0.0, 0.0);
            double distance = sqrt(pow(x_coords[i], 2) + pow(y_coords[i], 2));
            double z_aim_point = rec_origin[2] + (scatter_aimpoints ? aim_elevation[i] : 0.0);
            Vector3d aim_point = { rec_radius * (x_coords[i] / distance),
                                  rec_radius * (y_coords[i] / distance),
                                  z_aim_point };

            heliostat->set_target_position(aim_point);
            heliostat->create_geometry();
            heliostat->set_name("Heliostat_" + std::to_string(i + 1));
            heliostat->enable();
            heliostat_field.push_back(heliostat);
        }
    }

    void setup_simData() {
        // Set up sun
        Vector3d sun_pos;
        sun_position_vector_degrees(sun_pos, solar_azimuth, solar_elevation);
        {
            auto sun = SolTrace::Data::make_ray_source<Sun>();
            sun->set_position(sun_pos);
            sun->set_shape(SolTrace::Data::SunShape::PILLBOX, 0.0, 4.65, 0.0);
            simData.add_ray_source(sun);
        }

        // Set up stages
        stage_ptr st1 = SolTrace::Data::make_stage(1);
        st1->set_reference_frame_geometry(zero, khat, 0.0);
        for (const auto& heliostat : heliostat_field) {
            auto ret = st1->add_element(heliostat);
            EXPECT_TRUE(SolTrace::Data::Element::is_success(ret));
        }

        stage_ptr st2 = SolTrace::Data::make_stage(2);
        st2->set_reference_frame_geometry(zero, khat, 0.0);
        auto ret = st2->add_element(receiver);
        EXPECT_TRUE(SolTrace::Data::Element::is_success(ret));

        simData.add_stage(st1);
        simData.add_stage(st2);
    }

    void simulate() {
        RunnerStatus sts = runner.initialize();
        EXPECT_EQ(sts, RunnerStatus::SUCCESS);
        // Setup runs but is not complete
        sts = runner.setup_simulation(&simData);
        EXPECT_EQ(sts, RunnerStatus::SUCCESS);
        sts = runner.run_simulation();
        EXPECT_EQ(sts, RunnerStatus::SUCCESS);
        sts = runner.report_simulation(&result, 0);
        EXPECT_EQ(sts, RunnerStatus::SUCCESS);
    }

    void calculate_sun_size(double dni, bool print_sun_info) {
        const TSystem* sys = runner.get_system();
        const TSun* sun = &(sys->Sun);
        sun_width = (sun->MaxXSun - sun->MinXSun);
        sun_height = (sun->MaxYSun - sun->MinYSun);
        A_sun_box = sun_width * sun_height;
        power_per_ray = A_sun_box / sys->SunRayCount * dni;

        if (print_sun_info) {
            std::cout << "Power per ray: " << power_per_ray << std::endl;
            std::cout << "Sun box: " << sun_width << " x " << sun_height << std::endl;
            std::cout << "Sun ray count: " << sys->SunRayCount << std::endl;
        }
    }

    void calculate_ray_counts(bool print_info) {
        // Reset counts
        helio_hit_counts.resize(heliostat_field.size(), 0);
        reflect_counts.resize(heliostat_field.size(), 0);
        helio_absorb_counts.resize(heliostat_field.size(), 0);
        helio_block_counts.resize(heliostat_field.size(), 0);
        rec_absorb_count = 0;
        miss_count = 0;

        for (size_t i = 0; i < result.get_number_of_records(); i++) {
            const ray_record_ptr rr = result[i];
            bool first_helio_hit = false;
            for (size_t j = 0; j < rr->interactions.size(); j++) {
                auto hit_element = rr->get_element(j);
                SolTrace::Result::RayEvent rev = rr->get_event(j);

                if (rev == RayEvent::EXIT) miss_count++;
                if (rev == RayEvent::CREATE) first_helio_hit = true;
                if (hit_element < 0) continue;  // create or exit

                // Check receiver element
                if (hit_element == receiver->get_id()) {
                    if (rev == RayEvent::ABSORB) rec_absorb_count++;
                    continue;
                }

                // Check heliostat elements -> TODO: is there a way to directly map element IDs to heliostats?
                int helio_index = 0;
                bool finish_helio_check = false;
                for (const auto& heliostat : heliostat_field) {
                    for (auto iter = heliostat->get_const_iterator(); !heliostat->is_at_end(iter); ++iter) {
                        element_id facet_id = iter->second->get_id();
                        if (hit_element == facet_id) {
                            if (first_helio_hit) {
                                first_helio_hit = false;
                                helio_hit_counts[helio_index]++;
                                if (rev == RayEvent::REFLECT) reflect_counts[helio_index]++;
                                if (rev == RayEvent::ABSORB) helio_absorb_counts[helio_index]++;
                                finish_helio_check = true;
                                break;
                            }
                            else {
                                // Subsequent hits on a heliostat count as blocking
                                if (rev == RayEvent::ABSORB) helio_block_counts[helio_index]++;
                                finish_helio_check = true;
                                break;
                            }
                        }
                    }
                    helio_index++;
                    if (finish_helio_check) break;
                }



            }
        }

        tot_helio_hits = std::accumulate(helio_hit_counts.begin(), helio_hit_counts.end(), 0);
        tot_reflect_count = std::accumulate(reflect_counts.begin(), reflect_counts.end(), 0);
        tot_helio_absorb_count = std::accumulate(helio_absorb_counts.begin(), helio_absorb_counts.end(), 0);
        tot_helio_block_count = std::accumulate(helio_block_counts.begin(), helio_block_counts.end(), 0);

        if (print_info) {
            std::cout << "Heliostat Hit Count: " << tot_helio_hits << std::endl;
            std::cout << "Reflect Rays: " << tot_reflect_count << std::endl;
            std::cout << "Heliostat Absorbed Rays: " << tot_helio_absorb_count << std::endl;
            std::cout << "Heliostat Blocked Rays: " << tot_helio_block_count << std::endl;
            std::cout << "Receiver Absorbed Rays: " << rec_absorb_count << std::endl;
            std::cout << "Miss Rays: " << miss_count << std::endl;
        }
    }

    void reset_flux_map() {
        fluxGrid.clear();
        xValues.clear();
        yValues.clear();
        binszx = binszy = 0.0;
        PeakFlux = PeakFluxUncertainty = 0.0;
        AveFlux = AveFluxUncertainty = 0.0;
        MinFlux = SigmaFlux = Uniformity = 0.0;
        Centroid[0] = Centroid[1] = Centroid[2] = 0.0;
        zScale = 0.0;
        NumberOfRays = 0;
    }

    bool calculate_receiver_flux_map(
        int nbinsx, int nbinsy, bool is_cylinder, bool print_info) {
        reset_flux_map();

        double minx, maxx, miny, maxy;
        minx = maxx = miny = maxy = 0.0;
        Vector3d rec_origin = receiver->get_origin_global();

        // Autoscale
        if (true) {
            minx = miny = 1e199;
            maxx = maxy = -1e199;
            Vector3d local_position;
            Vector3d global_position;

            // automatically size the min/max x and y
            for (size_t i = 0; i < result.get_number_of_records(); i++) {
                const ray_record_ptr rr = result[i];
                for (size_t j = 0; j < rr->interactions.size(); j++) {
                    if (receiver->get_id() == rr->get_element(j)) {
                        rr->get_position(j, global_position);
                        receiver->convert_global_to_local(local_position, global_position);

                        double x = local_position[0];
                        double y = local_position[1];

                        if (x < minx) minx = x;
                        if (x > maxx) maxx = x;
                        if (y < miny) miny = y;
                        if (y > maxy) maxy = y;
                    }
                }
            }
        }

        if (nbinsx <= 1 || nbinsy <= 1
            || maxx <= minx || maxy <= miny)
        {
            return false;
        }

        double gridszx = maxx - minx;
        double gridszy = maxy - miny;

        if (is_cylinder) {
            minx = -PI * rec_radius;
            maxx = PI * rec_radius;
            gridszx = 2.0 * PI * rec_radius;
        }

        binszx = gridszx / (double)nbinsx;
        binszy = gridszy / (double)nbinsy;

        xValues.resize(nbinsx);
        yValues.resize(nbinsy);
        fluxGrid.resize(nbinsx, nbinsy);

        // Bin rays here -> BinRaysXY()
        double x, y, z;
        int GridIncrementX, GridIncrementY;

        size_t RayCount = 0;
        size_t NotBinned = 0;
        size_t npoints = 0;
        Centroid[0] = Centroid[1] = Centroid[2] = 0.0;
        fluxGrid.fill(0.0);

        for (size_t i = 0; i < result.get_number_of_records(); i++) {
            Vector3d local_position;
            Vector3d global_position;

            const ray_record_ptr rr = result[i];
            for (size_t j = 0; j < rr->interactions.size(); j++) {
                if (receiver->get_id() == rr->get_element(j)) {
                    if (rr->get_event(j) == RayEvent::ABSORB) {
                        rr->get_position(j, global_position);

                        receiver->convert_global_to_local(local_position, global_position);

                        x = local_position[0];
                        y = local_position[1];
                        z = local_position[2];

                        Centroid[0] += x;
                        Centroid[1] += y;
                        Centroid[2] += z;
                        npoints++;

                        if (is_cylinder) {
                            if (z <= rec_radius)
                                x = rec_radius * asin(x / rec_radius);
                            else if (z > rec_radius) {
                                if (x < 0) x = -(PI * rec_radius / 2.0 + rec_radius * acos(fabs(x) / rec_radius));
                                if (x >= 0) x = PI * rec_radius / 2.0 + rec_radius * acos(x / rec_radius);
                            }
                        }

                        GridIncrementX = -1; //initialize grid increment counters
                        GridIncrementY = -1;

                        //determine which bin the ray falls into
                        while ((minx + (GridIncrementX + 1) * binszx) < x)
                            GridIncrementX++;

                        while ((miny + (GridIncrementY + 1) * binszy) < y)
                            GridIncrementY++;

                        if (GridIncrementX >= 0 && GridIncrementX < (int)fluxGrid.nrows()
                            && GridIncrementY >= 0 && GridIncrementY < (int)fluxGrid.ncols())
                        {
                            fluxGrid.at(GridIncrementX, GridIncrementY) += 1;//if ray falls inside a bin, increment count for that bin
                            RayCount++;  //increment ray intersection counter
                        }
                        else
                        {
                            NotBinned++;
                            //	qDebug("Not binned: [%d %d],  x=%lg, y=%lg", GridIncrementX, GridIncrementY, x, y);
                        }
                    }
                }
            }
        }

        //calculate midpoints of bins
        for (size_t i = 0; i < xValues.size(); i++)
            xValues[i] = minx + binszx / 2.0 + i * binszx;

        for (size_t i = 0; i < yValues.size(); i++)
            yValues[yValues.size() - i - 1] = miny + binszy / 2.0 + i * binszy;

        if (npoints > 0)
        {
            Centroid[0] /= npoints;
            Centroid[1] /= npoints;
            Centroid[2] /= npoints;
        }

        double SumFlux, SumFlux2;
        PeakFlux = SumFlux = SumFlux2 = 0;
        MinFlux = 1e99;

        int NRaysInMinFluxBin = -1, NRaysInPeakFluxBin = -1;

        zScale = power_per_ray / (binszx * binszy);
        for (size_t r = 0; r < fluxGrid.nrows(); r++)
        {
            for (size_t c = 0; c < fluxGrid.ncols(); c++)
            {
                double z = fluxGrid.at(r, c) * zScale;
                SumFlux += z;
                SumFlux2 += z * z;

                if (z > PeakFlux) {
                    PeakFlux = z;
                    NRaysInPeakFluxBin = (int)fluxGrid.at(r, c);
                }

                if (z < MinFlux) {
                    MinFlux = z;
                    NRaysInMinFluxBin = (int)fluxGrid.at(r, c);
                }
            }
        }

        AveFlux = SumFlux / (nbinsx * nbinsy);
        SigmaFlux = sqrt((nbinsx * nbinsy * SumFlux2 - SumFlux * SumFlux) / (nbinsx * nbinsy * nbinsx * nbinsy));
        Uniformity = SigmaFlux / AveFlux;
        PeakFluxUncertainty = 100 / sqrt((double)NRaysInPeakFluxBin);
        AveFluxUncertainty = 100 / sqrt((double)result.get_number_of_records());     
        // TODO: Should the be number of rays hitting the surface, not total rays traced?

        if (print_info) {
            std::cout << "Receiver auto bounds:" << std::endl;
            std::cout << "X min: " << minx << " X max: " << maxx << std::endl;
            std::cout << "Y min: " << miny << " Y max: " << maxy << std::endl;

            std::cout << "Peak flux: " << PeakFlux << std::endl;
            std::cout << "Peak flux uncertainty: +/- " << PeakFluxUncertainty << " %" << std::endl;
            std::cout << "Min flux: " << MinFlux << std::endl;
            std::cout << "Sigma flux: " << SigmaFlux << std::endl;
            std::cout << "Avg. flux: " << AveFlux << std::endl;
            std::cout << "Avg. flux uncertainty: +/- " << AveFluxUncertainty << " %" << std::endl;
            std::cout << "Uniformity: " << Uniformity << std::endl;
            std::cout << "Centroid: (" << Centroid[0] << ", " << Centroid[1] << ", " << Centroid[2] << ")" << std::endl;
        }

        return true;

    }

    void TearDown() override {
        // Code here will be called immediately after each test (right
        // before the destructor).
    }

};


TEST_F(HeliostatFieldSimulation, basic_test)
{
    bool high_accuracy = false;
    bool print_info = true;

    SimulationParameters& params = simData.get_simulation_parameters();
    if (high_accuracy) {
        params.number_of_rays = 20.e6;
        params.max_number_of_rays = params.number_of_rays * 100;
    }

    create_heliostat_field(false);  // centerline aimpoints
    for (const auto& heliostat : heliostat_field) {
        heliostat->update_geometry(solar_azimuth, solar_elevation);
    }

    setup_simData();
    simulate();

    double dni = 1000.0;
    calculate_sun_size(dni, print_info);
    EXPECT_NEAR(sun_width, 2788.59, 1.e-2);
    EXPECT_NEAR(sun_height, 2457.81, 1.e-2);

    calculate_ray_counts(print_info);
    double total_power = rec_absorb_count * power_per_ray;

    double tol = high_accuracy ? 5.e-4 : 5.e-3;
    //double expected_power = 99967.3;    // No sunshape or surfaces errors
    //double expected_power = 93219.6;    // Sunshape only
    //double expected_power = 88033.6;    // Surface errors only
    double expected_power = 84984.8;      // Sunshape + surface errors

    uint_fast64_t NRAYS = params.number_of_rays;
    EXPECT_EQ(tot_helio_hits, NRAYS);
    EXPECT_EQ(tot_helio_absorb_count + tot_reflect_count, tot_helio_hits);
    EXPECT_EQ(rec_absorb_count + miss_count + tot_helio_block_count, tot_reflect_count);

    EXPECT_NEAR((double)tot_reflect_count / (double)tot_helio_hits, 0.9, tol);
    //EXPECT_NEAR(total_power, expected_power, tol * expected_power);

    //if (high_accuracy) {
    //    calculate_receiver_flux_map(100, 150, true, print_info);
    //    double expected_peak = 853.65157013;
    //    EXPECT_NEAR(PeakFlux, expected_peak, 2.0);
    //    // TODO: Create a flux map and compare to expected distribution
    //}
    //else {
    //    calculate_receiver_flux_map(30, 30, true, print_info);
    //    double expected_peak = 906.458;
    //    EXPECT_NEAR(PeakFlux, expected_peak, 30.0);
    // }
}
