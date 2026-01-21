#pragma once

#include "simulation_data_api.hpp"

#include <QVector3D>

template <class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

/*!
 * \brief Compute linear interpolation
 * \param x The interpolation source.
 * \param x0 The minimum range of x.
 * \param x1 The maximum range of x.
 * \param y0 The minimum range of y.
 * \param y1 The maximum range of y.
 *
 * Template T and U should support +, -, *, and / operators.
 */
template <class T, class U>
U lerp(T x, T const& x0, T const& x1, U const& y0, U const& y1) {
    return y0 + (y1 - y0) * ((x - x0) / (x1 - x0));
}


// NOTE that we are truncating vectors to floats...
namespace SD = SolTrace::Data;

inline QVector3D convert(SD::Vector3d v) {
    return QVector3D(v[0], v[1], v[2]);
}

inline SD::Vector3d convert(QVector3D v) {
    return { v.x(), v.y(), v.z() };
}
