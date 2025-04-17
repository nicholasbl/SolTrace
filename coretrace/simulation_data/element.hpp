#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

// #include <memory>
#include <string>

#include "aperature.hpp"
#include "container.hpp"
#include "datetime.hpp"
#include "optical_properties.hpp"
#include "ray_source.hpp"
#include "surface.hpp"
#include "vector3d.hpp"

class Element
{
public:
  Element() {};
  virtual ~Element() {};

  // Accessors
  virtual void disable() const = 0;
  virtual void enable() const = 0;
  virtual bool is_enabled() const = 0;

  virtual bool is_composite() const = 0;
  virtual bool is_virtual() const = 0;

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

  virtual const aperature_ptr get_aperature() const = 0;
  virtual aperature_ptr get_aperature() = 0;
  virtual void set_aperature(aperature_ptr) = 0;
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

  // Convert `ref` to local coordinates and store the result in `local`
  virtual int convert_reference_to_local(Vector3d &local,
                                         const Vector3d &ref) = 0;
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_reference(Vector3d &ref,
                                         const Vector3d &local) = 0;

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

  virtual inline void disable() const
  {
    this->active = false;
  }
  virtual inline void enable() const
  {
    this->active = true;
  }
  virtual bool is_enabled() const
  {
    return this->active;
  }

  virtual bool is_composite() const
  {
    return false;
  }
  virtual bool is_virtual() const
  {
    return false;
  }

  virtual const Vector3d &get_origin() const
  {
    return this->origin;
  }
  virtual void set_origin(const Vector3d &point)
  {
    this->origin = point;
    return;
  }
  virtual const Vector3d &get_aim_vector() const
  {
    return this->aim;
  }
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
  virtual double get_zrot() const
  {
    return this->zrot;
  }
  virtual void set_zrot(double rot)
  {
    this->zrot = rot;
    return;
  }

  virtual int compute_coordinate_rotations();

  // Convert `ref` to local coordinates and store the result in `local`
  virtual int convert_reference_to_local(Vector3d &local, const Vector3d &ref);
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_reference(Vector3d &ref, const Vector3d &local);

protected:
  mutable bool active;

  // Location of the origin in the reference coordinate system
  Vector3d origin;
  // Aim vector of element--aligns with the local (positive) z-axis
  Vector3d aim;
  // Rotation about the aim vector to set local x and y axes in radians
  double zrot;

  Vector3d euler_angles;

  Matrix3d reference_to_local;
  Matrix3d local_to_reference;
};

using element_id = std::int_fast64_t;
using ElementContainer = Container<element_id, Element>;
using element_ptr = ElementContainer::value_pointer;

template <typename C, typename... Args>
inline auto make_element(Args &&...args)
{
  return ElementContainer::make_pointer<C>(std::forward<Args>(args)...);
}

#endif
