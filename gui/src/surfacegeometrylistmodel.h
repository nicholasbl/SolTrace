#ifndef ELEMENTLISTMODEL_H
#define ELEMENTLISTMODEL_H

#include <QAbstractListModel>
#include "surfacegeometry.h"

class SurfaceGeometryListModel : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles { GeometryRole = Qt::UserRole + 1,
                 PositionRole,
                 RotationRole,
                 LabelRole,
                 VisibleRole
                 };

    SurfaceGeometryListModel(QObject* parent = nullptr);

    void setGeometries(const QVector<std::shared_ptr<SurfaceGeometry>>& geoms) {
        beginResetModel();
        m_geoms = geoms;
        endResetModel();
    }

    void push_back(const std::shared_ptr<SurfaceGeometry>& surface) {
        m_geoms.push_back(surface);
    }

    int rowCount(const QModelIndex&) const override {
        return m_geoms.size();
    }

    QVariant data(const QModelIndex& idx, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<std::shared_ptr<SurfaceGeometry>> m_geoms;
};

#endif // ELEMENTLISTMODEL_H
