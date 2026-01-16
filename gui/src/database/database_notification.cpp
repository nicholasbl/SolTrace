#include "database_notification.h"

namespace db {

entt::entity UIApi::add_group(QString new_name, QVector<entt::entity> members) {
    auto lock = m_host.lock();
    if (!lock) return entt::null;

    auto set = std::unordered_set(members.begin(), members.end());

    auto ent = lock->create();
    lock->emplace<GroupComponent>(
        ent,
        GroupComponent {
            .members = QVector<entt::entity>(set.begin(), set.end()),
        });

    lock->emplace<IdentityComponent>(ent,
                                     IdentityComponent { .name = new_name });

    for (auto child : set) {
        lock->emplace_or_replace<GroupMemberComponent>(
            child, GroupMemberComponent { .group = ent });
    }

    return ent;
}

void UIApi::delete_group(entt::entity to_delete, entt::entity move_to) {
    auto lock = m_host.lock();
    if (!lock) return;

    // if not a group, bail
    if (!lock->all_of<GroupComponent>(to_delete)) { return; }

    // steal current member list
    auto members = std::move(lock->get<GroupComponent>(to_delete).members);

    // destroy current group entity
    lock->destroy(to_delete);


    if (lock->valid(move_to) and lock->all_of<GroupComponent>(move_to)) {
        // moving to valid target

        // reset member list membership
        for (auto child : members) {
            lock->emplace_or_replace<GroupMemberComponent>(
                child, GroupMemberComponent { .group = move_to });
        }

        lock->patch<GroupComponent>(move_to, [&](GroupComponent& a) {
            a.members.append(members.begin(), members.end());
        });
    } else {
        // invalid target. clear

        lock->remove<GroupMemberComponent>(members.begin(), members.end());
    }
}

} // namespace db
