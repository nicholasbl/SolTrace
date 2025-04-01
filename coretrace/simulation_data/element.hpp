#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

#include <memory>
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

  virtual const Aperature *get_aperature() const = 0;
  virtual const Surface *get_surface() const = 0;

  // virtual const OpticalProperties &get_optical_properties() const = 0;
  // virtual void set_optical_properties(const OpticalProperties &) = 0;

  virtual const OpticalProperties &get_front_optical_properties() const = 0;
  virtual OpticalProperties &get_front_optical_properties() = 0;
  virtual void set_front_optical_properties(const OpticalProperties &) = 0;

  virtual const OpticalProperties &get_back_optical_properties() const = 0;
  virtual OpticalProperties &get_back_optical_properties() = 0;
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
  ElementBase(const Vector3d &origin, const Vector3d &aim);
  virtual ~ElementBase();

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

  // virtual const OpticalProperties &get_optical_properties() const;
  const OpticalProperties &get_front_optical_properties() const
  {
    return this->optics_front;
  }
  OpticalProperties &get_front_optical_properties()
  {
    return this->optics_front;
  }
  // virtual void set_optical_properties(const OpticalProperties &);
  void set_front_optical_properties(const OpticalProperties &op)
  {
    this->optics_front = op;
  }

  // virtual const OpticalProperties &get_optical_properties() const;
  const OpticalProperties &get_back_optical_properties() const
  {
    return this->optics_back;
  }
  OpticalProperties &get_back_optical_properties()
  {
    return this->optics_back;
  }
  // virtual void set_optical_properties(const OpticalProperties &);
  void set_back_optical_properties(const OpticalProperties &op)
  {
    this->optics_back = op;
  }

  virtual int compute_coordinate_rotations();

  // Convert `ref` to local coordinates and store the result in `local`
  virtual int convert_reference_to_local(Vector3d &local, const Vector3d &ref);
  // Convert `local` to reference coordinates and store the result in `ref`
  virtual int convert_local_to_reference(Vector3d &ref, const Vector3d &local);

protected:
  // void clear_box_bounds();

  OpticalProperties optics_front;
  OpticalProperties optics_back;
  std::unique_ptr<Aperature> aperature;
  std::unique_ptr<Surface> surface;

  // Location of the origin in the reference coordinate system
  Vector3d origin;
  // Aim vector of element--aligns with the local (positive) z-axis
  Vector3d aim;
  // Rotation about the aim vector to set local x and y axes in radians
  double zrot;

  Vector3d euler_angles;

  Matrix3d reference_to_local;
  Matrix3d local_to_reference;

  // // TODO: Should these be in local or reference coordinates?
  // // TODO: Do we need these here? Or only in the tracing part?
  // Vector3d upper_box_bound;
  // Vector3d lower_box_bound;
};

using element_id = std::int_fast64_t;
using ElementContainer = Container<element_id, Element>;
using element_ptr = ElementContainer::value_pointer;

#endif
