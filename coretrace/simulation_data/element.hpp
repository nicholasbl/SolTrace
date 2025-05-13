#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

// #include <memory>
#include <string>

#include "aperture.hpp"
#include "container.hpp"
#include "datetime.hpp"
#include "optical_properties.hpp"
#include "ray_source.hpp"
#include "surface.hpp"
#include "vector3d.hpp"

using element_id = std::int_fast64_t;
const element_id ELEMENT_ERROR = -1;
const element_id ELEMENT_ID_UNASSIGNED = -2;
const element_id ELEMENT_ALREADY_REGISTERED = -3;

// Forward declaration of the Element class so we can define ElementContainer
class Element;

using ElementContainer = Container<element_id, Element>;
using element_ptr = ElementContainer::value_pointer;

class Element
{
public:
  Element() {};
  virtual ~Element() {};

  // Accessors for any element
  /// @brief Disable the element for ray tracing
  virtual void disable() const = 0;
  /// @brief Enable the element for ray tracing
  virtual void enable() const = 0;
  /// @brief Check whether element is enabled/disabled
  /// @return true if enabled, false otherwise
  virtual bool is_enabled() const = 0;

  /// @brief Check whether the element is a CompositeElement
  /// @return true if CompositeElement, false otherwise
  virtual bool is_composite() const = 0;
  /// @brief Check whether the element is a SingleElement
  /// @return true if SingleElement, false otherwise
  virtual bool is_single() const = 0;
  /// @brief Check whether the element is a VirtualElement
  /// @return true if VirtualElement, false otherwise
  virtual bool is_virtual() const = 0;

  /// @brief Get the element id assigned when registerd with SimulationData
  /// @return id if registered with SimulationData, ELEMENT_ID_UNASSIGNED if not
  virtual element_id get_id() const = 0;
  /// @brief Set the element id--used by SimulationData
  /// @param id id assigned and to set
  virtual void set_id(element_id id) = 0;

  virtual const std::string& get_name() const = 0;
  virtual void set_name(const std::string &name) = 0;

  virtual const Vector3d &get_origin() const = 0;
  virtual void set_origin(const Vector3d &) = 0;
  virtual const Vector3d &get_aim_vector() const = 0;
  virtual void set_aim_vector(const Vector3d &) = 0;
  virtual const Vector3d &get_euler_angles() const = 0;
  virtual void set_euler_angles(const Vector3d &) = 0;
  virtual double get_zrot() const = 0;
  virtual void set_zrot(double) = 0;

  // virtual const Vector3d &get_upper_bounding_box() const = 0;
  // virtual const Vector3d &get_lower_bounding_box() const = 0;

  // Accessors for SingleElements
  virtual const aperture_ptr get_aperture() const = 0;
  virtual aperture_ptr get_aperture() = 0;
  virtual void set_aperture(aperture_ptr) = 0;
  virtual const surface_ptr get_surface() const = 0;
  virtual surface_ptr get_surface() = 0;
  virtual void set_surface(surface_ptr) = 0;

  // virtual const OpticalProperties &get_optical_properties() const = 0;
  // virtual void set_optical_properties(const OpticalProperties &) = 0;

  virtual const OpticalProperties *get_front_optical_properties() const = 0;
  virtual OpticalProperties *get_front_optical_properties() = 0;
  virtual void set_front_optical_properties(const OpticalProperties &) = 0;

  virtual const OpticalProperties *get_back_optical_properties() const = 0;
  virtual OpticalProperties *get_back_optical_properties() = 0;
  virtual void set_back_optical_properties(const OpticalProperties &) = 0;

  // Accessors for CompositeElements
  // virtual void set_parent_element(element_ptr parent) = 0;
  // virtual ElementContainer::iterator get_iterator() = 0;
  // virtual ElementContainer::const_iterator get_const_iterator() = 0;
  // virtual bool is_at_end(ElementContainer::iterator iter) = 0;
  // virtual bool is_at_end(ElementContainer::const_iterator iter) = 0;

  // Computational routines
  // Convert `global` to local coordinates and store the result in `local`
  virtual int convert_global_to_local(Vector3d &local,
                                      const Vector3d &global) = 0;
  // virtual int convert_global_to_local(Vector3d &inplace) = 0;
  // Convert `local` to global coordinates and store the result in `global`
  virtual int convert_local_to_global(Vector3d &global,
                                      const Vector3d &local) = 0;
  // virtual int convert_local_to_global(Vector3d &inplace) = 0;
  // Convert `ref` to local coordinates and store the result in `local`
  virtual int convert_reference_to_local(Vector3d &local,
                                         const Vector3d &ref) = 0;
  // virtual int convert_reference_to_local(Vector3d &inplace) = 0;
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_reference(Vector3d &ref,
                                         const Vector3d &local) = 0;
  // virtual int convert_local_to_reference(Vector3d &inplace) = 0;

  // Other routines
  virtual int compute_coordinate_rotations() = 0;
  // virtual int set_bounding_box() = 0;
  virtual int update_orientation(const DateTime &,
                                 const Vector3d &source,
                                 const Vector3d &target) = 0;

protected:
  // virtual int set_bounding_box() = 0;

private:
};

class ElementBase : public Element
{
public:
  ElementBase();
  // ElementBase(const Vector3d &origin, const Vector3d &aim);
  virtual ~ElementBase();

  virtual inline void disable() const { this->active = false; }
  virtual inline void enable() const { this->active = true; }
  virtual bool is_enabled() const { return this->active; }

  virtual bool is_composite() const { return false; }
  virtual bool is_single() const { return false; }
  // TODO: Do we need this?
  virtual bool is_virtual() const { return false; }

  // virtual ElementContainer::iterator get_iterator();
  // virtual ElementContainer::const_iterator get_const_iterator();
  // virtual bool is_at_end(ElementContainer::iterator iter) { return true; }
  // virtual bool is_at_end(ElementContainer::const_iterator iter) { return true; }
  // virtual void set_parent_element(element_ptr parent)
  // {
  //   this->parent = parent;
  // }

  virtual element_id get_id() const { return this->my_id; }
  virtual void set_id(element_id id)
  {
    this->my_id = id;
    return;
  }

  virtual const std::string& get_name() const
  {
    return this->my_name;
  }
  virtual void set_name(const std::string &name)
  {
    this->my_name = name;
  }

  virtual const Vector3d &get_origin() const { return this->origin; }
  virtual void set_origin(const Vector3d &point)
  {
    this->origin = point;
    return;
  }
  virtual const Vector3d &get_aim_vector() const { return this->aim; }
  virtual void set_aim_vector(const Vector3d &direction)
  {
    this->aim = direction;
    return;
  }
  virtual const Vector3d &get_euler_angles() const
  {
    return this->euler_angles;
  }
  virtual void set_euler_angles(const Vector3d &angles)
  {
    this->euler_angles = angles;
    return;
  }
  virtual double get_zrot() const { return this->zrot; }
  virtual void set_zrot(double rot)
  {
    this->zrot = rot;
    return;
  }

  virtual int compute_coordinate_rotations();

  // Convert `ref` to local coordinates and store the result in `local`
  virtual int convert_reference_to_local(Vector3d &local, const Vector3d &ref);
  // virtual int convert_reference_to_local(Vector3d &inplace);
  // Convert `local` to global coordinates and store the result in `global`
  virtual int convert_global_to_local(Vector3d &local, const Vector3d &global);
  // virtual int convert_global_to_local(Vector3d &inplace);
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_reference(Vector3d &ref, const Vector3d &local);
  // virtual int convert_local_to_reference(Vector3d &inplace);
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_global(Vector3d &ref, const Vector3d &local);
  // virtual int convert_local_to_global(Vector3d &inplace);

protected:
  // TODO: Do these need to be mutable?
  mutable bool active;
  mutable element_id my_id;

  std::string my_name;

  // Location of the origin in the reference coordinate system
  Vector3d origin;
  // Aim vector of element--aligns with the local (positive) z-axis
  Vector3d aim;
  // Rotation about the aim vector to set local x and y axes in radians
  double zrot;

  Vector3d euler_angles;

  Matrix3d reference_to_local;
  Matrix3d local_to_reference;

  // element_ptr parent;

private:
  // static ElementContainer empty_container;
};

// using element_ptr = typename std::shared_ptr<Element>;
// using ElementContainer = std::map<element_id, element_ptr>;

template <typename C, typename... Args>
inline auto make_element(Args &&...args)
{
  return ElementContainer::make_pointer<C>(std::forward<Args>(args)...);
}

#endif
