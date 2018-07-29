#include <math.h>
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
  float cosp2 = cosf(pitch / 2.f), sinp2 = sinf(pitch / 2.0f);
  float cosy2 = cosf(yaw / 2.0f), siny2 = sinf(yaw / 2.0f);
  float cosr2 = cosf(roll / 2.0f), sinr2 = sinf(roll / 2.0f);
  w_ = cosr2 * cosp2 * cosy2 + sinr2 * sinp2 * siny2;
  x_ = cosr2 * sinp2 * cosy2 + sinr2 * cosp2 * siny2;
  y_ = cosr2 * cosp2 * siny2 - sinr2 * sinp2 * cosy2;
  z_ = sinr2 * cosp2 * cosy2 - cosr2 * sinp2 * siny2;
}

Quaternion::Quaternion(Position p, float angle) {
  if (p.Length() == 0) {
    *this = Identity();
    return;
  }
  Position normed = p * (1.0f / p.Length());
  w_ = cosf(0.5f * angle);
  x_ = sinf(0.5f * angle) * normed.X();
  y_ = sinf(0.5f * angle) * normed.Y();
  z_ = sinf(0.5f * angle) * normed.Z();
}

Quaternion::Quaternion(const Position &p) {
  w_ = 0;
  x_ = p.X();
  y_ = p.Y();
  z_ = p.Z();
}

Quaternion Quaternion::Conj() const { return Quaternion(w_, -x_, -y_, -z_); }

float Quaternion::Dot(const Quaternion &rhs) const {
  return x_ * rhs.x_ + y_ * rhs.y_ + z_ * rhs.z_ + w_ * rhs.w_;
}

Quaternion Quaternion::Add(const Quaternion &rhs) const {
  return Quaternion(w_ + rhs.w_, x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Quaternion Quaternion::Mul(const Quaternion &rhs) const {
  return Quaternion(w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_,
                    w_ * rhs.x_ + x_ * rhs.w_ + y_ * rhs.z_ - z_ * rhs.y_,
                    w_ * rhs.y_ - x_ * rhs.z_ + y_ * rhs.w_ + z_ * rhs.x_,
                    w_ * rhs.z_ + x_ * rhs.y_ - y_ * rhs.x_ + z_ * rhs.w_);
}

Quaternion Quaternion::Reciprocal() const {
  return Conj() / (x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
}

Quaternion Quaternion::Normalize() const {
  float n = Norm();
  return Quaternion(w_ / n, x_ / n, y_ / n, z_ / n);
}

bool Quaternion::Normalized() const { return fabsf(Norm() - 1.0f) < epsilon; }

float Quaternion::Norm() const {
  return sqrtf(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
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
