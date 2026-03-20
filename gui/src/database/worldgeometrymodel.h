#pragma once

#include <QtQuick3D/qquick3dinstancing.h>

#include "database.h"
#include "database/database_models.h"
#include "database/geometryeditor.h"

namespace db {


// TODO: push instance generation to a thread
class InstancedElements : public QQuick3DInstancing {
    Q_OBJECT

    QByteArray                               m_instance_data;
    std::vector<entt::entity>                m_member_cache;
    std::unordered_map<entt::entity, size_t> m_rev_cache;

    QByteArray getInstanceBuffer(int* instanceCount) override;

    QPointer<Database> m_database;
    entt::entity       m_target_group = entt::null;

private slots:
    void on_geometry_group_change(entt::entity);

public:
    explicit InstancedElements(Database*       db,
                               entt::entity    group,
                               QQuick3DObject* parent = nullptr);
};

struct VisibleGroup {
    entt::entity                       geometry_group_entity;
    std::shared_ptr<InstancedElements> group_instances;
    std::shared_ptr<SurfaceGeometry>   group_geometry;

    RECORD_META(VisibleGroup,
                SM_EXPOSE_RO(group_instances),
                SM_EXPOSE_RO(group_geometry), );
};


class WorldGeometryModel : public StructModelAdapter<VisibleGroup> {
    Q_OBJECT

    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<VisibleGroup> rebuild_lists();

private slots:
    void recompute();

    void group_changed(entt::entity);
    void group_removed(entt::entity);

public:
    explicit WorldGeometryModel(QObject* parent = nullptr);
    virtual ~WorldGeometryModel() = default;

    void reset(Database* database);
};

} // namespace db
