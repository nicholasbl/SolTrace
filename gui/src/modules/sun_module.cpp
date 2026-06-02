#include "sun_module.h"
#include <QClipboard>
#include <QGuiApplication>
#include <QRegularExpression>
#include <algorithm>
#include <cmath>

namespace SolTrace::GUI::App {

namespace {

QVector<SunShapePoint> normalized_radial_points(QVector<SunShapePoint> points) {
    for (auto& point : points) {
        point.angle = std::abs(point.angle);
    }

    std::sort(points.begin(), points.end(), [](auto const& a, auto const& b) {
        return a.angle < b.angle;
    });

    QVector<SunShapePoint> merged;
    for (auto const& point : points) {
        if (!merged.empty() &&
            std::abs(merged.back().angle - point.angle) < 1.0e-9) {
            merged.back().intensity =
                std::max(merged.back().intensity, point.intensity);
        } else {
            merged.push_back(point);
        }
    }

    return merged;
}

QVector<SunShapePoint> mirrored_radial_points(QVector<SunShapePoint> points) {
    auto radial_points = normalized_radial_points(std::move(points));
    if (radial_points.empty()) return {};

    QVector<SunShapePoint> mirrored;
    mirrored.reserve(radial_points.size() * 2 - 1);

    for (auto it = radial_points.crbegin(); it != radial_points.crend(); ++it) {
        if (it->angle <= 1.0e-9) continue;
        mirrored.push_back({
            .angle     = -it->angle,
            .intensity = it->intensity,
        });
    }

    mirrored.append(radial_points);
    return mirrored;
}

} // namespace

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
    custom_distribution()->append(0, 1);
    custom_distribution()->append(1, 0.9);
    custom_distribution()->append(2, 0);
}

void SunShape::regenerate() {
    m_generated_distribution->clear();
    switch (m_shape) {
    case Shape::Gaussian: sample_gaussian(); break;
    case Shape::Pillbox: sample_pillbox(); break;
    case Shape::Buie_CSR: sample_buie(); break;
    case Shape::Custom: break;
    case Shape::LimbDarkened: sample_limb_darkened(); break;
    }
    update_x_axis();
}

void SunShape::sample_gaussian() {
    // Point generation code referenced from app/src/sunshape
    // (SunShapeForm::UpdatePlot())

    auto&  model      = m_generated_distribution;
    int    num_points = 100;
    double theta_x    = 0;
    double theta_inc  = 3 * m_sigma / num_points;

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

    model->append(0, 1);
    model->append(m_half_width, 1);
    model->append(m_half_width, 0);
}

void SunShape::sample_buie() {
    if (m_csr <= 0.0 || m_csr > 0.8) return;

    auto& model = m_generated_distribution;

    model->clear();

    double csr = m_csr;
    double chi;
    if (csr > 0.145)
        chi = -0.04419909985804843 +
              csr * (1.401323894233574 +
                     csr * (-0.3639746714505299 +
                            csr * (-0.9579768560161194 +
                                   1.1550475450828657 * csr)));
    else if (csr > 0.035)
        chi = 0.022652077593662934 +
              csr * (0.5252380349996234 +
                     (2.5484334534423887 - 0.8763755326550412 * csr) * csr);
    else
        chi = 0.004733749294807862 +
              csr * (4.716738065192151 +
                     csr * (-463.506669149804 +
                            csr * (24745.88727411664 +
                                   csr * (-606122.7511711778 +
                                          5521693.445014727 * csr))));

    double kappa         = 0.9 * log(13.5 * chi) * pow(chi, -0.3);
    double gamma         = 2.2 * log(0.52 * chi) * pow(chi, 0.43) - 0.1;
    double diskEdgeValue = cos(0.326 * 4.65) / cos(0.308 * 4.65);

    for (double theta = 0.0; theta <= 43.6; theta += 0.01) {
        double absTheta = theta;
        double intensity;
        if (absTheta <= 4.65)
            intensity = cos(0.326 * absTheta) / cos(0.308 * absTheta);
        else {
            intensity = exp(kappa) * pow(absTheta, gamma);
            intensity = std::min(intensity, diskEdgeValue);
        }
        model->append(theta, intensity);
    }
}

void SunShape::sample_limb_darkened() {
    auto& model = m_generated_distribution;

    constexpr double disk_edge = 4.65;
    constexpr int    num_points = 100;
    double           theta = 0.0;
    double           theta_inc = disk_edge / num_points;

    for (int i = 0; i <= num_points; ++i) {
        model->append(theta, std::cos(0.326 * theta) / std::cos(0.308 * theta));
        theta += theta_inc;
    }
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
        gdist->set_x_axis_from(-20.0);
        gdist->set_x_axis_to(20.0);
        break;
    case Shape::LimbDarkened:
        gdist->set_x_axis_from(-1.3 * 4.65);
        gdist->set_x_axis_to(1.3 * 4.65);
        break;
    case Shape::Custom:
        // Code referenced from app/src/sunshape (SunShapeForm::UpdatePlot())
        if (cdist->count() >= 2) {
            double max_x = 0;
            for (int i = 0; i < cdist->count(); i++) {
                double angle = std::abs(cdist->get_at(i)->angle);
                if (angle > max_x) max_x = angle;
            }

            if (max_x == 0) {
                cdist->set_x_axis_from(-1.3);
                cdist->set_x_axis_to(1.3);
            } else {
                cdist->set_x_axis_from(-1.3 * max_x);
                cdist->set_x_axis_to(1.3 * max_x);
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
    auto points = mirrored_radial_points(m_records);

    std::vector<double> result;
    for (const auto& point : points) {
        result.push_back(point.angle);
    }
    return result;
}

std::vector<double> SunShapeModel::get_intensity_data() {
    auto points = mirrored_radial_points(m_records);

    std::vector<double> result;
    for (const auto& point : points) {
        result.push_back(point.intensity);
    }
    return result;
}

QVariantList SunShapeModel::variant_data() {
    auto points = normalized_radial_points(m_records);

    QVariantList custom_shape;
    for (auto const& source : points) {
        QVariantMap point;
        point["angle"]     = source.angle;
        point["intensity"] = source.intensity;
        custom_shape.append(point);
    }
    return custom_shape;
}

void SunShapeModel::set_variant_data(QVariantList data) {
    clear();
    QVector<SunShapePoint> points;
    for (const auto& item : data) {
        QVariantMap point = item.toMap();
        points.push_back({
            .angle     = std::abs(point["angle"].toDouble()),
            .intensity = point["intensity"].toDouble(),
        });
    }
    points = normalized_radial_points(points);
    StructTableModel<SunShapePoint>::append(points);
    emit countChanged();
}

int SunShapeModel::count() const {
    return rowCount();
}

void SunShapeModel::append(double angle, double intensity) {
    StructTableModel::append({ std::abs(angle), intensity });
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
    auto points = normalized_radial_points(m_records);

    for (auto const& point : points) {
        text += QString::number(point.angle) + "\t" +
                QString::number(point.intensity) + "\n";
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
    set_variant_data(rows);
}

Data::SunShape SunShape::get_sunshape_data() const {
    switch (m_shape) {
    case Shape::Gaussian: return Data::SunShape::GAUSSIAN;
    case Shape::Pillbox: return Data::SunShape::PILLBOX;
    case Shape::Buie_CSR: return Data::SunShape::BUIE_CSR;
    case Shape::Custom: return Data::SunShape::USER_DEFINED;
    case Shape::LimbDarkened: return Data::SunShape::LIMBDARKENED;
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
