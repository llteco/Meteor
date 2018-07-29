#include <math.h>
#include "math/position.h"

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
  Quaternion vec(0, x_, y_, z_);
  vec = q * vec * q.Conj();
  x_ = vec.X();
  y_ = vec.Y();
  z_ = vec.Z();
}

float Position::Length() const { return sqrtf(x_ * x_ + y_ * y_ + z_ * z_); }

Position Position::operator-() const { return Position(-x_, -y_, -z_); }

Position Position::operator+(const Position &rhs) const {
  return Position(x_ + rhs.x_, y_ + rhs.y_, z_ + rhs.z_);
}

Position Position::operator-(const Position &rhs) const {
  return Position(x_ - rhs.x_, y_ - rhs.y_, z_ - rhs.z_);
}

Position Position::operator*(const Position &rhs) const {
  return Position(y_ * rhs.z_ - z_ * rhs.y_, z_ * rhs.x_ - x_ * rhs.z_,
                  x_ * rhs.y_ - y_ * rhs.x_);
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
  return (fabsf(x_ - rhs.x_) < epsilon) && (fabsf(y_ - rhs.y_) < epsilon) &&
         (fabsf(z_ - rhs.z_) < epsilon);
}
}  // namespace math
}  // namespace engine
}  // namespace ixr
