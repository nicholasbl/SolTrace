#include "database/conversion.h"

#include <gtest/gtest.h>

#define GLM_ENABLE_EXPERIMENTAL 1

#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>

#include <cmath>

namespace {

constexpr double kTolerance = 1.0e-12;

void expect_vec_near(glm::dvec3 const& actual,
                     glm::dvec3 const& expected,
                     double            tolerance = kTolerance) {
    EXPECT_NEAR(actual.x, expected.x, tolerance);
    EXPECT_NEAR(actual.y, expected.y, tolerance);
    EXPECT_NEAR(actual.z, expected.z, tolerance);
}

bool same_rotation(glm::dquat const& a,
                   glm::dquat const& b,
                   double            tolerance = kTolerance) {
    return glm::length(a - b) <= tolerance ||
           glm::length(a + b) <= tolerance;
}

} // namespace

TEST(DirRollToQuat, IdentityForZeroDirection) {
    auto q = db::dir_roll_to_quat({ 0.0, 0.0, 0.0 }, 1.25);

    EXPECT_TRUE(same_rotation(q, glm::identity<glm::dquat>()));
}

TEST(DirRollToQuat, MapsLocalForwardToDirection) {
    glm::dvec3 direction = glm::normalize(glm::dvec3(1.0, 2.0, 3.0));

    auto q = db::dir_roll_to_quat(direction, 0.0);

    expect_vec_near(q * glm::dvec3(0.0, 0.0, 1.0), direction);
}

TEST(DirRollToQuat, AppliesRollAroundDirection) {
    glm::dvec3 direction = glm::dvec3(0.0, 0.0, 1.0);
    double     roll      = glm::half_pi<double>();

    auto q = db::dir_roll_to_quat(direction, roll);

    expect_vec_near(q * glm::dvec3(0.0, 0.0, 1.0), direction);
    expect_vec_near(q * glm::dvec3(0.0, 1.0, 0.0),
                    glm::dvec3(-1.0, 0.0, 0.0));
}
