#include "rootelementsmodel.h"

#include "database/components.h"
#include "database/database_notification.h"

namespace db {

QVector<EntityNamePair> RootElementsModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    if (!m_host) return {};

    auto view =
        m_host->as_registry().view<ElementComponent>(entt::exclude<ChildOfComponent>);

    for (auto [entity] : view.each()) {
        new_recs.push_back(EntityNamePair::record_for_entity(*m_host, entity));
    }

    for (qsizetype i = 0; i < new_recs.size(); ++i) {
        m_reverse[new_recs[i].entity] = static_cast<int>(i);
    }

    return new_recs;
}

void RootElementsModel::recompute() {
    store_reset(rebuild_lists());
}

void RootElementsModel::ident_changed(entt::entity entity) {
    if (!m_host) return;

    if (auto iter = m_reverse.find(entity); iter != m_reverse.end()) {
        store_push_update(iter->second,
                          EntityNamePair::record_for_entity(*m_host, entity));
    }
}

RootElementsModel::RootElementsModel(QObject* parent)
    : StructModelAdapter(parent) { }

void RootElementsModel::reset(Database* database) {
    m_host = database;
    recompute();

    if (!database) return;

    connect(database->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &RootElementsModel::ident_changed);

    // Root membership is determined by the presence of ChildOfComponent.
    connect(database->parent.self(),
            &ComponentAPIBase::changed,
            this,
            &RootElementsModel::recompute);

    connect(database->parent.self(),
            &ComponentAPIBase::removed,
            this,
            &RootElementsModel::recompute);

    connect(database->element_tag.self(),
            &ComponentAPIBase::changed,
            this,
            &RootElementsModel::recompute);

    connect(database->element_tag.self(),
            &ComponentAPIBase::removed,
            this,
            &RootElementsModel::recompute);
}

} // namespace db
