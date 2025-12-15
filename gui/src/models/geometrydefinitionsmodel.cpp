#include "geometrydefinitionsmodel.h"

#include <QQuaternion>

void GeometryDefinitionsModel::_append_new(QVariant) { }

GeometryDefinitionsModel::GeometryDefinitionsModel(QObject* parent)
    : ListContainerModel(parent) {
    add_properties({
        {
            .display_name = "geometry",
            .getter       = [this](size_t index) -> QVariant {
                return QVariant::fromValue(m_items[index].get());
            },
        },
        {
            .display_name = "position",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->position();
            },
        },
        {
            .display_name = "aim",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->position();
            },
            .setter = [this](size_t index, QVariant value) -> bool {
                if (value.canConvert<QVector3D>()) {
                    m_items[index]->setAim(value.value<QVector3D>());
                    return true;
                }
                return false;
            },
        },
        {
            .display_name = "label",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->surfaceType();
            },
        },
        {
            .display_name = "visible",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->visible();
            },
            .setter = [this](size_t index, QVariant value) -> bool {
                m_items[index]->setVisible(value.toBool());
                return true;
            },
        },
        {
            .display_name = "euler",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->eulerAngles();
            },
        },
        {
            .display_name = "rotation",
            .getter       = [this](size_t index) -> QVariant {
                return QQuaternion::fromEulerAngles(
                    m_items[index]->eulerAngles());
            },
        },
        {
            .display_name = "z_angle",
            .getter       = [this](size_t index) -> QVariant {
                return m_items[index]->eulerAngles().z();
            },
            .setter = [this](size_t index, QVariant value) -> bool {
                m_items[index]->setZRotation(value.toFloat());
                return true;
            },
        },
    });
}
