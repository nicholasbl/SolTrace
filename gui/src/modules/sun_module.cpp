#include "sun_module.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QRegularExpression>

namespace SolTrace::GUI::App {

SunModule::SunModule(QObject* parent)
    : QObject(parent),
      m_backend(nullptr),
      m_status(new StatusComponent()),
      m_custom_sun_shape(new CustomSunShapeModel(this)),
      m_definition(new SunDefinition)
// m_ds_positions(new PresetComponent<DirectionalSunPosition>()),
//  m_ps_positions(new PresetComponent<PointSourcePosition>()
{ }

SunDefinition::SunDefinition(QObject* parent)
    : QObject(parent), m_custom_shape(new CustomSunShapeModel(this)) { }

DirectionalSunPosition::DirectionalSunPosition(QObject* parent)
    : QObject(parent) { }

PointSourcePosition::PointSourcePosition(QObject* parent) : QObject(parent) { }

CustomSunShapeModel::CustomSunShapeModel(QObject* parent)
    : StructTableModel(parent) { }

int CustomSunShapeModel::count() const {
    return rowCount();
}

void CustomSunShapeModel::append(double angle, double intensity) {
    StructTableModel::append({ angle, intensity });
    emit countChanged();
}

void CustomSunShapeModel::remove(int index) {
    if (index < 0 || index >= m_records.count()) return;
    remove_at(index);
    emit countChanged();
}

void CustomSunShapeModel::clear() {
    if (m_records.isEmpty()) return;
    reset();
    emit countChanged();
}

QVariantList CustomSunShapeModel::getData() const {
    QVariantList points;
    for (const auto& point : m_records) {
        QVariantMap map;
        map["angle"]     = point.angle;
        map["intensity"] = point.intensity;
        points.append(map);
    }
    return points;
}

void CustomSunShapeModel::setData(const QVariantList& data) {
    QVector<SunShapePoint> new_points;
    for (const QVariant& item : data) {
        QVariantMap map = item.toMap();
        new_points.append(
            { map["angle"].toDouble(), map["intensity"].toDouble() });
    }
    reset(new_points);
    emit countChanged();
}

void CustomSunShapeModel::copy_to_clipboard() {
    QString text = "Angle (mrad)\tIntensity\n";
    for (int i = 0; i < m_records.count(); i++) {
        text += QString::number(m_records[i].angle) + "\t" +
                QString::number(m_records[i].intensity) + "\n";
    }
    QGuiApplication::clipboard()->setText(text);
}

void CustomSunShapeModel::paste_from_clipboard() {
    QVariantList rows;
    QString      text = QGuiApplication::clipboard()->text();
    for (QString line : text.split('\n')) {
        if (line.trimmed() == "") continue;
        QStringList v = line.split(QRegularExpression("[\\t,]"));
        if (v.length() >= 2) {
            QVariantMap row;
            row["angle"]     = v[0].trimmed().toDouble();
            row["intensity"] = v[1].trimmed().toDouble();
            rows.append(row);
        }
    }
    setData(rows);
}

} // namespace SolTrace::GUI::App
