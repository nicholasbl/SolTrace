#pragma once


class database_models {
public:
    database_models();
};

// class GroupsModel : public IndirectTableModel {
//     std::weak_ptr<entt::registry> m_host;

//     struct CachedGroup {
//         QString      name;
//         entt::entity entity;
//     };

//     QVector<CachedGroup> m_cached;

//     void _append_new(QVariant) override;
//     void _delete_at(size_t index, size_t count) override;
//     int  _record_count() const override;
//     void _clear() override;

// public:
//     explicit GroupsModel(std::shared_ptr<entt::registry>,
//                          QObject* parent = nullptr);
//     virtual ~GroupsModel() = default;

// public slots:
//     void rebuild_cache();
// };
