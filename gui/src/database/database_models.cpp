#include "database_models.h"

#include "components.h"
#include "database/database.h"
#include "database/database_notification.h"
#include "utilities/math_utility.h"

namespace db {

void BreadcrumbModel::recompute() {
    m_path.clear();

    if (!m_host) return;

    if (!m_host->valid(m_node)) return;

    std::unordered_set<entt::entity> seen;

    entt::entity looking_at = m_node;

    while (true) {

        if (!m_host->valid(looking_at)) { break; }

        if (seen.contains(looking_at)) {
            // cycles??
            break;
        }

        seen.insert(looking_at);

        m_path.push_back(looking_at);

        looking_at = m_host->parent_of(looking_at);
    }

    std::reverse(m_path.begin(), m_path.end());

    QStringList ret;

    for (auto e : std::as_const(m_path)) {
        ret << m_host->name_of(e);
    }

    this->setStringList(ret);
}

BreadcrumbModel::BreadcrumbModel(QObject* parent) : QStringListModel(parent) { }

void BreadcrumbModel::reset(Database* database) {
    m_host = database;
    m_node = entt::null;
    recompute();

    connect(this,
            &BreadcrumbModel::node_changed,
            this,
            &BreadcrumbModel::recompute);

    if (database) {
        connect(database->identity.self(),
                &ComponentAPIBase::changed,
                this,
                [this](entt::entity e) {
                    if (m_path.contains(e)) { this->recompute(); }
                });
    }
}


// =============================================================================

static EntityNamePair rec_for_node(Database& reg, entt::entity item) {
    return EntityNamePair { .name = reg.name_of(item), .entity = item };
}

QVector<EntityNamePair> ChildModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    if (!m_host) return {};

    if (!m_host->valid(m_node)) return {};

    auto children = m_host->children_of(m_node);

    // Copy here, can revise later
    new_recs.reserve(children.size());

    for (auto x : children) {
        new_recs << rec_for_node(*m_host, x);
    }

    for (size_t i = 0; i < new_recs.size(); i++) {
        m_reverse[new_recs[i].entity] = i;
    }

    return new_recs;
}

void ChildModel::recompute() {
    auto r = rebuild_lists();

    this->store_reset(r);
}

void ChildModel::ident_changed(entt::entity e) {
    if (!m_host) return;

    if (auto iter = m_reverse.find(e); iter != m_reverse.end()) {
        this->store_push_update(iter->second, rec_for_node(*m_host, e));
    }
}

ChildModel::ChildModel(QObject* parent) : StructModelAdapter(parent) { }

void ChildModel::reset(Database* database) {
    m_host = database;
    m_node = entt::null;
    recompute();

    connect(this, &ChildModel::node_changed, this, &ChildModel::recompute);

    if (database) {
        connect(database->children.self(),
                &ComponentAPIBase::changed,
                this,
                [this](entt::entity e) {
                    if (node() == e) { recompute(); }
                });

        connect(database->identity.self(),
                &ComponentAPIBase::changed,
                this,
                &ChildModel::ident_changed);
    }
}

// =============================================================================

QVector<EntityNamePair> GroupsModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    if (!m_host) return {};

    auto view = m_host->as_registry().view<GroupComponent>();

    for (auto const& [e, group] : view.each()) {
        new_recs.push_back(rec_for_node(*m_host, e));
    }

    for (size_t i = 0; i < new_recs.size(); i++) {
        m_reverse[new_recs[i].entity] = i;
    }

    return new_recs;
}

void GroupsModel::recompute() {
    auto r = rebuild_lists();

    this->store_reset(r);
}

void GroupsModel::group_changed(entt::entity e) {
    if (!m_host) return;

    auto iter = m_reverse.find(e);

    if (iter == m_reverse.end()) { return recompute(); }

    this->store_push_update(iter->second, rec_for_node(*m_host, e));
}
void GroupsModel::group_removed(entt::entity e) {
    recompute();
}

GroupsModel::GroupsModel(QObject* parent) : StructModelAdapter(parent) { }

void GroupsModel::reset(Database* database) {
    m_host = database;
    recompute();

    if (!database) { return; }

    connect(database->group_root.self(),
            &ComponentAPIBase::changed,
            this,
            &GroupsModel::group_changed);

    connect(database->group_root.self(),
            &ComponentAPIBase::removed,
            this,
            &GroupsModel::group_removed);

    connect(database->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &GroupsModel::group_changed);
}

// =============================================================================

QVector<EntityNamePair> TagsModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    if (!m_host) return {};

    auto view = m_host->as_registry().view<TagComponent>();

    for (auto const& e : view.each()) {
        new_recs.push_back(rec_for_node(*m_host, std::get<0>(e)));
    }

    for (size_t i = 0; i < new_recs.size(); i++) {
        m_reverse[new_recs[i].entity] = i;
    }

    return new_recs;
}

void TagsModel::recompute() {
    auto r = rebuild_lists();

    this->store_reset(r);
}

void TagsModel::tag_changed(entt::entity e) {
    if (!m_host) return;

    auto iter = m_reverse.find(e);

    if (iter == m_reverse.end()) { return recompute(); }

    this->store_push_update(iter->second, rec_for_node(*m_host, e));
}
void TagsModel::tag_removed(entt::entity e) {
    recompute();
}

TagsModel::TagsModel(QObject* parent) : StructModelAdapter(parent) { }

void TagsModel::reset(Database* database) {
    m_host = database;
    recompute();

    if (!database) return;

    connect(database->tag_root.self(),
            &ComponentAPIBase::changed,
            this,
            &TagsModel::tag_changed);

    connect(database->tag_root.self(),
            &ComponentAPIBase::removed,
            this,
            &TagsModel::tag_removed);

    connect(database->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &TagsModel::tag_changed);
}

// =============================================================================


#define FIND(MEM)                                                              \
    if (!m_host) return;                                                       \
    if (!m_host->valid(m_entity)) return;                                      \
    auto& component = m_host->MEM;

void AnInstanceEditor::recompute() {
    emit position_changed();
    emit orientation_changed();
    emit hidden_changed();
    emit group_changed();
    emit parent_changed();
    emit tags_changed();
}

void AnInstanceEditor::an_entity_changed(entt::entity e) {
    if (m_entity != e) return;
    recompute();
}

AnInstanceEditor::AnInstanceEditor(QObject* parent) : QObject(parent) {
    /*
     *     auto lock = m_host.lock();

     if (!lock) { return set_empty(); }

      auto note = get_notifier(*lock);

     if (!note) { return set_empty(); }

      if (!lock->valid(m_entity)) { return set_empty(); }

     */
}

void AnInstanceEditor::set(entt::entity ent) {
    m_entity = ent;
    recompute();
}

void AnInstanceEditor::reset(Database* database) {
    m_host = database;

    if (!database) return;

    connect(database->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(database->transform.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(database->invisible.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(database->group_membership.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(database->tag_membership.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(database->parent.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    recompute();
}

QVector3D AnInstanceEditor::position() const {

    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->transform.get(m_entity); tf) {
            return convert(tf->position);
        }
    }

    return {};
}


void AnInstanceEditor::set_position(const QVector3D& newPosition) {
    if (position() == newPosition) return;

    FIND(transform);

    component.patch(m_entity, [&](TransformComponent& a) {
        a.position = convert(newPosition);
    });

    emit position_changed();
}

QQuaternion AnInstanceEditor::orientation() const {
    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->transform.get(m_entity); tf) {
            return convert(tf->rotation);
        }
    }
    return {};
}

void AnInstanceEditor::set_orientation(const QQuaternion& newOrientation) {
    if (orientation() == newOrientation) return;

    FIND(transform);

    component.patch(m_entity, [&](TransformComponent& a) {
        a.rotation = convert(newOrientation);
    });

    emit orientation_changed();
}

bool AnInstanceEditor::hidden() const {
    if (m_host and m_host->valid(m_entity)) {
        if (m_host->as_registry().any_of<InvisibleComponent>(m_entity)) {
            return true;
        }
    }

    return false;
}

void AnInstanceEditor::set_hidden(bool newHidden) {
    if (hidden() == newHidden) return;
    FIND(invisible);

    if (newHidden) {
        component.set(m_entity, InvisibleComponent {});
    } else {
        component.remove(m_entity);
    }


    emit hidden_changed();
}

entt::entity AnInstanceEditor::group() const {
    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->group_membership.get(m_entity); tf) { return tf->group; }
    }

    return entt::null;
}

void AnInstanceEditor::set_group(entt::entity newGroup) {
    if (group() == newGroup) return;

    m_host->assign_group(m_entity, newGroup);

    emit group_changed();
}

entt::entity AnInstanceEditor::parent() const {
    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->parent.get(m_entity); tf) { return tf->parent; }
    }

    return entt::null;
}

void AnInstanceEditor::set_parent(entt::entity newParent) {
    if (parent() == newParent) return;

    m_host->set_parent(m_entity, newParent);

    emit parent_changed();
}

QVector<entt::entity> AnInstanceEditor::tags() const {
    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->tag_membership.get(m_entity); tf) { return tf->tags; }
    }

    return {};
}

void AnInstanceEditor::set_tags(QVector<entt::entity> const& newTags) {
    auto current_tags = tags();

    std::unordered_set<entt::entity> incoming(newTags.begin(), newTags.end());
    std::unordered_set<entt::entity> current(current_tags.begin(),
                                             current_tags.end());

    if (incoming == current) return;
    if (!m_host) return;
    if (!m_host->valid(m_entity)) return;

    for (auto new_tag : incoming) {
        if (!current.contains(new_tag)) {
            m_host->assign_tag(m_entity, new_tag);
        }
    }

    // Remove tags no longer present.
    for (auto old_tag : current) {
        if (!incoming.contains(old_tag)) {
            m_host->unassign_tag(m_entity, old_tag);
        }
    }

    emit tags_changed();
}

QString AnInstanceEditor::entity_name() const {
    if (m_host and m_host->valid(m_entity)) {
        if (auto tf = m_host->identity.get(m_entity); tf) { return tf->name; }
    }

    return {};
}

void AnInstanceEditor::set_entity_name(const QString& newEntity_name) {
    if (entity_name() == newEntity_name) return;

    FIND(identity);

    component.set(m_entity, IdentityComponent { .name = newEntity_name });

    emit entity_name_changed();
}

} // namespace db
