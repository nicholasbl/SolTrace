#include "job_run_common.h"
#include "utilities/math_utility.h"

#include "analysis/ray_volume_raster.h"

#define SECTION(VALUE, TEXT)                                                   \
    promise.setProgressValueAndText(VALUE, TEXT);                              \
    promise.suspendIfRequested();                                              \
    if (promise.isCanceled()) {                                                \
        promise.emplaceResult("Cancelled at " TEXT);                           \
        return;                                                                \
    }

void construct_result(QPromise<SimResult>& promise,
                      ResultPtr            ptr,
                      SimDataPtr           data) {

    // now compute lookup tables
    {
        SECTION(90, "Building lookup tables");

        auto& st_result = ptr->result;

        auto num_records = st_result.get_number_of_records();

        if (num_records > 0) {
            ptr->element_ids_to_ray_ids.reserve(num_records);

            uint64_t iter_count = 0;

            for (auto iter = st_result.get_ray_record_iteratior();
                 !st_result.is_at_end(iter);
                 ++iter) {

                for (auto const& interaction : (*iter)->interactions) {
                    auto element = interaction->element;
                    // It looks like invalid element IDs are negative
                    // Any zero+ element id could be used
                    if (element >= 0) {
                        ptr->element_ids_to_ray_ids[interaction->element]
                            .push_back((*iter)->id);
                    }
                }

                iter_count++;
            }
        }
    }

    {
        SECTION(91, "Building bounding box");

        constexpr float maxFloat = std::numeric_limits<float>::max();

        QVector3D bounds_min(maxFloat, maxFloat, maxFloat);
        QVector3D bounds_max(-maxFloat, -maxFloat, -maxFloat);

        for (auto iter = ptr->result.get_ray_record_iteratior();
             !ptr->result.is_at_end(iter);
             ++iter) {

            auto& this_ray = (**iter);

            for (auto const& interaction : this_ray.interactions) {

                auto p = convert(interaction->location);

                bounds_min.setX(std::min(bounds_min.x(), p.x()));
                bounds_min.setY(std::min(bounds_min.y(), p.y()));
                bounds_min.setZ(std::min(bounds_min.z(), p.z()));

                bounds_max.setX(std::max(bounds_max.x(), p.x()));
                bounds_max.setY(std::max(bounds_max.y(), p.y()));
                bounds_max.setZ(std::max(bounds_max.z(), p.z()));
            }
        }

        ptr->bounds_min = bounds_min;
        ptr->bounds_max = bounds_max;
    }

    {
        SECTION(92, "Building ray volume");

        using namespace analysis;

        // Compute volume
        auto grid_size =
            ceil((ptr->bounds_max - ptr->bounds_min).normalized() * 128.0);

        // qDebug() << grid_size[0] << grid_size[1] << grid_size[2];

        // auto cell_size =
        //     (ptr->bounds_max.x() - ptr->bounds_min.x()) / grid_size[0];

        Grid3D<float> grid(grid_size[0], grid_size[1], grid_size[2]);

        for (auto iter = ptr->result.get_ray_record_iteratior();
             !ptr->result.is_at_end(iter);
             ++iter) {

            auto& this_ray = (**iter);

            if (this_ray.interactions.empty()) continue;

            QVector3D last_p = convert(this_ray.interactions[0]->location);

            for (auto i = 1; i < this_ray.interactions.size(); ++i) {
                auto const& this_interaction = this_ray.interactions[i];
                auto this_interaction_p = convert(this_interaction->location);

                if (last_p == this_interaction_p) continue;

                raster_segment(grid, last_p, this_interaction_p);

                last_p = this_interaction_p;
            }
        }

        // normalize
        float largest = 0.0;
        for (auto x : grid) {
            largest = std::max(x, largest);
        }

        for (auto& x : grid) {
            x /= largest;
        }

        ptr->ray_volume_grid = std::move(grid);
    }


    SECTION(100, "Done");

    promise.emplaceResult(std::move(ptr));
}
