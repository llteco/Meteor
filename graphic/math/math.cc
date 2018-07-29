/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : math helper
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#include "math/math.h"
#include <memory>  // for memcpy
namespace ixr {
namespace engine {
namespace math {
#ifdef IXR_MATH_BACKEND_DIRECTXMATH
#include <DirectXMath.h>
inline DirectX::XMVECTOR PositionToVector(const Position &p) {
  return DirectX::XMVectorSet(p.X(), p.Y(), p.Z(), 1.0f);
}

inline DirectX::XMVECTOR PositionToPoint(const Position &p) {
  return DirectX::XMVectorSet(p.X(), p.Y(), p.Z(), 0.0f);
}

inline DirectX::XMVECTOR QuaternionToVector(const Quaternion &q) {
  return DirectX::XMVectorSet(q.X(), q.Y(), q.Z(), q.W());
}

float Angular(const Position &a, const Position &b) {
  auto ans = DirectX::XMVector3AngleBetweenVectors(PositionToVector(a),
                                                   PositionToVector(b));
  return ans.m128_f32[0];
}

Matrix LookToLH(const Position &eye, const Position &at, const Position &up) {
  auto ans = DirectX::XMMatrixLookToLH(
      PositionToVector(eye), PositionToVector(at), PositionToVector(up));
  DirectX::XMFLOAT4X4 m;
  DirectX::XMStoreFloat4x4(&m, ans);
  return Matrix(m.m[0]);
}

Matrix PerspectiveLH(float fov, float ratio, float near, float far) {
  auto ans = DirectX::XMMatrixPerspectiveFovLH(fov, ratio, near, far);
  DirectX::XMFLOAT4X4 m;
  DirectX::XMStoreFloat4x4(&m, ans);
  return Matrix(m.m[0]);
}
#elif defined(IXR_MATH_BACKEND_GLM)

#else
float Angular(const Position &a, const Position &b) {
  float cos_theta = Quaternion(a).Dot(b) / a.Length() / b.Length();
  return acosf(cos_theta);
}

Matrix LookToLH(const Position &eye, const Position &at, const Position &up) {
  auto r2 = at / at.Length();
  auto r0 = up * r2;
  r0 = r0 / r0.Length();
  auto r1 = r2 * r0;
  auto d0 = Quaternion(r0).Dot(Quaternion(-eye));
  auto d1 = Quaternion(r1).Dot(Quaternion(-eye));
  auto d2 = Quaternion(r2).Dot(Quaternion(-eye));
  float m[] = {r0.X(), r1.X(), r2.X(), 0, r0.Y(), r1.Y(), r2.Y(), 0,
               r0.Z(), r1.Z(), r2.Z(), 0, d0,     d1,     d2,     1.0f};
  return Matrix(m);
}

Matrix PerspectiveLH(float fov, float ratio, float near, float far) {
  float sin_fov = sinf(0.5f * fov);
  float cos_fov = cosf(0.5f * fov);
  float h = cos_fov / sin_fov;
  float w = h / ratio;
  float range = far / (far - near);
  float m[] = {w, 0, 0, 0, 0, h, 0, 0, 0, 0, range, 1.f, 0, 0, -range * near,
               0};
  return Matrix(m);
}
#endif
}  // namespace math
}  // namespace engine
}  // namespace ixr
// Backend include
#ifdef IXR_MATH_BACKEND_DIRECTXMATH
#include "backend/directxmath/matrix.inl"
#include "backend/directxmath/position.inl"
#include "backend/directxmath/rotation.inl"
#elif defined(IXR_MATH_BACKEND_GLM)
#include "backend/glm/matrix.inl"
#include "backend/glm/position.inl"
#include "backend/glm/rotation.inl"
#else
#include "backend/basic/matrix.inl"
#include "backend/basic/position.inl"
#include "backend/basic/rotation.inl"
#endif
