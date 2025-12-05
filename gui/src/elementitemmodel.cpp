#include "elementitemmodel.h"
#include "element.hpp"

ElementItemModel::ElementItemModel(QObject* parent) : QAbstractItemModel(parent)
{
}

void ElementItemModel::addElements(const std::shared_ptr<SD::SimulationData> &data)
{
    for (auto iter = data->get_iterator();!data->is_at_end(iter); iter++)
    {
        auto element = iter->second;

        if (element->get_reference_element() == nullptr) // Only top-level elements
        {
            auto item = std::make_shared<ElementItem>(nullptr,element);

            beginInsertRows(QModelIndex(), m_items.size(), m_items.size());

            m_items.push_back(item);

            endInsertRows();

            QModelIndex root = index(m_items.size()-1, 0);

            auto composite = std::dynamic_pointer_cast<SD::CompositeElement>(element);

            if (composite) {
                for (auto child_iter = composite->get_iterator(); !composite->is_at_end(child_iter); child_iter++) {
                    auto childItem = std::make_shared<ElementItem>(item,child_iter->second);

                    addChildElement(root, childItem);
                }
            }

            // todo: determine if stages need to be handled here too.
        }
    }
}

void ElementItemModel::addChildElement(const QModelIndex& parent, std::shared_ptr<ElementItem> item)
{
    auto parentItem = item->parent();

    beginInsertRows(parent, parentItem->children().size(), parentItem->children().size());

    parentItem->children().push_back(item);

    endInsertRows();

    auto root = index(parentItem->children().size()-1, 0, parent);

    auto composite = std::dynamic_pointer_cast<SD::CompositeElement>(item->element());

    if (composite) {
        for (auto child_iter = composite->get_iterator(); !composite->is_at_end(child_iter); child_iter++) {
            auto childItem = std::make_shared<ElementItem>(item,child_iter->second);

            addChildElement(root, childItem);
        }
    }
}

QModelIndex ElementItemModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!parent.isValid()){
        return createIndex(row, column, m_items[row].get());
    }

    ElementItem* parentItem = static_cast<ElementItem*>(parent.internalPointer()); // gross

    if (parentItem->children().size() > row){
        return createIndex(row, column, parentItem->children()[row].get());
    }

    return QModelIndex();
}

QModelIndex ElementItemModel::parent(const QModelIndex& index) const
{

    if (!index.isValid())
    {
        return QModelIndex();
    }

    auto item = static_cast<ElementItem*>(index.internalPointer()); // gross
    auto parentItem = item->parent();

    if (!parentItem)
    {
        return QModelIndex();
    }

    auto grandParent = parentItem->parent();
    int row = 0;

    if (grandParent) {
        auto it = std::find_if(grandParent->children().begin(), grandParent->children().end(), [&](const std::shared_ptr<ElementItem>& p) {
                return p.get() == parentItem.get();
            });

        if (it != grandParent->children().end()) {
            row = std::distance(grandParent->children().begin(), it);
        }
    }

    return createIndex(row, 0, parentItem.get());
}

bool ElementItemModel::hasChildren(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return !m_items.empty();
    }

    auto item = static_cast<ElementItem*>(parent.internalPointer());
    return !item->children().empty();
}

int ElementItemModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return m_items.size();

    auto item = static_cast<ElementItem*>(parent.internalPointer());

    return item->children().size();
}

QVariant ElementItemModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    auto item = static_cast<ElementItem*>(index.internalPointer());

    switch (role) {
    case GeometryRole:
        return QVariant::fromValue(item->surface().get());
    case PositionRole:
        return QVariant::fromValue(item->surface()->position());
    case RotationRole:
        return QVariant::fromValue(item->surface()->eulerAngles());
    case VisibleRole:
        return QVariant::fromValue(item->surface()->visible());
    case SurfaceLabelRole:
        return QVariant::fromValue(item->surface()->surfaceType());
    case ApertureLabelRole:
        return QVariant::fromValue(item->surface()->apertureType());
    case ParentRole:
        return QVariant::fromValue(item->children().size());

    default:
        return {};
    }

}

bool ElementItemModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;

    auto item = static_cast<ElementItem*>(index.internalPointer());

    switch (role) {
    case VisibleRole:
        item->surface()->setVisible(value.toBool());
        emit dataChanged(index, index, { VisibleRole });
        return true;
    default:
        return false;
    }
}

Qt::ItemFlags ElementItemModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

QHash<int, QByteArray> ElementItemModel::roleNames() const
{
    return {
        { GeometryRole, "geometry" },
        { PositionRole, "position" },
        { RotationRole, "rotation" },
        { VisibleRole,  "visible"  },
        { SurfaceLabelRole,    "surfaceLabel"    },
        { ApertureLabelRole,   "apertureLabel"    },

        { ParentRole,   "nChildren" }
    };
}

ElementItemModel::ElementItem::ElementItem(std::shared_ptr<ElementItem> p, std::shared_ptr<SD::Element> e) :
    m_parent(p),
    m_element(e),
    m_surface(nullptr)
{
    m_surface = std::make_shared<SurfaceGeometry>(m_element);
}


