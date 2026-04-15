#include "simulationresult.h"
#include "analysis/ray_volume_raster.h"

#include "database.h"

#include <entt/entity/entity.hpp>

#include <QDebug>

namespace db {

static RayEventType convert(SolTrace::Result::RayEvent e) {
    switch (e) {
    case SolTrace::Result::RayEvent::CREATE: return RayEventType::CREATE;
    case SolTrace::Result::RayEvent::ABSORB: return RayEventType::ABSORB;
    case SolTrace::Result::RayEvent::REFLECT: return RayEventType::REFLECT;
    case SolTrace::Result::RayEvent::TRANSMIT: return RayEventType::TRANSMIT;
    case SolTrace::Result::RayEvent::VIRTUAL: return RayEventType::VIRTUAL;
    case SolTrace::Result::RayEvent::EXIT: return RayEventType::EXIT;
    case SolTrace::Result::RayEvent::UNKNOWN: return RayEventType::UNKNOWN;
    }
    return RayEventType::UNKNOWN;
}

static RayRecord extract(uint64_t                          id,
                         SimulationResultConversion const& opts,
                         SolTrace::Result::RayRecord&      rec) {
    std::vector<RayEvent> pack;

    pack.reserve(rec.interactions.size());

    auto entity_getter = [&](SolTrace::Data::element_id id) -> entt::entity {
        auto iter = opts.map.find(id);
        if (iter != opts.map.end()) { return iter->second; }
        return entt::null;
    };

    for (auto& c : rec.interactions) {
        pack.push_back({
            .location  = c->location,
            .direction = c->direction,
            .entity    = entity_getter(c->element),
            .event     = convert(c->event),
        });
    }

    return RayRecord {
        .id     = id,
        .events = std::move(pack),
    };
}

SimulationResult::SimulationResult() = default;

SimulationResult::~SimulationResult() = default;

std::unique_ptr<SimulationResult>
SimulationResult::convert(SimulationResultConversion const& opts) {
    qDebug() << Q_FUNC_INFO << "Converting results...";

    auto ret = std::make_unique<SimulationResult>();

    ret->records.reserve(opts.result.get_number_of_records());

    uint64_t id = 0;

    for (auto iter = opts.result.get_ray_record_iteratior();
         !opts.result.is_at_end(iter);
         ++iter) {

        ret->records.emplace_back(extract(id, opts, **iter));

        id++;
    }

    for (size_t ray_i = 0; ray_i < ret->records.size(); ray_i++) {
        auto const& events = ret->records[ray_i].events;
        for (auto const& event : events) {
            if (event.entity != entt::null) {
                ret->entity_to_ray_ids[event.entity].push_back(ray_i);
            }
        }
    }

    for (auto& [k, v] : ret->entity_to_ray_ids) {
        std::sort(v.begin(), v.end());
        auto last = std::unique(v.begin(), v.end());
        v.erase(last, v.end());
    }

    qDebug() << Q_FUNC_INFO << "Converted" << ret->records.size() << "rays";

    {
        constexpr float maxFloat = std::numeric_limits<float>::max();

        glm::dvec3 bounds_min(maxFloat);
        glm::dvec3 bounds_max(-maxFloat);


        for (auto const& r : ret->records) {
            for (auto const& inter : r.events) {
                bounds_min = glm::min(inter.location, bounds_min);
                bounds_max = glm::max(inter.location, bounds_max);
            }
        }

        if (glm::all(glm::lessThan(bounds_min, bounds_max))) {
            ret->bounds_max = bounds_max;
            ret->bounds_min = bounds_min;
        }
    }

    {
        // TODO: Move out of here.

        qDebug() << Q_FUNC_INFO << "Computing ray grid";

        auto const extent = ret->bounds_max - ret->bounds_min;

        // Compute volume
        auto grid_size = ceil(glm::normalize(extent) * 256.0);

        auto const grid_size_f = glm::vec3(grid_size);
        auto to_grid_coords    = [&](glm::dvec3 const& p) {
            glm::vec3 rel(0.0f);

            if (extent.x > 0.0) {
                rel.x = static_cast<float>((p.x - ret->bounds_min.x) /
                                           extent.x * grid_size_f.x);
            }
            if (extent.y > 0.0) {
                rel.y = static_cast<float>((p.y - ret->bounds_min.y) /
                                           extent.y * grid_size_f.y);
            }
            if (extent.z > 0.0) {
                rel.z = static_cast<float>((p.z - ret->bounds_min.z) /
                                           extent.z * grid_size_f.z);
            }

            return rel;
        };

        // qDebug() << grid_size[0] << grid_size[1] << grid_size[2];

        // auto cell_size =
        //     (ptr->bounds_max.x() - ptr->bounds_min.x()) / grid_size[0];

        analysis::Grid3D<float> grid(grid_size[0], grid_size[1], grid_size[2]);

        for (auto const& ray : ret->records) {

            if (ray.events.empty()) continue;

            auto last_p = to_grid_coords(ray.events[0].location);

            for (auto i = 1; i < ray.events.size(); ++i) {
                auto const& this_interaction   = ray.events[i];
                auto        this_interaction_p =
                    to_grid_coords(this_interaction.location);

                if (last_p == this_interaction_p) continue;

                analysis::raster_segment(grid, last_p, this_interaction_p);

                last_p = this_interaction_p;
            }
        }

        // normalize
        float largest = 0.0;
        for (auto x : grid) {
            largest = std::max(x, largest);
        }

        if (largest != 0.0) {
            for (auto& x : grid) {
                x /= largest;
            }
        }

        qDebug() << Q_FUNC_INFO << "Grid largest" << largest;

        ret->ray_volume = std::move(grid);
    }

    return ret;
}

} // namespace db
