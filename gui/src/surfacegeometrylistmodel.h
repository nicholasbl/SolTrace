#pragma once

#include "surfacegeometry.h"
#include <QAbstractListModel>

class SurfaceGeometryListModel : public QAbstractListModel {
    Q_OBJECT

    QVector<std::shared_ptr<SurfaceGeometry>> m_geoms;

public:
    enum Roles {
        GeometryRole = Qt::UserRole + 1,
        PositionRole,
        RotationRole,
        LabelRole,
        VisibleRole,
        EulerRole,
        ZRotateRole,
        AimRole
    };

    SurfaceGeometryListModel(QObject* parent = nullptr);

    void setGeometries(QVector<std::shared_ptr<SurfaceGeometry>> const& geoms) {
        beginResetModel();
        m_geoms = geoms;
        endResetModel();
    }

    void push_back(std::shared_ptr<SurfaceGeometry> const& surface) {
        auto at = rowCount();

        beginInsertRows(QModelIndex(), at, at);
        m_geoms.push_back(surface);
        endInsertRows();
    }

    int rowCount(QModelIndex const& parent = QModelIndex()) const override {
        return m_geoms.size();
    }

    QVariant data(QModelIndex const& idx,
                  int                role = Qt::DisplayRole) const override;

    bool
    setData(QModelIndex const& index, QVariant const& value, int role) override;

    Qt::ItemFlags          flags(QModelIndex const& index) const override;
    QHash<int, QByteArray> roleNames() const override;
};

