#include "worldgeometrymodel.h"
#include "utilities/math_utility.h"

namespace db {

void InstancedElements::on_group_change(entt::entity group) {
    m_instance_data.clear();
    m_member_cache.clear();
    m_rev_cache.clear();

    markDirty();

    if (group != m_target_group) return;
    if (!m_database) return;

    auto* ptr = m_database->group_root.get(group);


    for (auto member : std::as_const(ptr->members)) {

        auto global = m_database->global_transform(member);

        auto entry = calculateTableEntryFromQuaternion(convert(global.position),
                                                       QVector3D(1, 1, 1),
                                                       convert(global.rotation),
                                                       Qt::white);

        m_instance_data.append(reinterpret_cast<const char*>(&entry),
                               sizeof(entry));

        m_rev_cache[member] = m_member_cache.size();
        m_member_cache.push_back(member);
    }

    qDebug() << Q_FUNC_INFO << "group" << entt::to_integral(group) << "->"
             << m_member_cache.size();
}

InstancedElements::InstancedElements(Database*       db,
                                     entt::entity    group,
                                     QQuick3DObject* parent)
    : QQuick3DInstancing(parent), m_database(db), m_target_group(group) {
    if (!db) return;

    connect(db->group_membership.self(),
            &ComponentAPIBase::changed,
            this,
            &InstancedElements::on_group_change);

    connect(db->group_membership.self(),
            &ComponentAPIBase::removed,
            this,
            &InstancedElements::on_group_change);

    on_group_change(group);
}

QByteArray InstancedElements::getInstanceBuffer(int* instanceCount) {

    if (instanceCount) { *instanceCount = m_member_cache.size(); }

    return m_instance_data;
}

// =============================================================================

static VisibleGroup vis_assets_for_entity(Database& db, entt::entity e) {
    auto vg = VisibleGroup {
        .group_entity    = e,
        .group_instances = std::make_shared<InstancedElements>(&db, e),
        .group_geometry  = std::make_shared<SurfaceGeometry>(),
    };

    auto grp = db.group_parameters.get(e);

    if (!grp) {
        qWarning() << "Unable to get group parameters";
        return vg;
    }

    vg.group_geometry->set(&db, e);

    return vg;
}

QVector<VisibleGroup> WorldGeometryModel::rebuild_lists() {
    qDebug() << Q_FUNC_INFO;
    QVector<VisibleGroup> new_recs;
    m_reverse.clear();

    if (!m_host) return {};

    auto view = m_host->as_registry().view<RenderGroupComponent>();

    for (auto const& [e, group] : view.each()) {
        new_recs.push_back(vis_assets_for_entity(*m_host, e));
    }

    for (size_t i = 0; i < new_recs.size(); i++) {
        m_reverse[new_recs[i].group_entity] = i;
    }

    qDebug() << Q_FUNC_INFO << "Done" << new_recs.size();

    return new_recs;
}

void WorldGeometryModel::recompute() {
    auto r = rebuild_lists();

    this->store_reset(r);
}

void WorldGeometryModel::group_changed(entt::entity e) {
    if (!m_host) return;

    auto iter = m_reverse.find(e);

    if (iter == m_reverse.end()) { return recompute(); }

    this->store_push_update(iter->second, vis_assets_for_entity(*m_host, e));
}
void WorldGeometryModel::group_removed(entt::entity e) {
    recompute();
}

WorldGeometryModel::WorldGeometryModel(QObject* parent)
    : StructModelAdapter(parent) { }

void WorldGeometryModel::reset(Database* database) {
    m_host = database;
    recompute();

    if (!database) { return; }

    connect(database->group_root.self(),
            &ComponentAPIBase::changed,
            this,
            &WorldGeometryModel::group_changed);

    connect(database->group_root.self(),
            &ComponentAPIBase::removed,
            this,
            &WorldGeometryModel::group_removed);

    connect(database->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &WorldGeometryModel::group_changed);
}

} // namespace db
