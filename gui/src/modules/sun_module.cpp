#include "sun_module.h"
#include <QGuiApplication>
#include <QClipboard>
#include <QRegularExpression>

namespace SolTrace::GUI::App {
SunModule::SunModule(QObject *parent) :
    QObject(parent),
    m_backend(nullptr),
    m_status(new StatusComponent()),
    m_definition(new PresetComponent<SunDefinition>()),
    m_ds_positions(new PresetComponent<DirectionalSunPosition>()),
    m_ps_positions(new PresetComponent<PointSourcePosition>())
{}


SunDefinition::SunDefinition(QObject *parent) :
    QObject(parent),
    m_custom_shape(new CustomSunShapeModel(this))
{}

DirectionalSunPosition::DirectionalSunPosition(QObject *parent) :
    QObject(parent) {}

PointSourcePosition::PointSourcePosition(QObject *parent) :
    QObject(parent) {}

CustomSunShapeModel::CustomSunShapeModel(QObject *parent)
    : QAbstractListModel(parent) {}

int CustomSunShapeModel::count() const
{
    return m_points.count();
}

int CustomSunShapeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) return 0;
    return m_points.count();
}

QVariant CustomSunShapeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_points.size())
        return {};

    const Point& point = m_points[index.row()];

    switch (role) {
    case AngleRole: return point.angle;
    case IntensityRole: return point.intensity;
    default: return {};
    }
}

bool CustomSunShapeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_points.size())
        return false;

    Point& point = m_points[index.row()];

    switch (role) {
    case AngleRole: point.angle = value.toDouble(); break;
    case IntensityRole: point.intensity = value.toDouble(); break;
    default: return false;
    }

    emit dataChanged(index, index, {role});
    return true;
}

QHash<int, QByteArray> CustomSunShapeModel::roleNames() const
{
    return {
        { AngleRole, "angle"},
        { IntensityRole, "intensity"}
    };
}

Qt::ItemFlags CustomSunShapeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

void CustomSunShapeModel::append(double angle, double intensity)
{
    const int row = m_points.size();
    beginInsertRows({}, row, row);
    m_points.append(Point(angle, intensity));
    endInsertRows();
    emit countChanged();
}

void CustomSunShapeModel::remove(int index)
{
    if (index < 0 || index >= m_points.count()) return;
    beginRemoveRows({}, index, index);
    m_points.removeAt(index);
    endRemoveRows();
    emit countChanged();
}

void CustomSunShapeModel::clear()
{
    if (m_points.isEmpty()) return;
    beginResetModel();
    m_points.clear();
    endResetModel();
    emit countChanged();
}

QVariantList CustomSunShapeModel::getData() const
{
    QVariantList points;
    for (const Point& point : m_points) {
        QVariantMap map;
        map["angle"] = point.angle;
        map["intensity"] = point.intensity;
        points.append(map);
    }
    return points;
}

void CustomSunShapeModel::setData(const QVariantList &data)
{
    beginResetModel();
    m_points.clear();
    for (const QVariant &item : data) {
        QVariantMap map = item.toMap();
        m_points.append({ map["angle"].toDouble(), map["intensity"].toDouble() });
    }
    endResetModel();
    emit countChanged();
}

void CustomSunShapeModel::copy_to_clipboard()
{
    QString text = "Angle (mrad)\tIntensity\n";
    for (int i = 0; i < m_points.count(); i++) {
        text += QString::number(m_points[i].angle) + "\t" + QString::number(m_points[i].intensity) + "\n";
    }
    QGuiApplication::clipboard()->setText(text);
}

void CustomSunShapeModel::paste_from_clipboard()
{
    QVariantList rows;
    QString text = QGuiApplication::clipboard()->text();
    for (QString line : text.split('\n')) {
        if (line.trimmed() == "") continue;
        QStringList v = line.split(QRegularExpression("[\\t,]"));
        if (v.length() >= 2) {
            QVariantMap row;
            row["angle"] = v[0].trimmed().toDouble();
            row["intensity"] = v[1].trimmed().toDouble();
            rows.append(row);
        }
    }
    setData(rows);
}

} // namespace SolTrace::GUI::App
