#pragma once


#include "database/database.h"
#include "utilities/qt_helpers.h"
#include "utilities/structmodel.h"

#include <entt/entity/fwd.hpp>

#include <QQuaternion>
#include <QStringListModel>
#include <QVector3D>

namespace db {

/// Get the hierarchy of an entity, walks the parent chain and provides a string
/// list, starting from the root on down.
class BreadcrumbModel : public QStringListModel {
    Q_OBJECT

    QPointer<Database> m_host;

    QVector<entt::entity> m_path;

    Q_WRITABLE_PROPERTY(entt::entity, node, entt::null);

private slots:
    void recompute();

public:
    explicit BreadcrumbModel(QObject* parent = nullptr);
    virtual ~BreadcrumbModel() = default;

    void reset(Database* database);
};

// =============================================================================

struct EntityNamePair {
    QString      name;
    entt::entity entity;

    RECORD_META(db::EntityNamePair, SM_EXPOSE_RW(name), SM_EXPOSE_RO(entity), );
};

/// A model providing all children of a given entity
class ChildModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT
    QPointer<Database> m_host;

    Q_WRITABLE_PROPERTY(entt::entity, node, entt::null);

    // QVector<entt::entity>                 m_list;
    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();
    void ident_changed(entt::entity);

public:
    explicit ChildModel(QObject* parent = nullptr);
    virtual ~ChildModel() = default;

    void reset(Database* database);
};

// =============================================================================

/// A model providing the active groups in a database
class RenderGroupsModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT

    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();

    void group_changed(entt::entity);
    void group_removed(entt::entity);

public:
    explicit RenderGroupsModel(QObject* parent = nullptr);
    virtual ~RenderGroupsModel() = default;

    void reset(Database* database);
};

// =============================================================================

/// A model providing the available tags in a database
class TagsModel : public StructModelAdapter<EntityNamePair> {
    Q_OBJECT
    QPointer<Database> m_host;

    std::unordered_map<entt::entity, int> m_reverse;

    QVector<EntityNamePair> rebuild_lists();

private slots:
    void recompute();

    void tag_changed(entt::entity);
    void tag_removed(entt::entity);

public:
    explicit TagsModel(QObject* parent = nullptr);
    virtual ~TagsModel() = default;

    void reset(Database* database);
};


// =============================================================================

/// A model that helps edit a geometry instance
class AnInstanceEditor : public QObject {
    Q_OBJECT
    QPointer<Database> m_host;

    entt::entity m_entity = entt::null;

    Q_PROPERTY(QString entity_name READ entity_name WRITE set_entity_name NOTIFY
                   entity_name_changed FINAL)

    Q_PROPERTY(QVector3D position READ position WRITE set_position NOTIFY
                   position_changed FINAL)
    Q_PROPERTY(QQuaternion orientation READ orientation WRITE set_orientation
                   NOTIFY orientation_changed FINAL)
    Q_PROPERTY(
        bool hidden READ hidden WRITE set_hidden NOTIFY hidden_changed FINAL)

    Q_PROPERTY(entt::entity group READ group WRITE set_group NOTIFY
                   group_changed FINAL)

    Q_PROPERTY(entt::entity parent READ parent WRITE set_parent NOTIFY
                   parent_changed FINAL)

    Q_PROPERTY(QVector<entt::entity> tags READ tags WRITE set_tags NOTIFY
                   tags_changed FINAL)

private slots:
    void an_entity_changed(entt::entity);
    void recompute();

public:
    explicit AnInstanceEditor(QObject* parent = nullptr);
    virtual ~AnInstanceEditor() = default;

    void reset(Database* database);

    void set(entt::entity);

public:
    QString entity_name() const;
    void    set_entity_name(const QString& newEntity_name);

    QVector3D position() const;
    void      set_position(const QVector3D& newPosition);

    QQuaternion orientation() const;
    void        set_orientation(const QQuaternion& newOrientation);

    bool hidden() const;
    void set_hidden(bool newHidden);

    entt::entity group() const;
    void         set_group(entt::entity newGroup);

    entt::entity parent() const;
    void         set_parent(entt::entity newParent);

    QVector<entt::entity> tags() const;
    void                  set_tags(const QVector<entt::entity>& newTags);

signals:
    void position_changed();
    void orientation_changed();
    void hidden_changed();
    void group_changed();
    void parent_changed();
    void tags_changed();
    void entity_name_changed();
};

} // namespace db
