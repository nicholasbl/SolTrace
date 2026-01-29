#include "conversion.h"

#include <QtMath>
#include <cmath>
#include <random>

namespace db {

static constexpr double PI     = M_PI;
static constexpr double TWO_PI = 2.0 * PI;

static inline double wrap_pi(double a) {
    a = std::fmod(a + PI, TWO_PI);
    if (a < 0) a += TWO_PI;
    return a - PI;
}

static inline double angle_diff(double a, double b) {
    return std::abs(wrap_pi(a - b));
}

static inline QQuaternion angle_axis_rad(double           angleRad,
                                         QVector3D const& axisUnit) {
    // Qt uses DEGREES for fromAxisAndAngle
    return QQuaternion::fromAxisAndAngle(axisUnit, qRadiansToDegrees(angleRad));
}

static inline QQuaternion align_unit_vector(QVector3D const& fromUnit,
                                            QVector3D const& toUnit) {
    double d = QVector3D::dotProduct(fromUnit, toUnit);

    if (d > 1.0 - 1e-7) { return QQuaternion(); }

    if (d < -1.0 + 1e-7) {
        QVector3D ortho = (std::abs(fromUnit.z()) < 0.9) ? QVector3D(0, 0, 1)
                                                         : QVector3D(0, 1, 0);
        QVector3D axis  = QVector3D::crossProduct(fromUnit, ortho).normalized();
        return QQuaternion::fromAxisAndAngle(axis, 180.0);
    }

    return QQuaternion::rotationTo(fromUnit, toUnit);
}

QQuaternion dir_roll_to_quat(QVector3D const& directionWorld,
                             double           zRollRadians) {
    QVector3D dir = directionWorld;
    double    len = dir.length();
    if (len < 1e-8) return QQuaternion();

    dir /= len;

    const QVector3D localForward(0, 0, 1);
    auto            qAlign = align_unit_vector(localForward, dir);

    auto qRoll = angle_axis_rad(zRollRadians, dir);

    return (qRoll * qAlign).normalized();
}

static inline QQuaternion twist_around_axis(QQuaternion const& q,
                                            QVector3D const&   axisUnit) {
    QVector3D v(q.x(), q.y(), q.z());
    auto      proj = axisUnit * QVector3D::dotProduct(v, axisUnit);

    QQuaternion twist(q.scalar(), proj.x(), proj.y(), proj.z());
    double      n = twist.length();
    if (n < 1e-8) return QQuaternion();

    twist.normalize();
    return twist;
}

static inline double twist_angle_radians(QQuaternion const& twist,
                                         QVector3D const&   axisUnit) {
    auto t = twist.normalized();

    QVector3D v(t.x(), t.y(), t.z());
    double    vlen = v.length();
    double    w    = t.scalar();

    double angle = 2.0 * std::atan2(vlen, w); // [0, 2pi)
    double s     = QVector3D::dotProduct(v, axisUnit);
    if (s < 0.0) angle = -angle;

    return wrap_pi(angle);
}

static inline bool approx_equal_quat(QQuaternion const& a,
                                     QQuaternion const& b,
                                     double             eps = 1e-5f) {
    // q and -q represent the same rotation
    return (a - b).length() <= eps || (a + b).length() <= eps;
}

void quat_to_dir_roll(QQuaternion const& qIn,
                      QVector3D&         outDirectionWorld,
                      double&            outZRollRadians) {
    auto q = qIn.normalized();

    const QVector3D localForward(0, 0, 1);
    auto            dir = q.rotatedVector(localForward);

    double len = dir.length();
    if (len < 1e-8) {
        outDirectionWorld = QVector3D(0, 0, 1);
        outZRollRadians   = 0.0;
        return;
    }

    dir /= len;
    outDirectionWorld = dir;

    auto twist      = twist_around_axis(q, dir);
    outZRollRadians = twist_angle_radians(twist, dir);
}


static inline QVector3D random_unit_vec(std::mt19937& rng) {
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    QVector3D                             v;
    do {
        v = QVector3D(dist(rng), dist(rng), dist(rng));
    } while (v.lengthSquared() < 1e-8f);
    return v.normalized();
}

static inline void runRoundTripTests(std::uint32_t seed  = 12345,
                                     int           iters = 10000) {
    std::mt19937                          rng(seed);
    std::uniform_real_distribution<float> rollDist(-PI, PI);

    float maxDirErr    = 0.0f;
    float maxRollErr   = 0.0f;
    int   quatFailures = 0;

    for (int i = 0; i < iters; ++i) {
        QVector3D dir0  = random_unit_vec(rng);
        float     roll0 = rollDist(rng);

        auto q0 = dir_roll_to_quat(dir0, roll0);

        QVector3D dir1;
        double    roll1;
        quat_to_dir_roll(q0, dir1, roll1);

        auto q1 = dir_roll_to_quat(dir1, roll1);

        float dirErr  = (dir0 - dir1).length();
        float rollErr = angle_diff(roll0, roll1);

        maxDirErr  = std::max(maxDirErr, dirErr);
        maxRollErr = std::max(maxRollErr, rollErr);

        if (!approx_equal_quat(q0, q1, 1e-4f)) { ++quatFailures; }
    }

    qDebug() << "Round-trip tests: " << iters;
    qDebug() << "Max direction error: " << maxDirErr;
    qDebug() << "Max roll error (rad): " << maxRollErr;
    qDebug() << "Quaternion re-encode mismatches (tolerant): " << quatFailures;
}

int test() {
    auto  dir  = QVector3D(1, 2, 3).normalized();
    float roll = 0.7f;

    auto q = dir_roll_to_quat(dir, roll);

    QVector3D dir2;
    double    roll2;
    quat_to_dir_roll(q, dir2, roll2);

    qDebug() << "Original dir: (" << dir.x() << "," << dir.y() << "," << dir.z()
             << ")";
    qDebug() << "Decoded  dir: (" << dir2.x() << "," << dir2.y() << ","
             << dir2.z() << ")";
    qDebug() << "Original roll: " << roll;
    qDebug() << "Decoded  roll: " << roll2;

    runRoundTripTests(12345, 20000);
    return 0;
}

int test2() {

    std::vector<QVector3D> positions = {
        { 300.4760, 670.9580, 3.6927 },    { 300.0650, 669.9370, 2.6376 },
        { 299.6530, 668.9150, 1.5826 },    { 293.9950, 671.1970, 1.5826 },
        { -1214.6600, 163.1450, -0.5042 }, { -1082.3500, 360.9840, -3.5815 },
        { 209.9780, -847.9350, 2.9950 },   { -1003.4800, -476.8940, 3.2507 },
    };
    std::vector<QVector3D> aims = {
        { 38.8543, 27.7015, 723.2590 },    { 38.7178, 27.3632, 722.9140 },
        { 38.5817, 27.0256, 722.5680 },    { 36.7186, 27.7769, 722.5680 },
        { -591.0470, -55.8965, 749.9110 }, { -501.2880, 28.8448, 739.4200 },
        { 14.6201, -319.2480, 829.0250 },  { -413.4470, -318.4600, 794.9350 },
    };
    std::vector<float> rots = { 29.184700,  29.184700,  29.184700,  29.184700,
                                -75.322200, -67.126400, -23.855000, 77.872600 };

    for (int i = 0; i < positions.size(); i++) {
        auto p = positions.at(i);
        auto a = aims.at(i);
        auto r = qDegreesToRadians(rots.at(i));

        a = (a - p).normalized();

        auto q = dir_roll_to_quat(a, r);

        QVector3D dir2;
        double    roll2;
        quat_to_dir_roll(q, dir2, roll2);

        qDebug() << "-----";
        qDebug() << "Original dir: (" << a.x() << "," << a.y() << "," << a.z()
                 << ")";
        qDebug() << "Decoded  dir: (" << dir2.x() << "," << dir2.y() << ","
                 << dir2.z() << ")";
        qDebug() << "Original roll: " << r;
        qDebug() << "Decoded  roll: " << roll2;
        qDebug() << (a - p).length();
    }
    return 0;
}


static int VT = test();
static int VT2 = test2();

} // namespace db
