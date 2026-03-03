#pragma once
#include <QObject>
#include "utilities/qt_helpers.h"
#include "module_common.h"
#include "backend.h"

namespace SolTrace::GUI::App {

class CustomSunShapeModel : public QAbstractListModel {
    Q_OBJECT
public:
    explicit CustomSunShapeModel(QObject* parent = nullptr);

    enum Roles {
        AngleRole = Qt::UserRole + 1,
        IntensityRole
    };

    struct Point {
        double angle = 0.0;
        double intensity = 0.0;
    };

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index,
    int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    Q_INVOKABLE void append(double angle = 0.0, double intensity = 0.0);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantList getData() const;
    Q_INVOKABLE void setData(const QVariantList &data);

    Q_INVOKABLE void copy_to_clipboard();
    Q_INVOKABLE void paste_from_clipboard();

    int count() const;

signals:
    void countChanged();

private:
    QVector<Point> m_points;
};

class SunDefinition : public QObject {
    Q_OBJECT
public:
    explicit SunDefinition(QObject* parent = nullptr);

    enum class SunType { Directional, PointSource };
    enum class SunShape { Gaussian, Pillbox, CSR, Custom };

    Q_ENUM(SunType)
    Q_ENUM(SunShape)

    Q_WRITABLE_PROPERTY(SunType, sun_type, SunType::Directional)
    Q_WRITABLE_PROPERTY(SunShape, sun_shape, SunShape::Gaussian)

    // Gaussian
    Q_WRITABLE_PROPERTY(double, std, 5.18)

    // Pillbox
    Q_WRITABLE_PROPERTY(double, half_width, 4.65)

    // Buie
    Q_WRITABLE_PROPERTY(double, csr, 2.0)

    // Custom Sun Shape
    Q_WRITABLE_PROPERTY(int, num_points, 1)

    QOBJECT_WRITABLE_PROPERTY(CustomSunShapeModel, custom_shape)
};

class DirectionalSunPosition : public QObject {
    Q_OBJECT

public:
    explicit DirectionalSunPosition(QObject* parent = nullptr);

    enum class PositionCalculator { Legacy, Duffie, SOLPOS, SPA };

    Q_ENUM(PositionCalculator)

    Q_WRITABLE_PROPERTY(PositionCalculator,
                        position_calculator,
                        PositionCalculator::Legacy)

    // Position
    Q_WRITABLE_PROPERTY(double, latitude, 35.04)
    Q_WRITABLE_PROPERTY(double, longitude, -105.10)

    // Date
    Q_WRITABLE_PROPERTY(int, year, 2026)
    Q_WRITABLE_PROPERTY(int, month, 12)
    Q_WRITABLE_PROPERTY(int, day, 25)

    // Time
    Q_WRITABLE_PROPERTY(int, hour, 14)
    Q_WRITABLE_PROPERTY(int, minute, 30)
    Q_WRITABLE_PROPERTY(int, second, 0)
    Q_WRITABLE_PROPERTY(int, timezone, -7)

    // SOLPOS
    Q_WRITABLE_PROPERTY(int, interval, 1) ///< Averaging interval in seconds

    // SPA Optional fields
    Q_WRITABLE_PROPERTY(bool, optional_spa_fields, false)
    Q_WRITABLE_PROPERTY(double, dut1, 0.0)
    Q_WRITABLE_PROPERTY(double, altitude, 1000)
    Q_WRITABLE_PROPERTY(double, pressure, 1013.25)
    Q_WRITABLE_PROPERTY(double, temperature, 20.0)
};


class PointSourcePosition : public QObject {
    Q_OBJECT

public:
    explicit PointSourcePosition(QObject* parent = nullptr);

    Q_WRITABLE_PROPERTY(double, x, 1000)
    Q_WRITABLE_PROPERTY(double, y, 1000)
    Q_WRITABLE_PROPERTY(double, z, 1000)
};


class SunModule : public QObject {
    Q_OBJECT

public:
    explicit SunModule(QObject* parent = nullptr);

    QPOINTER_WRITABLE_PROPERTY(SunBackend, backend)
    QOBJECT_READONLY_PROPERTY(StatusComponent, status)
    QOBJECT_READONLY_PROPERTY(CustomSunShapeModel, custom_sun_shape)
    QOBJECT_READONLY_PROPERTY(PresetComponent<SunDefinition>, definition)
    QOBJECT_READONLY_PROPERTY(PresetComponent<DirectionalSunPosition>,
                              ds_positions)
    QOBJECT_READONLY_PROPERTY(PresetComponent<PointSourcePosition>,
                              ps_positions)

};
} // namespace SolTrace::GUI::App


