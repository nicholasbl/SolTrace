#include "opticaleditor.h"

namespace db {

OpticalPropertiesObject::OpticalPropertiesObject(bool back, QObject* parent)
    : QObject(parent), m_current_group(entt::null), m_back(back) { }

OpticalPropertiesObject::~OpticalPropertiesObject() = default;

void OpticalPropertiesObject::trigger_all_changed() {
    qDebug() << Q_FUNC_INFO << m_back;
    emit interaction_type_changed();
    emit error_distribution_type_changed();
    emit transmitivity_changed();
    emit reflectivity_changed();
    emit slope_error_changed();
    emit specularity_error_changed();
    emit refraction_index_front_changed();
    emit refraction_index_back_changed();
}

void OpticalPropertiesObject::set(Database* db, entt::entity group) {
    observe(db);

    if (!db->material_parameters.get(group)) {
        m_current_group = entt::null;
        return;
    }

    m_current_group = group;

    trigger_all_changed();
}

void OpticalPropertiesObject::set_new_database_connections(Database* ptr) {
    add_connection(connect(ptr->material_parameters.self(),
                           &ComponentAPIBase::changed,
                           this,
                           &OpticalPropertiesObject::parameters_changed));
}

void OpticalPropertiesObject::parameters_changed(entt::entity e) {
    if (this->m_current_group != e) return;

    trigger_all_changed();
}

SolTrace::Data::OpticalProperties* OpticalPropertiesObject::get_properties() {
    if (!database()) return nullptr;

    auto* ptr = database()->material_parameters.get(m_current_group);

    if (!ptr) return nullptr;

    if (m_back) {
        return &ptr->optics_back;
    } else {
        return &ptr->optics_front;
    }
}

SolTrace::Data::OpticalProperties const*
OpticalPropertiesObject::get_properties() const {
    if (!database()) return nullptr;

    auto* ptr = database()->material_parameters.get(m_current_group);

    if (!ptr) return nullptr;

    if (m_back) {
        return &ptr->optics_back;
    } else {
        return &ptr->optics_front;
    }
}

QString OpticalPropertiesObject::interaction_type() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return QString::fromStdString(
        SolTrace::Data::InteractionTypeMap.at(ptr->my_type));
}

QString OpticalPropertiesObject::error_distribution_type() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return QString::fromStdString(
        SolTrace::Data::DistributionTypeMap.at(ptr->error_distribution_type));
}

double OpticalPropertiesObject::transmitivity() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->transmitivity;
}

double OpticalPropertiesObject::reflectivity() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->reflectivity;
}

double OpticalPropertiesObject::slope_error() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->slope_error;
}

double OpticalPropertiesObject::specularity_error() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->specularity_error;
}

double OpticalPropertiesObject::refraction_index_front() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->refraction_index_front;
}

double OpticalPropertiesObject::refraction_index_back() const {
    auto* ptr = get_properties();

    if (!ptr) return {};

    return ptr->refraction_index_back;
}


void OpticalPropertiesObject::set_interaction_type(QString v) {
    auto* ptr = get_properties();

    if (!ptr) return;

    auto string = v.toStdString();

    auto iter = reverse_lookup(SolTrace::Data::InteractionTypeMap, string);

    if (iter) {
        ptr->my_type = *iter;
        emit interaction_type_changed();
    }
}

void OpticalPropertiesObject::set_error_distribution_type(QString v) {
    auto* ptr = get_properties();

    if (!ptr) return;

    auto string = v.toStdString();

    auto iter = reverse_lookup(SolTrace::Data::DistributionTypeMap, string);

    if (iter) {
        ptr->error_distribution_type = *iter;
        emit error_distribution_type_changed();
    }
}

#define SETTER(MEMBER)                                                         \
    auto* ptr = get_properties();                                              \
    if (!ptr) return;                                                          \
    ptr->MEMBER = v;                                                           \
    emit MEMBER##_changed();

void OpticalPropertiesObject::set_transmitivity(double v) {
    SETTER(transmitivity);
}

void OpticalPropertiesObject::set_reflectivity(double v) {
    SETTER(reflectivity);
}

void OpticalPropertiesObject::set_slope_error(double v) {
    SETTER(slope_error);
}

void OpticalPropertiesObject::set_specularity_error(double v) {
    SETTER(specularity_error);
}

void OpticalPropertiesObject::set_refraction_index_front(double v) {
    SETTER(refraction_index_front);
}

void OpticalPropertiesObject::set_refraction_index_back(double v) {
    SETTER(refraction_index_back);
}

void OpticalPropertiesObject::set_ideal_absorption() {
    auto* ptr = get_properties();

    if (!ptr) return;

    ptr->set_ideal_absorption();

    trigger_all_changed();
}

void OpticalPropertiesObject::set_ideal_reflection() {
    auto* ptr = get_properties();

    if (!ptr) return;

    ptr->set_ideal_reflection();

    trigger_all_changed();
}

void OpticalPropertiesObject::set_ideal_transmission() {
    auto* ptr = get_properties();

    if (!ptr) return;

    ptr->set_ideal_transmission();

    trigger_all_changed();
}

void OpticalPropertiesObject::set_ideal_transmission_with_indices(
    double n_front,
    double n_back) {
    auto* ptr = get_properties();

    if (!ptr) return;

    ptr->set_ideal_transmission(n_front, n_back);

    trigger_all_changed();
}

} // namespace db
