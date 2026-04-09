#include "flux_map.h"

#include "analysis/grid2d.h"
#include <QFutureWatcher>
#include <QtConcurrent/qtconcurrentrun.h>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/gtx/intersect.hpp>

#include <QImage>
#include <QPainter>

namespace analysis {

static QPointF uv_to_pixel(glm::vec2 uv, QSize const& size) {
    auto width  = std::max(0, size.width() - 1);
    auto height = std::max(0, size.height() - 1);

    return QPointF(uv.x * width, uv.y * height);
}

inline std::optional<glm::vec3> uv_projection(db::Vertex const& v1,
                                              db::Vertex const& v2,
                                              db::Vertex const& v3,
                                              glm::vec3         p,
                                              glm::vec3         n) {
    float     distance;
    glm::vec2 barycenter;

    bool ok =
        glm::intersectRayTriangle(p,
                                  n, // we want to point TOWARD the surface
                                  v1.position,
                                  v2.position,
                                  v3.position,
                                  barycenter,
                                  distance);

    if (ok) {
        float u = barycenter.x;
        float v = barycenter.y;
        float w = 1.0f - u - v;

        auto uv = v1.uv * w + v2.uv * u + v3.uv * v;

        return glm::vec3(uv, distance);
    }

    return {};
}

std::optional<glm::vec2> project_point_to_uv(db::Mesh const& m, glm::vec3 p) {

    // this is probably stupid, but for now we can do this

    constexpr float INIT = 100000000;

    glm::vec3 closest(0, 0, INIT);

    for (size_t i = 0; i < m.index.size(); i += 3) {
        auto const& v1 = m.vertex[m.index[i + 0]];
        auto const& v2 = m.vertex[m.index[i + 1]];
        auto const& v3 = m.vertex[m.index[i + 2]];

        // generate a normal here. dont use what is given, as we want the
        // triangle surface normal

        auto normal = glm::normalize(
            glm::cross(v1.position - v2.position, v1.position - v3.position));

        auto test_a = uv_projection(v1, v2, v3, p, normal);

        // check rear. does distance go negative? can we remove this check?
        if (!test_a.has_value()) {
            test_a = uv_projection(v1, v2, v3, p, -normal);
        }

        if (!test_a.has_value()) continue;

        // ok, we have a uv coord. see if its closest

        if (test_a->z < closest.z) { closest = *test_a; }
    }

    if (closest.z < INIT) {
        return glm::clamp(glm::vec2(closest), glm::vec2(0), glm::vec2(1));
    }

    return {};
}

static void raster_mesh_overlay(QPainter&        painter,
                                db::Mesh const&  mesh,
                                QSize const&     image_size,
                                QColor const&    line_color) {
    if (!line_color.isValid()) { return; }

    painter.save();
    painter.setPen(QPen(line_color, 1.0));

    for (size_t i = 0; i + 2 < mesh.index.size(); i += 3) {
        auto const& v1 = mesh.vertex[mesh.index[i + 0]];
        auto const& v2 = mesh.vertex[mesh.index[i + 1]];
        auto const& v3 = mesh.vertex[mesh.index[i + 2]];

        QPolygonF triangle;
        triangle << uv_to_pixel(v1.uv, image_size) << uv_to_pixel(v2.uv, image_size)
                 << uv_to_pixel(v3.uv, image_size);

        painter.drawPolygon(triangle);
    }

    painter.restore();
}


void execute_map_generation_for(
    QPromise<QImage>&                           promise,
    FluxMapBakeOptions                          opts,
    entt::entity                                entity,
    std::shared_ptr<db::SimulationResult const> results,
    db::Mesh                                    mesh) {

    promise.setProgressRange(0, 100);

    // CAUTION. we are NOT thread safe here if someone wants to change the
    // database. The results MUST also be const.


    if (!glm::all(glm::lessThan(glm::uvec2(0), opts.bin_counts))) { return; }

    auto iter = results->entity_to_ray_ids.find(entity);

    if (iter == results->entity_to_ray_ids.end()) { return; }

    // ok. now get all points that 'intersect' with this thing.

    // create a bin system.

    auto bins = Grid2D<float>(opts.bin_counts.x, opts.bin_counts.y);

    auto bin_bounds = glm::vec2(opts.bin_counts) - glm::vec2(1);

    auto bin_index = [=](glm::vec2 uv) -> glm::ivec2 {
        return glm::floor(uv * bin_bounds);
    };

    float total = 0.0;

    constexpr int PROGRESS_SETUP     = 10;
    constexpr int PROGRESS_RAY_CHECK = 50;
    constexpr int PROGRESS_RASTER    = 100;

    promise.setProgressValue(PROGRESS_SETUP);

    // something like this should be in a toolbox somewhere
    auto report_progress =
        [&](int item, int max_item, int prog_low, int prog_high) {
            auto a = (float)item / (float)max_item;
            int  p = glm::mix((float)prog_low, (float)prog_high, a);
            promise.setProgressValue(p);
        };

    // for every ray that has hit this

    size_t ray_count = 0;

    for (auto ray_index : iter->second) {

        ray_count++;

        if (ray_count % 500 == 0) {
            report_progress(ray_count,
                            iter->second.size(),
                            PROGRESS_SETUP,
                            PROGRESS_RAY_CHECK);
        }

        if (promise.isCanceled()) { return; }

        // scan all events and get intersection points
        for (auto const& interaction : results->records.at(ray_index).events) {

            // whatever it was, is it us?
            if (interaction.entity != entity) { continue; }

            // we have a viable point.

            auto p = interaction.location;

            // we now need to project this point to the surface

            auto uv = project_point_to_uv(mesh, p);

            // no uv?
            if (!uv) { continue; }

            auto bin_xy = bin_index(*uv);

            bins(bin_xy.x, bin_xy.y) += 1.0;
            total += 1.0;
        }
    }

    // normalize all bins?

    if (total > 0.0) {
        for (auto& f : bins) {
            f /= total;
        }
    }

    // bins ready
    if (promise.isCanceled()) { return; }

    promise.setProgressValue(PROGRESS_RAY_CHECK);

    // now raster it

    // This format is supposedly the most optimized.
    auto img = QImage(
        opts.image_resolution.x, opts.image_resolution.y, QImage::Format_RGB32);

    // we need to see if this is the right coordinate system so we dont flip or
    // something.
    auto painter = QPainter(&img);

    // raster bins

    glm::uvec2 bin_size       = opts.image_resolution / opts.bin_counts;
    auto       color_map_size = opts.color_map.size();

    for (int x = 0; x < opts.bin_counts.x; x++) {

        report_progress(
            x + 1, opts.bin_counts.x, PROGRESS_RAY_CHECK, PROGRESS_RASTER);

        for (int y = 0; y < opts.bin_counts.y; y++) {
            auto bin_value = bins(x, y);

            auto sample = QPoint(bin_value * (color_map_size.width() - 1),
                                 color_map_size.height() / 2);

            auto color = opts.color_map.pixelColor(sample);

            auto corner = bin_size * glm::uvec2(x, y);

            auto rect = QRect(QPoint(corner.x, corner.y),
                              QSize(bin_size.x, bin_size.y));

            painter.fillRect(rect, color);
        }

        if (promise.isCanceled()) { return; }
    }

    raster_mesh_overlay(painter, mesh, img.size(), opts.grid_line_color);

    promise.emplaceResult(img);

    return;
}

FluxMapComputer::FluxMapComputer(QObject* parent) : QObject(parent) { }

FluxMapComputer::~FluxMapComputer() = default;

void FluxMapComputer::set_results(
    std::shared_ptr<db::SimulationResult const> p) {
    m_database = p;

    cancel_all();
}

bool FluxMapComputer::start_generate_for(db::Entity         e,
                                         db::Mesh           mesh,
                                         FluxMapBakeOptions options) {

    if (!m_database) { return false; }

    if (options.color_map.isNull()) {
        options.color_map = QImage(":/assets/images/b_to_r_wide.png");

        if (options.color_map.isNull()) {
            qCritical() << "Missing colormaps!";
            return false;
        }
    }


    auto future = QtConcurrent::run(
        execute_map_generation_for, options, e, m_database, mesh);

    // TODO: future; recompute bins once

    using FW = QFutureWatcher<QImage>;

    auto watcher = new FW(this);

    watcher->setFuture(future);

    connect(watcher, &FW::finished, watcher, &FW::deleteLater);

    connect(watcher, &FW::resultReadyAt, this, [this, e](int index) {
        auto* from = dynamic_cast<FW*>(sender());

        if (from) { emit image_ready(e, from->result()); }
    });

    connect(watcher, &FW::progressValueChanged, this, [this, e](int value) {
        emit image_progress(e, value);
    });

    // TODO: make sure this also deletes the watcher?
    connect(this, &FluxMapComputer::cancel_all, watcher, &FW::cancel);

    connect(this,
            &FluxMapComputer::cancel_specific,
            watcher,
            [e, watcher](db::Entity item) {
                if (item == e) { watcher->cancel(); }
            });

    return true;
}

} // namespace analysis
