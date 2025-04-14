#ifndef SOLTRACE_SURFACE_H
#define SOLTRACE_SURFACE_H

struct Surface {
    Surface() {}
    virtual ~Surface() {}
};

struct Conical : public Surface {
    double half_angle;
    Conical() : half_angle(0.0) {}
    Conical(double ha) : half_angle(ha) {}
    virtual ~Conical() {}
};

struct Cylinder : public Surface {};

struct Flat : public Surface {
    Flat() {}
    virtual ~Flat() {}
};

struct Parabolic : public Surface {};

struct Spherical : public Surface {};

using surface_ptr = std::shared_ptr<Surface>;

template <typename S, typename... Args>
inline surface_ptr make_surface(Args &&...args)
{
    return std::make_shared<S>(std::forward<Args>(args)...);
}

#endif
