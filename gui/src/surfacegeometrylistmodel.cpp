#include "surfacegeometrylistmodel.h"
#include <QQuaternion>


SurfaceGeometryListModel::SurfaceGeometryListModel(QObject* parent)
    : QAbstractListModel { parent } { }

QVariant SurfaceGeometryListModel::data(QModelIndex const& idx,
                                        int                role) const {
    if (!idx.isValid())
        return {};

    const auto &geom = m_geoms[idx.row()];

    switch (role) {
    case GeometryRole:
        return QVariant::fromValue(m_geoms[idx.row()].get());
    case PositionRole:
        return QVariant::fromValue(m_geoms[idx.row()]->position());
    case AimRole:
        return QVariant::fromValue(m_geoms[idx.row()]->aim());
    case EulerRole:
        return QVariant::fromValue(m_geoms[idx.row()]->eulerAngles());
    case RotationRole:
        return QVariant::fromValue(QQuaternion::fromEulerAngles(m_geoms[idx.row()]->eulerAngles()));
    case VisibleRole:
        return QVariant::fromValue(m_geoms[idx.row()]->visible());
    case LabelRole:
        return QVariant::fromValue(m_geoms[idx.row()]->surfaceType());
    default:
        return {};
    }
}

bool SurfaceGeometryListModel::setData(QModelIndex const& index,
                                       QVariant const&    value,
                                       int                role) {
    if (!index.isValid())
        return false;

    auto &entry = m_geoms[index.row()];

    switch (role) {
    case VisibleRole:
        entry->setVisible(value.toBool());
        emit dataChanged(index, index, { VisibleRole });
        return true;
    case ZRotateRole:
        qDebug() << "ZRotateRole";
        entry->setZRotation(value.toFloat());
        emit dataChanged(index, index, { EulerRole });
        emit dataChanged(index, index, { RotationRole });
        return true;
    case AimRole:
        entry->setAim(value.value<QVector3D>());
        emit dataChanged(index, index, { EulerRole });
        emit dataChanged(index, index, { RotationRole });
        return true;
    default:
        return false;
    }
}

Qt::ItemFlags SurfaceGeometryListModel::flags(QModelIndex const& index) const {
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> SurfaceGeometryListModel::roleNames() const {

    static QHash<int, QByteArray> roles = { { GeometryRole, "geometry" },
                                            { PositionRole, "position" },
                                            { RotationRole, "rotation" },
                                            { VisibleRole, "visible" },
                                            { EulerRole,    "euler" },
                                            { VisibleRole,  "visible"  },
                                            { LabelRole,    "label"    },
                                            { ZRotateRole,  "zrot"     },
                                            { AimRole,      "aim"      }};

    return roles;
}

