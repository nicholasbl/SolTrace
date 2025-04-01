#ifndef SOLTRACE_SURFACE_H
#define SOLTRACE_SURFACE_H

struct Surface {};

struct Conical : public Surface {};
struct Cylinder : public Surface {};
struct Flat : public Surface {};
struct Parabolic : public Surface {};
struct Spherical : public Surface {};

#endif
