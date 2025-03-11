#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

#include <cstdint>
#include <string>

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
  virtual const Vector3d & get_origin() const = 0;
  virtual void set_origin(const Vector3d &) = 0;
  virtual const Vector3d & get_aim_point() const = 0;
  virtual void set_aim_point(const Vector3d &) = 0;
  virtual const Vector3d & get_euler_angles() const = 0;
  virtual void set_euler_angles(const Vector3d &) = 0;

  virtual const Vector3d & get_upper_bounding_box() const = 0;
  virtual const Vector3d & get_lower_bounding_box() const = 0;

  virtual const Shape & get_shape() const = 0;
  
  virtual const OpticalProperties & get_optical_properties() const = 0;
  virtual void set_optical_properties(const OpticalProperties &) = 0;

  // Other routines
  virtual int update_orientation(const DateTime &, const RaySource &) = 0;

protected:

  virtual int set_bounding_box() = 0;

private:
};

class ElementBase : public Element
{
public:
  ElementBase();
  virtual ~ElementBase();

  virtual const Vector3d & get_origin() const
  {
    return this->origin;
  }
  virtual void set_origin(const Vector3d & point)
  {
    this->origin = point;
    return;
  }
  virtual const Vector3d & get_aim_point() const
  {
    return this->aim;
  }
  virtual void set_aim_point(const Vector3d & direction)
  {
    this->aim = direction;
    return;
  }
  virtual const Vector3d & get_euler_angles() const
  {
    return this->euler_angles;
  }
  virtual void set_euler_angles(const Vector3d & angles)
  {
    this->euler_angles = angles;
  }

private:

  Vector3d origin;
  Vector3d aim;
  Vector3d euler_angles;

  Matrix3d reference_to_local;
  Matrix3d local_to_reference;

  Vector3d upper_box_bound;
  Vector3d lower_box_bound;

  OpticalProperties optics;
  Shape shape;

};

#endif
