/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : math helper
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MATH_MATH_H_
#define IXR_ENGINE_MATH_MATH_H_
#include "engine/types.h"
#include "export/export.h"
#include "math/matrix.h"
#include "math/position.h"
#include "math/rotation.h"

namespace ixr {
namespace engine {
namespace math {

constexpr float PI = 3.14159f;
constexpr float EPSILON = 1e-5f;

float IXR_ENGINE_API Angular(const Position &a, const Position &b);

Matrix IXR_ENGINE_API LookToLH(const Position &eye, const Position &at,
                               const Position &up);

Matrix IXR_ENGINE_API PerspectiveLH(float fov, float ratio, float near,
                                    float far);
}  // namespace math
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MATH_MATH_H_
