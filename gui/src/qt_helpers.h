#pragma once

#include <QPointer>
#include <QStringList>
#include <QMetaEnum>

//─── Property Macros ───────────────────────────────────────────────────────────────────

#define QOBJECT_WRITABLE_PROPERTY(TYPE, PROPERTY_NAME)                         \
public:                                                                        \
    Q_PROPERTY(TYPE* PROPERTY_NAME READ PROPERTY_NAME WRITE                    \
                   set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##_changed)         \
protected:                                                                     \
    QPointer<TYPE> m_##PROPERTY_NAME;                                          \
                                                                               \
public:                                                                        \
    TYPE* PROPERTY_NAME() const {                                              \
        return m_##PROPERTY_NAME;                                              \
    }                                                                          \
public Q_SLOTS:                                                                \
    void set_##PROPERTY_NAME(TYPE* new_value) {                                \
        if (m_##PROPERTY_NAME == new_value) return;                            \
        m_##PROPERTY_NAME = new_value;                                         \
        Q_EMIT PROPERTY_NAME##_changed();                                      \
    }                                                                          \
Q_SIGNALS:                                                                     \
    void PROPERTY_NAME##_changed();                                            \
                                                                               \
public:


#define QOBJECT_READONLY_PROPERTY(TYPE, PROPERTY_NAME)                         \
public:                                                                        \
    Q_PROPERTY(TYPE* PROPERTY_NAME READ PROPERTY_NAME CONSTANT)                \
protected:                                                                     \
    QPointer<TYPE> m_##PROPERTY_NAME;                                          \
                                                                               \
public:                                                                        \
    TYPE* PROPERTY_NAME() const {                                              \
        return m_##PROPERTY_NAME;                                              \
    }                                                                          \
                                                                               \
public:


#define Q_WRITABLE_PROPERTY(TYPE, PROPERTY_NAME, VALUE)                        \
public:                                                                        \
    Q_PROPERTY(TYPE PROPERTY_NAME READ PROPERTY_NAME WRITE                     \
                   set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##_changed)         \
protected:                                                                     \
    TYPE m_##PROPERTY_NAME = VALUE;                                            \
                                                                               \
public:                                                                        \
    TYPE PROPERTY_NAME() const {                                               \
        return m_##PROPERTY_NAME;                                              \
    }                                                                          \
public Q_SLOTS:                                                                \
    void set_##PROPERTY_NAME(TYPE const& new_value) {                          \
        if (m_##PROPERTY_NAME == new_value) return;                            \
        m_##PROPERTY_NAME = new_value;                                         \
        Q_EMIT PROPERTY_NAME##_changed();                                      \
    }                                                                          \
Q_SIGNALS:                                                                     \
    void PROPERTY_NAME##_changed();                                            \
                                                                               \
public:

#define Q_WRITABLE_PROPERTY_INDIRECT(TYPE, PROPERTY_NAME, GETTER, SETTER)      \
public:                                                                        \
    Q_PROPERTY(TYPE PROPERTY_NAME READ PROPERTY_NAME WRITE                     \
                   set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##_changed)         \
                                                                               \
public:                                                                        \
    TYPE PROPERTY_NAME() const {                                               \
        return GETTER();                                                       \
    }                                                                          \
public Q_SLOTS:                                                                \
    void set_##PROPERTY_NAME(TYPE const& new_value) {                          \
        if (GETTER() == new_value) return;                                     \
        SETTER(new_value);                                                     \
        Q_EMIT PROPERTY_NAME##_changed();                                      \
    }                                                                          \
Q_SIGNALS:                                                                     \
    void PROPERTY_NAME##_changed();                                            \
                                                                               \
public:

#define Q_WRITABLE_PROPERTY_CUSTOM_STORAGE(TYPE, PROPERTY_NAME, LOCATION)      \
public:                                                                        \
    Q_PROPERTY(TYPE PROPERTY_NAME READ PROPERTY_NAME WRITE                     \
                   set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##_changed)         \
                                                                               \
public:                                                                        \
    TYPE PROPERTY_NAME() const {                                               \
        return LOCATION;                                                       \
    }                                                                          \
public Q_SLOTS:                                                                \
    void set_##PROPERTY_NAME(TYPE const& new_value) {                          \
        if ((LOCATION) == new_value) return;                                   \
        LOCATION = new_value;                                                  \
        Q_EMIT PROPERTY_NAME##_changed();                                      \
    }                                                                          \
Q_SIGNALS:                                                                     \
    void PROPERTY_NAME##_changed();                                            \
                                                                               \
public:


#define Q_READONLY_PROPERTY(TYPE, PROPERTY_NAME)                               \
public:                                                                        \
    Q_PROPERTY(                                                                \
        TYPE PROPERTY_NAME READ PROPERTY_NAME NOTIFY PROPERTY_NAME##_changed)  \
protected:                                                                     \
    TYPE m_##PROPERTY_NAME = TYPE();                                           \
                                                                               \
public:                                                                        \
    TYPE PROPERTY_NAME() const {                                               \
        return m_##PROPERTY_NAME;                                              \
    }                                                                          \
    void set_##PROPERTY_NAME(TYPE const& new_value) {                          \
        if (m_##PROPERTY_NAME == new_value) return;                            \
        m_##PROPERTY_NAME = new_value;                                         \
        Q_EMIT PROPERTY_NAME##_changed();                                      \
    }                                                                          \
Q_SIGNALS:                                                                     \
    void PROPERTY_NAME##_changed();                                            \
                                                                               \
public:


#define QPOINTER_WRITABLE_PROPERTY(TYPE, NAME)                                 \
public:                                                                        \
    Q_PROPERTY(TYPE* NAME READ NAME WRITE set_##NAME NOTIFY NAME##_changed)    \
    protected:                                                                 \
    QPointer<TYPE> m_##NAME = nullptr;                                         \
    public:                                                                    \
    QPointer<TYPE> NAME() const { return m_##NAME; }                           \
    public Q_SLOTS:                                                            \
    void set_##NAME(TYPE* new_value) {                                         \
        if (m_##NAME == new_value) return;                                     \
        m_##NAME = QPointer<TYPE>(new_value);                                  \
        Q_EMIT NAME##_changed();                                               \
}                                                                              \
    Q_SIGNALS:                                                                 \
    void NAME##_changed();                                                     \
    public:

#define QPOINTER_READONLY_PROPERTY(TYPE, NAME)                                 \
    Q_PROPERTY(TYPE* NAME READ NAME CONSTANT)                                  \
    public:                                                                    \
    QPointer<TYPE> NAME() const { return m_##NAME; }                           \
    private:                                                                   \
    QPointer<TYPE> m_##NAME;                                                   \
                                                                               \
    public:

//─── Helper methods ───────────────────────────────────────────────────────────────────

template<typename EnumType>
inline QStringList enum_to_stringlist() {
    QMetaEnum metaEnum = QMetaEnum::fromType<EnumType>();
    QStringList result;
    for (int i = 0; i < metaEnum.keyCount(); ++i) {
        result << metaEnum.key(i);
    }
    return result;
}
