#pragma once

#include <QQuaternion>
#include <QVector3D>

namespace db {
QQuaternion dir_roll_to_quat(QVector3D const& directionWorld,
                             double           zRollRadians);

void quat_to_dir_roll(QQuaternion const& qIn,
                      QVector3D&         outDirectionWorld,
                      double&            outZRollRadians);

} // namespace db
