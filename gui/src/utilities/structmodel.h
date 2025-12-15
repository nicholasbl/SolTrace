#pragma once

#include <QAbstractTableModel>
#include <QDebug>

#include <span>


template <class T>
struct SMRecordMeta {
    using SMGetter = QVariant (*)(T const&);
    using SMSetter = void     (*)(T&, QVariant const&);


    const char* name   = "";
    size_t      offset = 0;
    SMGetter    getter = nullptr;
    SMSetter    setter = nullptr;
};

template <class T>
struct SMMetaGetter;

template <class T>
concept is_shared_ptr = std::is_same_v<T, std::shared_ptr<T>>;

#define EXPOSE_RW(RT, MEM)                                                     \
    SMRecordMeta<RT> {                                                         \
        .name = #MEM, .offset = offsetof(RT, MEM),                             \
        .getter = [](auto const& t) -> QVariant {                              \
            return QVariant::fromValue(t.MEM);                                 \
        },                                                                     \
        .setter = [](auto& t, QVariant const& a) {                             \
            using LT = std::remove_cvref_t<decltype(t.MEM)>;                   \
            t.MEM    = a.value<LT>();                                          \
        },                                                                     \
    }

#define EXPOSE_RO(RT, MEM)                                                     \
    SMRecordMeta<RT> {                                                         \
        .name = #MEM, .offset = offsetof(RT, MEM),                             \
        .getter = [](auto const& t) -> QVariant {                              \
            if constexpr (is_shared_ptr<decltype(t.MEM)>) {                    \
                return t.MEM.get();                                            \
            } else {                                                           \
                return QVariant::fromValue(t.MEM);                             \
            }                                                                  \
        },                                                                     \
        .setter = nullptr,                                                     \
    }

#define RECORD_META(RT, ...)                                                   \
    template <>                                                                \
    struct SMMetaGetter<RT> {                                                  \
        static inline constexpr std::array meta = { __VA_ARGS__ };             \
    };

template <class Record>
QStringList get_header() {
    QStringList ret;
    constexpr auto const& meta = SMMetaGetter<Record>::meta;
    for (auto m : meta) {
        ret << m.name;
    }
    return ret;
}

template <class Record>
QHash<int, QByteArray> const& get_name_map() {
    constexpr auto const& meta = SMMetaGetter<Record>::meta;

    static QHash<int, QByteArray> ret = []() {
        QHash<int, QByteArray> build;

        for (int i = 0; i < std::size(meta); i++) {
            build[Qt::UserRole + i] = meta[i].name;
        }

        return build;
    }();
    return ret;
}

template <class Record>
constexpr int role_for_member_offset(size_t off) {
    constexpr auto const& meta = SMMetaGetter<Record>::meta;

    for (int i = 0; i < std::size(meta); i++) {
        if (meta[i].offset == off) { return Qt::UserRole + i; }
    }
}

#define ROLE_FOR_MEMBER(RT, MEM) role_for_member_offset(offsetof(RT, MEM))


template <class T>
constexpr bool is_qobject = std::is_base_of_v<QObject, std::remove_cvref_t<T>>;

template <class T>
struct is_shared_qobject {
    static constexpr bool value = false;
};

template <class T>
struct is_shared_qobject<std::shared_ptr<T>> {
    static constexpr bool value = is_qobject<T>;
};

template <class Record>
QVariant record_runtime_get(Record const& r, int i) {
    constexpr auto const& meta   = SMMetaGetter<Record>::meta;
    auto                  getter = meta.at(i).getter;
    if (getter) { return std::invoke(getter, r); }
    return {};
}

template <class Record>
bool record_runtime_set(Record& r, int i, QVariant const& v) {
    constexpr auto const& meta   = SMMetaGetter<Record>::meta;
    auto                  setter = meta.at(i).setter;

    if (setter) {
        std::invoke(setter, r, v);
        return true;
    }

    return false;
}

class StructTableModelBase : public QAbstractTableModel {
    Q_OBJECT
public:
    using QAbstractTableModel::QAbstractTableModel;
};

template <class Record>
class StructTableModel : public StructTableModelBase {
    QVector<Record> m_records;

    QStringList const m_header;

public:
    explicit StructTableModel(QObject* parent = nullptr)
        : StructTableModelBase(parent), m_header(get_header<Record>()) { }

    // Header:
    QVariant headerData(int             section,
                        Qt::Orientation orientation,
                        int             role = Qt::DisplayRole) const override {
        if (orientation != Qt::Orientation::Horizontal) return {};
        if (role != Qt::DisplayRole) return {};

        return m_header.value(section);
    }

    int rowCount(QModelIndex const& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return m_records.size();
    }

    int columnCount(QModelIndex const& parent = QModelIndex()) const override {
        if (parent.isValid()) return 0;
        return m_header.size();
    }

    QVariant data(QModelIndex const& index,
                  int                role = Qt::DisplayRole) const override {

        // qDebug() << Q_FUNC_INFO << index << role;

        if (!index.isValid()) return {};
        if (index.row() >= m_records.size()) return {};

        auto const& item = m_records[index.row()];

        if (role == Qt::DisplayRole or role == Qt::EditRole) {
            return record_runtime_get(item, index.column());
        }

        if (role >= Qt::UserRole) {
            auto local_role = role - Qt::UserRole;

            assert(local_role >= 0);

            if (local_role >= m_header.size()) return {};

            return record_runtime_get(item, local_role);
        }

        return {};
    }


    bool setData(QModelIndex const& index,
                 QVariant const&    value,
                 int                role = Qt::EditRole) override {

        // qDebug() << Q_FUNC_INFO << index << value << role;

        if (data(index, role) == value) return false;

        auto& item = m_records[index.row()];

        int location = -1;

        if (role >= Qt::UserRole) {
            location = role - Qt::UserRole;
        } else {
            location = index.column();
        }

        if (location >= m_header.size()) return false;

        bool ok = record_runtime_set(item, location, value);

        if (!ok) return false;

        Q_EMIT dataChanged(index, index, QList<int>() << role);
        return true;
    }

    Qt::ItemFlags flags(QModelIndex const& index) const override {
        if (!index.isValid()) return Qt::NoItemFlags;

        auto const& meta = SMMetaGetter<Record>::meta;

        bool can_edit = !!meta.at(index.column()).setter;

        if (!can_edit) return Qt::ItemIsEnabled;

        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }

    QHash<int, QByteArray> roleNames() const override {
        static const auto roles = get_name_map<Record>();

        return roles;
    }

    bool insertRows(int                row,
                    int                count,
                    QModelIndex const& p = QModelIndex()) override {
        if (row < 0 or count <= 0) return false;

        beginInsertRows(p, row, row + count - 1);
        m_records.insert(row, count, Record {});
        endInsertRows();
        return true;
    }

    bool removeRows(int                row,
                    int                count,
                    QModelIndex const& p = QModelIndex()) override {
        if (row < 0 or count <= 0) return false;
        if (count > m_records.size()) return false;

        beginRemoveRows(p, row, row + count - 1);
        m_records.remove(row, count);
        endRemoveRows();
        return true;
    }

    void reset(QList<Record> new_records = {}) {
        // qDebug() << Q_FUNC_INFO;
        beginResetModel();
        m_records = new_records;
        endResetModel();
    }

    // this emits a remove signal, instead of a reset
    void remove_all() {
        // qDebug() << Q_FUNC_INFO;
        if (m_records.isEmpty()) return;
        beginRemoveRows(QModelIndex(), 0, std::max(rowCount() - 1, 0));
        m_records.clear();
        endRemoveRows();
    }

    Record const* get_at(int i) const {
        if (i < 0) return nullptr;
        if (i >= m_records.size()) return nullptr;
        return &m_records[i];
    }

    auto append(Record const& r) {
        int rc = rowCount();
        beginInsertRows({}, rc, rc);
        m_records << r;
        endInsertRows();
    }

    auto append(QVector<Record> r) {
        if (r.isEmpty()) return;
        int rc = rowCount();
        beginInsertRows({}, rc, rc + r.size() - 1);
        m_records << r;
        endInsertRows();
    }

    auto replace(QVector<Record> r = {}) {
        remove_all();
        append(r);
    }

    auto update(int i, Record const& r) {
        // qDebug() << Q_FUNC_INFO;

        if (i < 0) return;
        if (i >= m_records.size()) return;

        m_records[i] = r;

        auto left  = index(i, 0);
        auto right = index(i, columnCount() - 1);

        Q_EMIT dataChanged(left, right);
    }

    void remove_at(int index, int count = 1) {
        if (index < 0) return;
        if (index >= m_records.size()) return;

        beginRemoveRows(QModelIndex(), index, index + count - 1);
        m_records.remove(index, count);
        endRemoveRows();
    }

    void insert_at(int index, std::span<Record> records) {
        qDebug() << Q_FUNC_INFO << index << (m_records.size());
        if (records.empty()) return;
        beginInsertRows({}, index, index + records.size() - 1);
        m_records.insert(index, records.size(), Record {});
        for (int i = 0; i < records.size(); i++) {
            m_records[index + i] = records[i];
        }
        endInsertRows();
    }


    // delete by a predicate
    template <class Function>
    void remove_by_predicate(Function&& f) {
        QVector<int> to_remove;

        for (int i = 0; i < m_records.size(); i++) {
            if (f(m_records[i])) to_remove << i;
        }

        // sort to high -> low, so indices are preserved
        std::reverse(to_remove.begin(), to_remove.end());

        for (auto i : std::as_const(to_remove)) {
            remove_at(i);
        }
    }

    auto const& vector() const { return m_records; }

    auto begin() const { return m_records.begin(); }
    auto end() const { return m_records.end(); }

    auto cbegin() const { return m_records.begin(); }
    auto cend() const { return m_records.end(); }
};
