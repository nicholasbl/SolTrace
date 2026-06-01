/**
 * @file optical_properties.hpp
 * @brief Optical properties definitions for materials
 *
 * Defines optical properties (reflectivity, transmissivity, refractive index)
 * and interaction types for optical surfaces and materials. Includes error
 * distribution parameters for modeling surface imperfections and optical errors.
 */

#ifndef SOLTRACE_OPTICAL_PROPERTIES_H
#define SOLTRACE_OPTICAL_PROPERTIES_H

#include <container.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

#include "error_distributions.hpp"

namespace SolTrace::Data
{
    using optics_id = std::int_fast64_t;
    constexpr optics_id OPTICS_ID_UNASSIGNED = -2;
    constexpr optics_id OPTICS_ID_VIRTUAL = -3;
    class OpticalPropertySet;
    using OpticalPropertySetContainer = Container<optics_id, OpticalPropertySet>;

    enum class InteractionType
    {
        REFLECTION,
        REFRACTION,
        UNKNOWN
    };

    inline const std::map<InteractionType, std::string> InteractionTypeMap =
    {
        {InteractionType::REFLECTION, "REFLECTION"},
        {InteractionType::REFRACTION, "REFRACTION"},
        {InteractionType::UNKNOWN, "UNKNOWN"}
    };

    struct OpticalPropertiesFace
    {
        DistributionType error_distribution_type;
        double transmissivity;
        double reflectivity;
        double slope_error;                 // [mrad]
        double specularity_error;           // [mrad]

        OpticalPropertiesFace() : error_distribution_type(DistributionType::UNKNOWN),
                                  transmissivity(0.0),
                                  reflectivity(0.0),
                                  slope_error(0.0),
                                  specularity_error(0.0)
        {
        }

        OpticalPropertiesFace(DistributionType dtype,
                          double trans, double refl,
                          double slope_err, double spec_err)
            : error_distribution_type(dtype),
              transmissivity(trans),
              reflectivity(refl),
              slope_error(slope_err),
              specularity_error(spec_err)
        {
        }

        OpticalPropertiesFace(const nlohmann::ordered_json& jnode);

        // TODO: What should the error settings be with the below?

        void set_ideal_material()
        {
            this->error_distribution_type = DistributionType::NONE;
            this->specularity_error = 0.0;
            this->slope_error = 0.0;
            return;
        }

        // Warning: this does not check if set is REFLECTION type
        void set_ideal_absorption()
        {
            this->set_ideal_material();
            this->transmissivity = 0.0;
            this->reflectivity = 0.0;
            return;
        }

        // Warning: this does not check if set is REFLECTION type
        void set_ideal_reflection()
        {
            this->set_ideal_material();
            this->transmissivity = 0.0;
            this->reflectivity = 1.0;
            return;
        }

        void write_json(nlohmann::ordered_json& jnode) const;

        bool operator==(const OpticalPropertiesFace& other) const;
        bool operator!=(const OpticalPropertiesFace& other) const;

        friend std::ostream &operator<<(std::ostream &os,
                                        const OpticalPropertiesFace& op);
    };

    struct OpticalPropertySet
    {
        OpticalPropertiesFace front;
        OpticalPropertiesFace back;

        InteractionType my_type;

        double refraction_index_front;
        double refraction_index_back;

        std::string my_name;

        OpticalPropertySet(OpticalPropertiesFace front_prop, OpticalPropertiesFace back_prop,
            InteractionType interaction_type, double refrac_front, double refrac_back, 
            std::string name = "")
            : front(front_prop), back(back_prop),
            my_type(interaction_type), refraction_index_front(refrac_front),
            refraction_index_back(refrac_back),
            my_name(name)
        {
        };

        OpticalPropertySet()
            : my_type(InteractionType::UNKNOWN),
            refraction_index_front(0.0),
            refraction_index_back(0.0),
            my_name("")
        {
        };

        OpticalPropertySet(const nlohmann::ordered_json& jnode);

        void set_ideal_transmission()
        {
            this->my_type = InteractionType::REFRACTION;

            this->front.set_ideal_material();
            this->back.set_ideal_material();

            this->front.transmissivity = 1.0;
            this->front.reflectivity = 0.0;

            this->back.transmissivity = 1.0;
            this->back.reflectivity = 0.0;

            return;
        }
        void set_ideal_transmission(double refraction_index_front,
            double refraction_index_back)
        {
            this->set_ideal_transmission();
            this->refraction_index_front = refraction_index_front;
            this->refraction_index_back = refraction_index_back;
            return;
        }
        void set_ideal_one_sided_reflector()
        {
            this->my_type = InteractionType::REFLECTION;

            this->front.set_ideal_material();
            this->back.set_ideal_material();

            this->front.reflectivity = 1.0;
            this->front.transmissivity = 0.0;

            this->back.reflectivity = 0.0;
            this->back.transmissivity = 0.0;
        }

        void write_json(nlohmann::ordered_json& jnode) const;

        bool operator==(const OpticalPropertySet& other) const;
        bool operator!=(const OpticalPropertySet& other) const;

        friend std::ostream& operator<<(std::ostream& os,
            const OpticalPropertySet& op);
    };

} // namespace SolTrace::Data

#endif
