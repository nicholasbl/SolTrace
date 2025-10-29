#include "surfacegeometrylistmodel.h"


SurfaceGeometryListModel::SurfaceGeometryListModel(QObject* parent)
    : QAbstractListModel { parent } { }

QVariant SurfaceGeometryListModel::data(const QModelIndex& idx, int role) const
{
    if (!idx.isValid())
        return {};

    const auto &geom = m_geoms[idx.row()];

    switch (role) {
    case GeometryRole:
        return QVariant::fromValue(m_geoms[idx.row()].get());
    case PositionRole:
        return QVariant::fromValue(m_geoms[idx.row()]->position());
    case RotationRole:
        return QVariant::fromValue(m_geoms[idx.row()]->eulerAngles());
    case VisibleRole:
        return QVariant::fromValue(m_geoms[idx.row()]->visible());
    case LabelRole:
        qDebug() << m_geoms[idx.row()]->label();
        return QVariant::fromValue(m_geoms[idx.row()]->label());
    default:
        return {};
    }
}

bool SurfaceGeometryListModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
        return false;

    auto &entry = m_geoms[index.row()];

    switch (role) {
    case VisibleRole:
        entry->setVisible(value.toBool());
        emit dataChanged(index, index, { VisibleRole });
        return true;
    default:
        return false;
    }
}

Qt::ItemFlags SurfaceGeometryListModel::flags(const QModelIndex &index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> SurfaceGeometryListModel::roleNames() const {
    return {
        { GeometryRole, "geometry" },
        { PositionRole, "position" },
        { RotationRole, "rotation" },
        { VisibleRole,  "visible"  },
        { LabelRole,    "label"    }
    };
}
