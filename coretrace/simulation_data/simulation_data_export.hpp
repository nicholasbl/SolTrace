#ifndef SOLTRACE_SIMDATA_EXPORT_H
#define SOLTRACE_SIMDATA_EXPORT_H

#include "aperture.hpp"
#include "composite_element.hpp"
#include "simulation_data.hpp"
#include "simulation_parameters.hpp"
#include "single_element.hpp"
#include "stage_element.hpp"
#include "sun.hpp"
#include "surface.hpp"

#include "cst_templates/heliostat.hpp"
#include "cst_templates/linear_fresnel.hpp"
#include "cst_templates/parabolic_dish.hpp"
#include "cst_templates/parabolic_trough.hpp"

// Classes and Structs
using SolTrace::Data::Annulus;
using SolTrace::Data::Aperture;
using SolTrace::Data::ApertureType;
using SolTrace::Data::Circle;
using SolTrace::Data::CompositeElement;
using SolTrace::Data::Cylinder;
using SolTrace::Data::DistributionType;
using SolTrace::Data::EqualateralTriangle;
using SolTrace::Data::Flat;
using SolTrace::Data::Hexagon;
using SolTrace::Data::InteractionType;
using SolTrace::Data::IrregularQuadrilateral;
using SolTrace::Data::IrregularTriangle;
using SolTrace::Data::Matrix3d;
using SolTrace::Data::OpticalProperties;
using SolTrace::Data::Parabola;
using SolTrace::Data::Rectangle;
using SolTrace::Data::SimulationData;
using SolTrace::Data::SimulationParameters;
using SolTrace::Data::SingleElement;
using SolTrace::Data::Sphere;
using SolTrace::Data::StageElement;
using SolTrace::Data::Sun;
using SolTrace::Data::Surface;
using SolTrace::Data::SurfaceType;
using SolTrace::Data::Vector3d;

// Template Types
using SolTrace::Data::Heliostat;
using SolTrace::Data::LinearFresnel;
using SolTrace::Data::ParabolicDish;
using SolTrace::Data::ParabolicTrough;

// Other Types
using SolTrace::Data::aperture_ptr;
using SolTrace::Data::element_id;
using SolTrace::Data::element_ptr;
using SolTrace::Data::ray_source_id;
using SolTrace::Data::ray_source_ptr;
using SolTrace::Data::stage_ptr;
using SolTrace::Data::surface_ptr;

// Functions
using SolTrace::Data::make_aperture;
using SolTrace::Data::make_element;
using SolTrace::Data::make_ray_source;
using SolTrace::Data::make_stage;
using SolTrace::Data::make_surface;
using SolTrace::Data::make_surface_from_type;

// Matrix-Vector Functions
using SolTrace::Data::CopyVec3;
using SolTrace::Data::DOT;
using SolTrace::Data::IdentityMat3;
using SolTrace::Data::SetVec3;
using SolTrace::Data::ZeroVec3;

// Coordinate Transform Functions
using SolTrace::Data::CalculateTransformMatrices;
using SolTrace::Data::TransformToLocal;
using SolTrace::Data::TransformToReference;

// Status Constants
using SolTrace::Data::ELEMENT_ERROR;
using SolTrace::Data::ELEMENT_ID_UNASSIGNED;
using SolTrace::Data::ELEMENT_ALREADY_REGISTERED;
using SolTrace::Data::ELEMENT_INVALID_SETUP;
using SolTrace::Data::ELEMENT_NULL;

// Math Constants
using SolTrace::Data::D2R;
using SolTrace::Data::PI;
using SolTrace::Data::R2D;

#endif
