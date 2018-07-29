/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : rotation (quaternion and vector4)
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MATH_ROTATION_H_
#define IXR_ENGINE_MATH_ROTATION_H_

namespace ixr {
namespace engine {
namespace math {
class Position;

class IXR_ENGINE_API Quaternion {
 public:
  Quaternion();
  Quaternion(float w, float x, float y, float z);
  Quaternion(float pitch, float yaw, float roll);
  Quaternion(Position p, float angle);
  Quaternion(const Position &p);

  float W() const { return w_; }
  float X() const { return x_; }
  float Y() const { return y_; }
  float Z() const { return z_; }

  Quaternion Conj() const;

  float Dot(const Quaternion &rhs) const;

  Quaternion Add(const Quaternion &rhs) const;

  Quaternion Mul(const Quaternion &rhs) const;

  Quaternion Reciprocal() const;

  Quaternion Normalize() const;

  bool Normalized() const;

  float Norm() const;

  Quaternion Scale(float s) const;

  Quaternion operator-() const;
  Quaternion operator+(const Quaternion &rhs) const;
  Quaternion operator-(const Quaternion &rhs) const;
  Quaternion operator*(const Quaternion &rhs) const;
  Quaternion operator*(const float &scalar) const;
  Quaternion operator/(const float &scalar) const;
  Quaternion &operator=(const Quaternion &rhs);
  Quaternion &operator=(const Position &rhs);

  static const Quaternion Identity();

 private:
  float x_, y_, z_, w_;
};
}  // namespace math
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MATH_ROTATION_H_
