#include "math/matrix.h"
#include "math/rotation.h"

namespace ixr {
namespace engine {
namespace math {
Matrix::Matrix() {}

Matrix::Matrix(const float *data) { memcpy(m_, data, sizeof(m_)); }

Matrix::Matrix(const Quaternion &q) {
  float xx = q.X() * q.X();
  float xy = q.X() * q.Y();
  float xz = q.X() * q.Z();
  float xw = q.X() * q.W();
  float yy = q.Y() * q.Y();
  float yz = q.Y() * q.Z();
  float yw = q.Y() * q.W();
  float zz = q.Z() * q.Z();
  float zw = q.Z() * q.W();
  m_[0] = (1 - 2 * (yy + zz));
  m_[1] = (2 * (xy - zw));
  m_[2] = (2 * (xz + yw));
  m_[4] = (2 * (xy + zw));
  m_[5] = (1 - 2 * (xx + zz));
  m_[6] = (2 * (yz - xw));
  m_[8] = (2 * (xz - yw));
  m_[9] = (2 * (yz + xw));
  m_[10] = (1 - 2 * (xx + yy));
  m_[3] = m_[7] = m_[11] = m_[12] = m_[13] = m_[14] = 0;
  m_[15] = 1;
}

Matrix::~Matrix() {}

Matrix Matrix::Mul(const Matrix &rhs) {
  float ans[16];
  for (int i = 0; i < 4; i++) {
    Quaternion row(m_[i * 4 + 3], m_[i * 4], m_[i * 4 + 1], m_[i * 4 + 2]);
    for (int j = 0; j < 4; j++) {
      Quaternion col(rhs(3, j), rhs(0, j), rhs(1, j), rhs(2, j));
      ans[i * 4 + j] = row.Dot(col);
    }
  }
  return Matrix(ans);
}

Matrix Matrix::Add(const Matrix &rhs) {
  float ans[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ans[i * 4 + j] = m_[i * 4 + j] + rhs(i, j);
    }
  }
  return Matrix(ans);
}

Matrix Matrix::Scale(float scalar) const {
  float ans[16];
  for (int i = 0; i < 16; i++) {
    ans[i] = m_[i] * scalar;
  }
  return Matrix(ans);
}

Matrix Matrix::Transpose() const {
  float ans[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      ans[i * 4 + j] = m_[j * 4 + i];
    }
  }
  return Matrix(ans);
}

float Matrix::Det() const {
  float ans = 0;
  ans += m_[0] * m_[5] * m_[10] * m_[15];
  ans += m_[1] * m_[6] * m_[11] * m_[12];
  ans += m_[2] * m_[7] * m_[8] * m_[13];
  ans += m_[3] * m_[4] * m_[9] * m_[14];
  ans -= m_[0] * m_[13] * m_[10] * m_[7];
  ans -= m_[4] * m_[1] * m_[14] * m_[11];
  ans -= m_[8] * m_[5] * m_[2] * m_[15];
  ans -= m_[12] * m_[9] * m_[6] * m_[3];
  return ans;
}

Matrix &Matrix::operator=(const Matrix &rhs) {
  if (this != &rhs) {
    memcpy(this, &rhs, sizeof(rhs));
  }
  return *this;
}

Matrix Matrix::operator+(const Matrix &rhs) { return Add(rhs); }

Matrix Matrix::operator-(const Matrix &rhs) { return Add(rhs.Scale(-1.f)); }

Matrix Matrix::operator*(const Matrix &rhs) { return Mul(rhs); }

Matrix Matrix::operator*(float scalar) { return Scale(scalar); }

float Matrix::operator()(const int r, const int c) const {
  return m_[r * 4 + c];
}

Matrix Matrix::Identity() {
  const float ans[] = {1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f};
  return Matrix(ans);
}
}  // namespace math
}  // namespace engine
}  // namespace ixr
