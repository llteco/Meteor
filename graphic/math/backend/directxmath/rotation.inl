#include <DirectXMath.h>
#include "math/position.h"
#include "math/rotation.h"

namespace ixr {
namespace engine {
namespace math {
Quaternion::Quaternion() { x_ = y_ = z_ = w_ = 0; }

Quaternion::Quaternion(float w, float x, float y, float z) {
  x_ = x;
  y_ = y;
  z_ = z;
  w_ = w;
}

Quaternion::Quaternion(float pitch, float yaw, float roll) {
  auto q = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
  w_ = DirectX::XMVectorGetW(q);
  x_ = DirectX::XMVectorGetX(q);
  y_ = DirectX::XMVectorGetY(q);
  z_ = DirectX::XMVectorGetZ(q);
}

Quaternion::Quaternion(Position p, float angle) {
  auto axis = PositionToVector(p);
  if (angle < EPSILON) {
    w_ = 1.f;
    x_ = y_ = z_ = 0;
    return;
  }
  auto q = DirectX::XMQuaternionRotationAxis(axis, angle);
  w_ = DirectX::XMVectorGetW(q);
  x_ = DirectX::XMVectorGetX(q);
  y_ = DirectX::XMVectorGetY(q);
  z_ = DirectX::XMVectorGetZ(q);
}

Quaternion::Quaternion(const Position &p) {
  w_ = 0;
  x_ = p.X();
  y_ = p.Y();
  z_ = p.Z();
}

Quaternion Quaternion::Conj() const {
  auto q = QuaternionToVector(*this);
  q = DirectX::XMQuaternionConjugate(q);
  return Quaternion(DirectX::XMVectorGetW(q), DirectX::XMVectorGetX(q),
                    DirectX::XMVectorGetY(q), DirectX::XMVectorGetZ(q));
}

float Quaternion::Dot(const Quaternion &rhs) const {
  auto a = QuaternionToVector(*this);
  auto b = QuaternionToVector(rhs);
  auto ans = DirectX::XMQuaternionDot(a, b);
  return ans.m128_f32[0];
}

Quaternion Quaternion::Add(const Quaternion &rhs) const {
  return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Quaternion Quaternion::Mul(const Quaternion &rhs) const {
  auto a = QuaternionToVector(*this);
  auto b = QuaternionToVector(rhs);
  auto ans = DirectX::XMQuaternionMultiply(a, b);
  return Quaternion(DirectX::XMVectorGetW(ans), DirectX::XMVectorGetX(ans),
                    DirectX::XMVectorGetY(ans), DirectX::XMVectorGetZ(ans));
}

Quaternion Quaternion::Reciprocal() const {
  return Conj() / (x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
}

Quaternion Quaternion::Normalize() const {
  auto ans = DirectX::XMQuaternionNormalize(QuaternionToVector(*this));
  return Quaternion(DirectX::XMVectorGetW(ans), DirectX::XMVectorGetX(ans),
                    DirectX::XMVectorGetY(ans), DirectX::XMVectorGetZ(ans));
}

bool Quaternion::Normalized() const { return fabsf(Norm() - 1.0f) < EPSILON; }

float Quaternion::Norm() const {
  auto ans = DirectX::XMQuaternionLength(QuaternionToVector(*this));
  return ans.m128_f32[0];
}

Quaternion Quaternion::Scale(float s) const {
  return Quaternion(w_ * s, x_ * s, y_ * s, z_ * s);
}

Quaternion Quaternion::operator-() const {
  return Quaternion(-w_, -x_, -y_, -z_);
}

Quaternion Quaternion::operator+(const Quaternion &rhs) const {
  return Add(rhs);
}

Quaternion Quaternion::operator-(const Quaternion &rhs) const {
  return Add(-rhs);
}

Quaternion Quaternion::operator*(const Quaternion &rhs) const {
  return Mul(rhs);
}

Quaternion Quaternion::operator*(const float &scalar) const {
  return Scale(scalar);
}

Quaternion Quaternion::operator/(const float &scalar) const {
  return Scale(1.0f / scalar);
}

Quaternion &Quaternion::operator=(const Quaternion &rhs) {
  if (this != &rhs) {
    w_ = rhs.w_;
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
  }
  return *this;
}

Quaternion &Quaternion::operator=(const Position &rhs) {
  w_ = 0;
  x_ = rhs.X();
  y_ = rhs.Y();
  z_ = rhs.Z();
  return *this;
}

const Quaternion Quaternion::Identity() { return Quaternion(1, 0, 0, 0); }
}  // namespace math
}  // namespace engine
}  // namespace ixr
