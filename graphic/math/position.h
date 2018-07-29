/********************************************************************
Copyright 2018 Intel Corp. All Rights Reserved.
Description : position (vector3)
Author      : Wenyi Tang
Email       : wenyi.tang@intel.com
Created     : Feb. 28th, 2018
********************************************************************/
#ifndef IXR_ENGINE_MATH_POSITION_H_
#define IXR_ENGINE_MATH_POSITION_H_

namespace ixr {
namespace engine {
namespace math {
class Quaternion;

class IXR_ENGINE_API Position {
 public:
  Position();

  Position(float x, float y, float z);

  float X() const { return x_; }
  float Y() const { return y_; }
  float Z() const { return z_; }

  void Translate(float dx, float dy, float dz);

  void Scale(float fx, float fy, float fz);

  void Scale(float factor);

  void Rotate(const Quaternion &q);

  float Length() const;

  Position operator-() const;
  Position operator+(const Position &rhs) const;
  Position operator-(const Position &rhs) const;
  Position operator*(const Position &rhs) const;
  Position operator*(const float &x) const;
  Position operator/(const float &x) const;
  Position &operator=(const Position &rhs);
  Position &operator+=(const Position &rhs);
  bool operator==(const Position &rhs) const;

 private:
  float x_, y_, z_;
};
}  // namespace math
}  // namespace engine
}  // namespace ixr
#endif  // IXR_ENGINE_MATH_POSITION_H_
