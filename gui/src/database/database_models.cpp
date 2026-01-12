#include "database_models.h"


database_models::database_models() { }

#if 0


void GroupsModel::_delete_at(size_t index, size_t count) {
    // nothing for now?
}

int GroupsModel::_record_count() const {
    return m_cached.size();
}

void GroupsModel::_clear() {
    m_cached.clear();
}

GroupsModel::GroupsModel(std::shared_ptr<entt::registry> p, QObject* parent)
    : IndirectTableModel(parent), m_host(p) {

    add_properties({
        {
            .display_name = "name",
            .getter       = [this](size_t index) -> QVariant {
                return this->m_cached.value(index).name;
            },
            .setter = [this](size_t index, QVariant value) -> bool {
                auto ent = this->m_cached.value(index).entity;

                if (auto ptr = this->m_host.lock(); ptr) {
                    if (auto comp = ptr->try_get<GroupComponent>(ent); comp) {
                        comp->name                 = value.toString();
                        this->m_cached[index].name = value.toString();
                        return true;
                    }
                }
                return false;
            },
        },
    });

    rebuild_cache();
}

void GroupsModel::rebuild_cache() {
    beginResetModel();

    m_cached.clear();


    endResetModel();
}

#endif
