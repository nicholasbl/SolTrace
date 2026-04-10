#include "fluxmapworldmodel.h"

#include "analysis/flux_map.h"
#include "database/surface.h"

namespace db {

static QString image_name(Entity e) {
    return QString::number(entt::to_integral((entt::entity)e));
}

PendingFluxMapModel::PendingFluxMapModel(QObject* parent)
    : StructModelAdapter(parent) {

    m_compute = new analysis::FluxMapComputer(this);

    connect(m_compute,
            &analysis::FluxMapComputer::image_progress,
            this,
            &PendingFluxMapModel::on_progress);

    connect(m_compute,
            &analysis::FluxMapComputer::image_ready,
            this,
            &PendingFluxMapModel::on_ready);
}


void PendingFluxMapModel::reset(db::SimulationResult* res) {
    emit cleared();

    m_compute->set_results(res);

    on_changed();

    if (res->database) {
        m_host = res->database;
        for (auto const& [e, c] :
             res->database->as_registry().view<HasFluxMapComponent>().each()) {
            emit ready(e, c.map_info, res->database);
        }
    } else {
        m_host = nullptr;
    }
}

void PendingFluxMapModel::on_changed() {
    store_remove_all();
}

void PendingFluxMapModel::on_ready(Entity e, analysis::BakedFluxMapPtr image) {
    if (!m_host) return;

    m_host->as_registry().emplace_or_replace<HasFluxMapComponent>(
        e, HasFluxMapComponent { .map_info = image });

    store_remove_by_predicate([e](auto& record) { return record.entity == e; });

    emit ready(e, image, m_host);
}

void PendingFluxMapModel::on_progress(Entity e, int progress) {
    // find and update

    for (int i = 0; i < this->rowCount(); i++) {
        auto* p = get_at(i);

        if (!p) continue;

        if (p->entity == e) {
            auto copy = *p;

            copy.progress = progress;

            store_push_update(i, copy);

            return;
        }
    }
}

static std::optional<Mesh>
find_mesh_for(db::SurfaceGenerationOptions surface_options,
              Database*                    database,
              Entity                       entity) {
    auto* surface_membership = database->geometry_group_membership.get(entity);

    if (!surface_membership) { return {}; }

    auto* surface =
        database->geometry_parameters.get(surface_membership->group);

    if (!surface) { return {}; }

    return db::generate_surface(surface->surface, surface->aperture);
}

bool PendingFluxMapModel::start_generate_for(Entity entity) {
    if (!m_host) return false;

    auto mesh_res = std::max(1, mesh_resolution_multiply());

    db::SurfaceGenerationOptions surface_options;
    surface_options.height_field_resolution *= mesh_res;
    surface_options.radial_subdivisions *= mesh_res;
    surface_options.perimeter_subdivisions *= mesh_res;
    surface_options.cylinder_angular_subdivisions *= mesh_res;
    surface_options.cylinder_length_subdivisions *= mesh_res;

    auto mesh = find_mesh_for(surface_options, m_host, entity);

    if (!mesh) return false;

    store_remove_by_predicate([this, entity](auto const& item) {
        if (item.entity == entity) { this->cancel_for(entity); }
        return item.entity == entity;
    });

    auto opts = analysis::FluxMapBakeOptions {
        .image_resolution = { this->image_resolution().width(),
                              this->image_resolution().height(), },
        .grid_line_color =
            this->show_mesh_grid() ? this->mesh_line_color() : QColor(),
        .color_map = QImage(color_map()),
    };

    if (!m_compute->start_generate_for(entity, *mesh, opts)) { return false; }

    store_push_append(FluxMappedPendingItem {
        .entity   = entity,
        .progress = 0,
    });

    return true;
}

void PendingFluxMapModel::cancel_for(Entity entity) {
    m_compute->cancel_specific(entity);
}


FluxMapProvider* PendingFluxMapModel::make_new_provider() {
    auto ret = new FluxMapProvider();

    connect(this, &PendingFluxMapModel::ready, ret, &FluxMapProvider::on_ready);
    connect(this, &PendingFluxMapModel::cleared, ret, &FluxMapProvider::clear);

    return ret;
}


// ============================================================================

void FluxMapWorldModel::on_reset() {
    store_remove_all();
}

FluxMapWorldModel::FluxMapWorldModel(QObject* parent)
    : StructModelAdapter(parent) { }


void FluxMapWorldModel::on_ready(Entity                    e,
                                 analysis::BakedFluxMapPtr img,
                                 Database*                 db) {
    // make sure we dont have this already. Not the cleanest, but we shouldn't
    // have that many maps here

    if (!db) return;

    store_remove_by_predicate(
        [e](auto const& item) { return item.entity == e; });

    auto geom = std::make_shared<SurfaceGeometry>();

    geom->set(db, e);

    store_push_append(FluxMappedItem {
        .entity         = e,
        .texture_source = "image://fluxmap/" + image_name(e),
        .geometry       = geom,
    });
}

// ============================================================================

FluxMapProvider::FluxMapProvider()
    : QQuickImageProvider(QQuickImageProvider::ImageType::Image) { }

QImage FluxMapProvider::requestImage(QString const& id,
                                     QSize*         size,
                                     QSize const&   requestedSize) {
    QImage ret;

    {
        m_lock.lock();
        auto iter = m_store.find(id);
        if (iter != m_store.end()) { ret = iter.value()->image; }
        m_lock.unlock();
    }

    if (size) *size = ret.size();

    return ret;
}

void FluxMapProvider::on_ready(Entity                    k,
                               analysis::BakedFluxMapPtr v,
                               Database*) {
    m_lock.lock();
    m_store[image_name(k)] = v;
    m_lock.unlock();
}

void FluxMapProvider::clear() {
    m_lock.lock();
    m_store.clear();
    m_lock.unlock();
}

} // namespace db
