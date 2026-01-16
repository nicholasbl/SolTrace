#include "database_models.h"

#include "components.h"
#include "database/database.h"
#include "database/database_notification.h"

namespace db {

void BreadcrumbModel::recompute() {
    m_path.clear();

    auto lock = m_host.lock();

    if (!lock) return;

    if (!lock->valid(m_node)) return;

    std::unordered_set<entt::entity> seen;

    entt::entity looking_at = m_node;

    while (true) {

        if (!lock->valid(looking_at)) { break; }

        if (seen.contains(looking_at)) {
            // cycles??
            break;
        }

        seen.insert(looking_at);

        m_path.push_back(looking_at);

        auto* ptr = lock->try_get<ChildOfComponent>(looking_at);

        if (!ptr) break;

        looking_at = ptr->parent;
    }

    std::reverse(m_path.begin(), m_path.end());

    QStringList ret;

    for (auto e : std::as_const(m_path)) {
        ret << name_of(*lock, e);
    }

    this->setStringList(ret);
}

BreadcrumbModel::BreadcrumbModel(std::shared_ptr<entt::registry> p,
                                 QObject*                        parent)
    : QStringListModel(parent), m_host(p) {
    reset(p);
}

void BreadcrumbModel::reset(std::shared_ptr<entt::registry> ptr) {
    m_host = ptr;
    m_node = entt::null;
    recompute();

    connect(this,
            &BreadcrumbModel::node_changed,
            this,
            &BreadcrumbModel::recompute);

    connect(get_notifier(*ptr)->identity.self(),
            &ComponentAPIBase::changed,
            this,
            [this](entt::entity e) {
                if (m_path.contains(e)) { this->recompute(); }
            });
}


// =============================================================================

static EntityNamePair rec_for_node(entt::registry& reg, entt::entity item) {
    return EntityNamePair { .name = name_of(reg, item), .entity = item };
}

QVector<EntityNamePair> ChildModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    auto lock = m_host.lock();

    if (!lock) return {};

    if (!lock->valid(m_node)) return {};

    auto ptr = lock->try_get<ChildrenComponent>(m_node);

    if (!ptr) return {};

    // Copy here, can revise later
    new_recs.reserve(ptr->children.size());

    for (auto x : ptr->children) {
        new_recs << rec_for_node(*lock, x);
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
    auto lock = m_host.lock();

    if (!lock) return;

    if (auto iter = m_reverse.find(e); iter != m_reverse.end()) {
        this->store_push_update(iter->second, rec_for_node(*lock, e));
    }
}

ChildModel::ChildModel(std::shared_ptr<entt::registry> p, QObject* parent)
    : StructModelAdapter(parent), m_host(p) {
    reset(p);
}

void ChildModel::reset(std::shared_ptr<entt::registry> ptr) {
    m_host = ptr;
    m_node = entt::null;
    recompute();

    connect(this, &ChildModel::node_changed, this, &ChildModel::recompute);

    connect(get_notifier(*ptr)->children.self(),
            &ComponentAPIBase::changed,
            this,
            [this](entt::entity e) {
                if (node() == e) { recompute(); }
            });

    connect(get_notifier(*ptr)->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &ChildModel::ident_changed);
}

// =============================================================================

QVector<EntityNamePair> GroupsModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    auto lock = m_host.lock();

    if (!lock) return {};

    auto view = lock->view<GroupComponent>();

    for (auto const& [e, group] : view.each()) {
        new_recs.push_back(rec_for_node(*lock, e));
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
    auto lock = m_host.lock();

    if (!lock) return;

    auto iter = m_reverse.find(e);

    if (iter == m_reverse.end()) { return recompute(); }

    this->store_push_update(iter->second, rec_for_node(*lock, e));
}
void GroupsModel::group_removed(entt::entity e) {
    recompute();
}

GroupsModel::GroupsModel(std::shared_ptr<entt::registry> p, QObject* parent)
    : StructModelAdapter(parent), m_host(p) {
    reset(p);
}

void GroupsModel::reset(std::shared_ptr<entt::registry> ptr) {
    m_host = ptr;
    recompute();

    auto* notifier = get_notifier(*ptr);

    connect(notifier->group_root.self(),
            &ComponentAPIBase::changed,
            this,
            &GroupsModel::group_changed);

    connect(notifier->group_root.self(),
            &ComponentAPIBase::removed,
            this,
            &GroupsModel::group_removed);

    connect(notifier->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &GroupsModel::group_changed);
}

// =============================================================================

QVector<EntityNamePair> TagsModel::rebuild_lists() {
    QVector<EntityNamePair> new_recs;
    m_reverse.clear();

    auto lock = m_host.lock();

    if (!lock) return {};

    auto view = lock->view<TagComponent>();

    for (auto const& e : view.each()) {
        new_recs.push_back(rec_for_node(*lock, std::get<0>(e)));
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
    auto lock = m_host.lock();

    if (!lock) return;

    auto iter = m_reverse.find(e);

    if (iter == m_reverse.end()) { return recompute(); }

    this->store_push_update(iter->second, rec_for_node(*lock, e));
}
void TagsModel::tag_removed(entt::entity e) {
    recompute();
}

TagsModel::TagsModel(std::shared_ptr<entt::registry> p, QObject* parent)
    : StructModelAdapter(parent), m_host(p) {
    reset(p);
}

void TagsModel::reset(std::shared_ptr<entt::registry> ptr) {
    m_host = ptr;
    recompute();

    auto* notifier = get_notifier(*ptr);

    connect(notifier->tag_root.self(),
            &ComponentAPIBase::changed,
            this,
            &TagsModel::tag_changed);

    connect(notifier->tag_root.self(),
            &ComponentAPIBase::removed,
            this,
            &TagsModel::tag_removed);

    connect(notifier->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &TagsModel::tag_changed);
}

// =============================================================================


#define FIND(MEM)                                                              \
    auto lock = m_host.lock();                                                 \
    if (!lock) return;                                                         \
    if (!lock->valid(m_entity)) return;                                        \
    auto note = get_notifier(*lock);                                           \
    if (!note) return;                                                         \
    auto& component = note->MEM;

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

AnInstanceEditor::AnInstanceEditor(std::shared_ptr<entt::registry> ptr,
                                   QObject*                        parent)
    : QObject(parent) {
    reset(ptr);

    auto lock = m_host.lock();
    if (!lock) return;
    if (!lock->valid(m_entity)) return;
    auto note = get_notifier(*lock);
    if (!note) return;

    connect(note->identity.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(note->transform.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(note->invisible.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(note->group.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(note->tags.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

    connect(note->parent.self(),
            &ComponentAPIBase::changed,
            this,
            &AnInstanceEditor::an_entity_changed);

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

void AnInstanceEditor::reset(std::shared_ptr<entt::registry> ptr) {
    m_host = ptr;
    recompute();
}

QVector3D AnInstanceEditor::position() const {

    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->transform.get(m_entity); tf) {
                return tf->position;
            }
        }
    }

    return {};
}


void AnInstanceEditor::set_position(const QVector3D& newPosition) {
    if (position() == newPosition) return;

    FIND(transform);

    component.patch(m_entity,
                    [&](TransformComponent& a) { a.position = newPosition; });

    emit position_changed();
}

QQuaternion AnInstanceEditor::orientation() const {
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->transform.get(m_entity); tf) {
                return tf->rotation;
            }
        }
    }
    return {};
}

void AnInstanceEditor::set_orientation(const QQuaternion& newOrientation) {
    if (orientation() == newOrientation) return;

    FIND(transform);

    component.patch(
        m_entity, [&](TransformComponent& a) { a.rotation = newOrientation; });

    emit orientation_changed();
}

bool AnInstanceEditor::hidden() const {
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (lock->any_of<InvisibleComponent>(m_entity)) { return true; }
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
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->group.get(m_entity); tf) { return tf->group; }
        }
    }

    return entt::null;
}

void AnInstanceEditor::set_group(entt::entity newGroup) {
    if (group() == newGroup) return;

    assign_group(*m_host.lock(), m_entity, newGroup);

    emit group_changed();
}

entt::entity AnInstanceEditor::parent() const {
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->parent.get(m_entity); tf) { return tf->parent; }
        }
    }

    return entt::null;
}

void AnInstanceEditor::set_parent(entt::entity newParent) {
    if (parent() == newParent) return;

    ::db::set_parent(*m_host.lock(), m_entity, newParent);

    emit parent_changed();
}

QVector<entt::entity> AnInstanceEditor::tags() const {
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->tags.get(m_entity); tf) { return tf->tags; }
        }
    }

    return {};
}

void AnInstanceEditor::set_tags(QVector<entt::entity> const& newTags) {
    auto current_tags = tags();

    std::unordered_set<entt::entity> incoming(newTags.begin(), newTags.end());
    std::unordered_set<entt::entity> current(current_tags.begin(),
                                             current_tags.end());

    if (incoming == current) return;
    auto lock = m_host.lock();
    if (!lock) return;
    if (!lock->valid(m_entity)) return;

    for (auto new_tag : incoming) {
        if (!current.contains(new_tag)) {
            assign_tag(*lock, m_entity, new_tag);
        }
    }

    // Remove tags no longer present.
    for (auto old_tag : current) {
        if (!incoming.contains(old_tag)) {
            unassign_tag(*lock, m_entity, old_tag);
        }
    }

    emit tags_changed();
}

QString AnInstanceEditor::entity_name() const {
    if (auto lock = m_host.lock(); lock and lock->valid(m_entity)) {
        if (auto note = get_notifier(*lock); note) {
            if (auto tf = note->identity.get(m_entity); tf) { return tf->name; }
        }
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
