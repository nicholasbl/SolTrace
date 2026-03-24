#pragma once

#include "database/database_models.h"

namespace db {

/// A model providing all element entities that do not currently have a parent.
class RootElementsModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT

    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();
    void ident_changed(entt::entity);

public:
    explicit RootElementsModel(QObject* parent = nullptr);
    ~RootElementsModel() override = default;

    void reset(Database* database);
};

} // namespace db
