#include <DirectXMath.h>
#include "math/position.h"
#include "math/rotation.h"

namespace ixr {
namespace engine {
namespace math {
Position::Position() { x_ = y_ = z_ = 0; }

Position::Position(float x, float y, float z) {
  x_ = x;
  y_ = y;
  z_ = z;
}

void Position::Translate(float dx, float dy, float dz) {
  x_ += dx;
  y_ += dy;
  z_ += dz;
}

void Position::Scale(float fx, float fy, float fz) {
  x_ *= fx;
  y_ *= fy;
  z_ *= fz;
}

void Position::Scale(float factor) { Scale(factor, factor, factor); }

void Position::Rotate(const Quaternion &q) {
  auto ans =
      DirectX::XMVector3Rotate(PositionToPoint(*this), QuaternionToVector(q));
  x_ = DirectX::XMVectorGetX(ans);
  y_ = DirectX::XMVectorGetY(ans);
  z_ = DirectX::XMVectorGetZ(ans);
}

float Position::Length() const {
  auto ans = DirectX::XMVector3Length(PositionToPoint(*this));
  return ans.m128_f32[0];
}

Position Position::operator-() const { return Position(-x_, -y_, -z_); }

Position Position::operator+(const Position &rhs) const {
  return Position(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Position Position::operator-(const Position &rhs) const {
  return Position(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
}

Position Position::operator*(const Position &rhs) const {
  auto a = PositionToPoint(*this);
  auto b = PositionToPoint(rhs);
  auto ans = DirectX::XMVector3Cross(a, b);
  return Position(DirectX::XMVectorGetX(ans), DirectX::XMVectorGetY(ans),
                  DirectX::XMVectorGetZ(ans));
}

Position Position::operator*(const float &x) const {
  Position r(*this);
  r.Scale(x);
  return r;
}

Position Position::operator/(const float &x) const {
  Position r(*this);
  r.Scale(1.0f / x);
  return r;
}

Position &Position::operator=(const Position &rhs) {
  if (this != &rhs) {
    x_ = rhs.x_;
    y_ = rhs.y_;
    z_ = rhs.z_;
  }
  return *this;
}

Position &Position::operator+=(const Position &rhs) {
  x_ += rhs.x_;
  y_ += rhs.y_;
  z_ += rhs.z_;
  return *this;
}

bool Position::operator==(const Position &rhs) const {
  return (fabsf(x_ - rhs.x_) < EPSILON) && (fabsf(y_ - rhs.y_) < EPSILON) &&
         (fabsf(z_ - rhs.z_) < EPSILON);
}
}  // namespace math
}  // namespace engine
}  // namespace ixr
