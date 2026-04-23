#include "sun_module.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QRegularExpression>

namespace SolTrace::GUI::App {

SunModule::SunModule(QObject* parent)
    : QObject(parent),
      m_status(new StatusComponent()),
      m_shape(new SunShape()),
      m_ps_position(new SolarPositionData()),
      m_ds_position(new SolarPositionData()),
      m_calc_data(new SolarCalculatorData())

{
    connect(m_shape, &SunShape::changed, this, &SunModule::update_shape);
    connect(m_calc_data,
            &SolarCalculatorData::changed,
            this,
            &SunModule::update_position);

    connect(this, &SunModule::type_changed, this, &SunModule::update_type);

    update_type();
    update_position();
}


void SunModule::update_shape() {
    /* SD::ray_source_ptr ray_source = m_current_database->get_ray_source();
    if (!ray_source) return;

    ray_source->set_shape(m_shape->get_sunshape_data(),
                          m_shape->sigma(),
                          m_shape->half_width(),
                          m_shape->csr(),
                          m_shape->custom_distribution()->get_angle_data(),
                          m_shape->custom_distribution()->get_intensity_data());
    */
}

void SunModule::update_type() {
    if (m_type == Type::Directional) set_position(m_ds_position);
    else
        set_position(m_ps_position);
}

void SunModule::update_position() {
    if (!m_position->from_calculator()) return;

    /*SD::ray_source_ptr ray_source = m_current_database->get_ray_source();
    if (!ray_source) return;

    ray_source->set_position(
    m_dpos->get_datetime_data(), m_dpos->latitude(), m_dpos->longitude());
*/
    m_calculator.set_method(Data::SolarPositionCalculationMethod::LEGACY);

    m_calculator.set_location(m_calc_data->latitude(),
                              m_calc_data->longitude(),
                              m_calc_data->timezone_offset());

    m_calculator.set_date(
        m_calc_data->year(), m_calc_data->month(), m_calc_data->day());
    m_calculator.set_time(
        m_calc_data->hour(), m_calc_data->minute(), m_calc_data->second());

    double x, y, z;
    m_calculator.get_sun_vector(&x, &y, &z);
    m_position->set_x(x);
    m_position->set_y(y);
    m_position->set_z(z);
}

SunShape::SunShape(QObject* parent)
    : QObject(parent),
      m_generated_distribution(new SunShapeModel()),
      m_custom_distribution(new SunShapeModel()) {

    // SunShape::shape_changed() -> SunShape::update_current_distribution()
    connect(this,
            &SunShape::shape_changed,
            this,
            &SunShape::update_current_distribution);

    // SunShape::*_changed() -> SunShape::regenerate()
    connect(this, &SunShape::shape_changed, this, &SunShape::regenerate);
    connect(this, &SunShape::sigma_changed, this, &SunShape::regenerate);
    connect(this, &SunShape::half_width_changed, this, &SunShape::regenerate);
    connect(this, &SunShape::csr_changed, this, &SunShape::regenerate);
    connect(m_custom_distribution,
            &SunShapeModel::changed,
            this,
            &SunShape::update_x_axis);

    // SunShape::*_changed() -> SunShape::changed()
    connect(this, &SunShape::shape_changed, this, &SunShape::changed);
    connect(this, &SunShape::sigma_changed, this, &SunShape::changed);
    connect(this, &SunShape::half_width_changed, this, &SunShape::changed);
    connect(this, &SunShape::csr_changed, this, &SunShape::changed);
    connect(m_custom_distribution,
            &SunShapeModel::changed,
            this,
            &SunShape::changed);

    // Initialization
    regenerate();
    update_current_distribution();
}

void SunShape::reset_current_distribution() {
    custom_distribution()->clear();
    custom_distribution()->append(-1, 0);
    custom_distribution()->append(0, 0.6);
    custom_distribution()->append(1, 0);
}

void SunShape::regenerate() {
    m_generated_distribution->clear();
    switch (m_shape) {
    case Shape::Gaussian: sample_gaussian(); break;
    case Shape::Pillbox: sample_pillbox(); break;
    case Shape::Buie_CSR: sample_buie(); break;
    case Shape::Custom: break;
    }
    update_x_axis();
}

void SunShape::sample_gaussian() {
    // Point generation code referenced from app/src/sunshape
    // (SunShapeForm::UpdatePlot())

    auto&  model      = m_generated_distribution;
    int    num_points = 100;
    double theta_x    = -m_sigma * 3;
    double theta_inc  = 6 * m_sigma / num_points;

    for (int i = 0; i < num_points; i++) {
        model->append(theta_x,
                      1.0 / exp(theta_x * theta_x / (2 * m_sigma * m_sigma)));
        theta_x += theta_inc;
    }
}

void SunShape::sample_pillbox() {
    // Point generation code referenced from app/src/sunshape
    // (SunShapeForm::UpdatePlot())

    auto& model = m_generated_distribution;

    model->append(-m_half_width, 0);
    model->append(-m_half_width, 1);
    model->append(m_half_width, 1);
    model->append(m_half_width, 0);
}

void SunShape::sample_buie() {
    // TODO
}

void SunShape::update_x_axis() {
    QPointer<SunShapeModel> gdist = m_generated_distribution;
    QPointer<SunShapeModel> cdist = m_custom_distribution;

    switch (m_shape) {
    case Shape::Gaussian:
        gdist->set_x_axis_from(-3.3 * m_sigma);
        gdist->set_x_axis_to(3.3 * m_sigma);
        break;
    case Shape::Pillbox:
        gdist->set_x_axis_from(-3.3 * m_half_width);
        gdist->set_x_axis_to(3.3 * m_half_width);
        break;
    case Shape::Buie_CSR:
        // TODO: bounding logic
        break;
    case Shape::Custom:
        // Code referenced from app/src/sunshape (SunShapeForm::UpdatePlot())
        if (cdist->count() >= 2) {
            double min_x = 0;
            double max_x = 0;
            for (int i = 0; i < cdist->count(); i++) {
                double angle = cdist->get_at(i)->angle;
                if (angle > max_x) max_x = angle;
                if (angle < min_x) min_x = angle;
            }

            if (min_x == 0 && max_x == 0) {
                cdist->set_x_axis_from(-1.3);
                cdist->set_x_axis_to(1.3);
            } else {
                double extent = std::max(std::abs(min_x), std::abs(max_x));
                cdist->set_x_axis_from(-1.3 * extent);
                cdist->set_x_axis_to(1.3 * extent);
            }
        }
        break;
    }
}

void SunShape::update_current_distribution() {
    if (m_shape == Shape::Custom)
        set_current_distribution(m_custom_distribution);
    else
        set_current_distribution(m_generated_distribution);
}

SolarCalculatorData::SolarCalculatorData(QObject* parent) : QObject(parent) {
    connect(this,
            &SolarCalculatorData::calculator_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::latitude_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::longitude_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::year_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::month_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::day_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::hour_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::minute_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::second_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::timezone_offset_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::optional_solpos_fields_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::interval_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::optional_spa_fields_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::dut1_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::altitude_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::pressure_changed,
            this,
            &SolarCalculatorData::changed);
    connect(this,
            &SolarCalculatorData::temperature_changed,
            this,
            &SolarCalculatorData::changed);
}

DateTime SolarCalculatorData::get_datetime_data() const {
    return DateTime { }; // TODO: stub
}

SunShapeModel::SunShapeModel(QObject* parent) : StructTableModel(parent) {
    connect(this, &SunShapeModel::dataChanged, this, &SunShapeModel::changed);
    connect(this, &SunShapeModel::rowsInserted, this, &SunShapeModel::changed);
    connect(this, &SunShapeModel::rowsRemoved, this, &SunShapeModel::changed);
    connect(this, &SunShapeModel::modelReset, this, &SunShapeModel::changed);
}

std::vector<double> SunShapeModel::get_angle_data() {
    std::vector<double> result;
    for (const auto& point : m_records) {
        result.push_back(point.angle);
    }
    return result;
}

std::vector<double> SunShapeModel::get_intensity_data() {
    std::vector<double> result;
    for (const auto& point : m_records) {
        result.push_back(point.intensity);
    }
    return result;
}

QVariantList SunShapeModel::variant_data() {
    QVariantList custom_shape;
    for (int i = 0; i < count(); i++) {
        QVariantMap point;
        point["angle"]     = get_at(i)->angle;
        point["intensity"] = get_at(i)->intensity;
        custom_shape.append(point);
    }
    return custom_shape;
}

void SunShapeModel::set_variant_data(QVariantList data) {
    for (const auto& item : data) {
        QVariantMap point = item.toMap();
        append(point["angle"].toDouble(), point["intensity"].toDouble());
    }
}

int SunShapeModel::count() const {
    return rowCount();
}

void SunShapeModel::append(double angle, double intensity) {
    StructTableModel::append({ angle, intensity });
    emit countChanged();
}

void SunShapeModel::remove(int index) {
    if (index < 0 || index >= m_records.count()) return;
    remove_at(index);
    emit countChanged();
}

void SunShapeModel::clear() {
    if (m_records.isEmpty()) return;
    reset();
    emit countChanged();
}

void SunShapeModel::copy_to_clipboard() {
    QString text = "Angle (mrad)\tIntensity\n";
    for (int i = 0; i < m_records.count(); i++) {
        text += QString::number(m_records[i].angle) + "\t" +
                QString::number(m_records[i].intensity) + "\n";
    }
    QGuiApplication::clipboard()->setText(text);
}

void SunShapeModel::paste_from_clipboard() {
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
    // setData(rows);
}

Data::SunShape SunShape::get_sunshape_data() const {
    switch (m_shape) {
    case Shape::Gaussian: return Data::SunShape::GAUSSIAN;
    case Shape::Pillbox: return Data::SunShape::PILLBOX;
    case Shape::Buie_CSR: return Data::SunShape::BUIE_CSR;
    case Shape::Custom: return Data::SunShape::USER_DEFINED;
    default: return Data::SunShape::UNKNOWN;
    }
}

SolarPositionData::SolarPositionData(QObject* parent) {
    connect(
        this, &SolarPositionData::x_changed, this, &SolarPositionData::changed);
    connect(
        this, &SolarPositionData::y_changed, this, &SolarPositionData::changed);
    connect(
        this, &SolarPositionData::z_changed, this, &SolarPositionData::changed);
    connect(this,
            &SolarPositionData::from_calculator_changed,
            this,
            &SolarPositionData::changed);
}


} // namespace SolTrace::GUI::App
