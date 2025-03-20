#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

#include <memory>
#include <string>

#include "container.hpp"
#include "datetime.hpp"
#include "optical_properties.hpp"
#include "ray_source.hpp"
#include "shape.hpp"
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

  virtual const Vector3d &get_upper_bounding_box() const = 0;
  virtual const Vector3d &get_lower_bounding_box() const = 0;

  virtual const Shape &get_shape() const = 0;

  virtual const OpticalProperties &get_optical_properties() const = 0;
  virtual void set_optical_properties(const OpticalProperties &) = 0;

  // Other routines
  virtual int compute_coordinate_rotations() = 0;
  virtual int set_bounding_box() = 0;
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
  virtual ~ElementBase();

  virtual const Vector3d &get_origin() const
  {
    return this->get_origin();
  }
  virtual void set_origin(const Vector3d &point)
  {
    this->set_origin(point);
    return;
  }
  virtual const Vector3d &get_aim_vector() const
  {
    return this->get_aim_vector();
  }
  virtual void set_aim_vector(const Vector3d &direction)
  {
    this->set_aim_vector(direction);
    return;
  }
  virtual const Vector3d &get_euler_angles() const
  {
    return this->get_euler_angles();
  }
  virtual void set_euler_angles(const Vector3d &angles)
  {
    this->set_euler_angles(angles);
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
  int convert_reference_to_local(Vector3d &local, const Vector3d &ref);
  // Convert `local` to reference coordinates and store the result in `ref`
  int convert_local_to_reference(Vector3d &ref, const Vector3d &local);

protected:
  void clear_box_bounds();

  OpticalProperties optics;
  std::unique_ptr<Shape> shape;

  // Location of the origin in the reference coordinate system
  Vector3d origin;
  // Aim vector of element--aligns with the local (positive) z-axis
  Vector3d aim;
  // Rotation about the aim vector to set local x and y axes in radians
  double zrot;

  Vector3d euler_angles;

  Matrix3d reference_to_local;
  Matrix3d local_to_reference;

  // TODO: Should these be in local or reference coordinates?
  Vector3d upper_box_bound;
  Vector3d lower_box_bound;
};

using element_id = std::int_fast64_t;
using ElementContainer = Container<element_id, Element>;
using element_ptr = ElementContainer::value_pointer;

#endif
