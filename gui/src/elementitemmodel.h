#pragma once

#include <QAbstractItemModel>
#include <memory>
#include "element.hpp"
#include "simulation_data_api.hpp"
#include "surfacegeometry.h"

namespace SD = SolTrace::Data;



class ElementItemModel : public QAbstractItemModel {
    Q_OBJECT
public:
    enum Roles {
        GeometryRole = Qt::UserRole + 1,
        PositionRole,
        RotationRole,
        SurfaceLabelRole,
        ApertureLabelRole,
        VisibleRole,
        ParentRole
    };

    class ElementItem {
        std::shared_ptr<ElementItem> m_parent;
        std::shared_ptr<SD::Element> m_element;
        std::vector<std::shared_ptr<ElementItem>> m_children;

        std::shared_ptr<SurfaceGeometry> m_surface;

    public:
        ElementItem(std::shared_ptr<ElementItem> p, std::shared_ptr<SD::Element> e);

        std::shared_ptr<ElementItem> parent() { return m_parent; }
        std::shared_ptr<SD::Element> element() { return m_element; }
        std::vector<std::shared_ptr<ElementItem>>& children() { return m_children; }
        std::shared_ptr<SurfaceGeometry> surface() { return m_surface; }
    };

    explicit ElementItemModel(QObject* parent = nullptr);

    void addElements(const std::shared_ptr<SD::SimulationData> &data);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override { Q_UNUSED(parent); return 2; }

    bool hasChildren(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;

    const std::vector<std::shared_ptr<ElementItemModel::ElementItem>>& items() { return m_items; }

private:
    void addChildElement(const QModelIndex& parent, std::shared_ptr<ElementItem> item);

    std::vector<std::shared_ptr<ElementItem>> m_items;
};
