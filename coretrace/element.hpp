#ifndef SOLTRACE_ELEMENT_H
#define SOLTRACE_ELEMENT_H

#include <cstdint>

#include "datetime.hpp"
#include "ray_source.hpp"

class Element
{
public:

  Element(){};
  virtual ~Element(){};

  virtual const double *get_origin() = 0;
  virtual const double *get_aim_point() = 0;
  virtual const double *get_euler_angles() = 0;

  virtual int update_orientation(DateTime &, const RaySource*) = 0;
  
private:

};

#endif
