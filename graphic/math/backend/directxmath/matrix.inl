#include <DirectXMath.h>
#include "math/matrix.h"
#include "math/rotation.h"

namespace ixr {
namespace engine {
namespace math {
Matrix::Matrix() {}

Matrix::Matrix(const float *data) {
  auto m =
      DirectX::XMMatrixSet(data[0], data[1], data[2], data[3], data[4], data[5],
                           data[6], data[7], data[8], data[9], data[10],
                           data[11], data[12], data[13], data[14], data[15]);
  memcpy(m_, &m, sizeof(m_));
}

Matrix::Matrix(const Quaternion &q) {
  auto m = DirectX::XMMatrixRotationQuaternion(QuaternionToVector(q));
  memcpy(m_, &m, sizeof(m_));
}

Matrix::~Matrix() {}

Matrix Matrix::Mul(const Matrix &rhs) {
  auto a = DirectX::XMMatrixSet(m_[0], m_[1], m_[2], m_[3], m_[4], m_[5], m_[6],
                                m_[7], m_[8], m_[9], m_[10], m_[11], m_[12],
                                m_[13], m_[14], m_[15]);
  DirectX::XMFLOAT4X4 b;
  memcpy(&b, &rhs, sizeof(b));
  auto bb = DirectX::XMLoadFloat4x4(&b);
  auto ans = DirectX::XMMatrixMultiply(a, bb);
  DirectX::XMStoreFloat4x4(&b, ans);
  return Matrix(b.m[0]);
}

Matrix Matrix::Add(const Matrix &rhs) {
  auto a = DirectX::XMMatrixSet(m_[0], m_[1], m_[2], m_[3], m_[4], m_[5], m_[6],
                                m_[7], m_[8], m_[9], m_[10], m_[11], m_[12],
                                m_[13], m_[14], m_[15]);
  DirectX::XMFLOAT4X4 b;
  memcpy(&b, &rhs, sizeof(b));
  auto bb = DirectX::XMLoadFloat4x4(&b);
  auto ans = a + bb;
  DirectX::XMStoreFloat4x4(&b, ans);
  return Matrix(b.m[0]);
}

Matrix Matrix::Scale(float scalar) const {
  float ans[16];
  for (int i = 0; i < 16; i++) {
    ans[i] = m_[i] * scalar;
  }
  return Matrix(ans);
}

Matrix Matrix::Transpose() const {
  auto a = DirectX::XMMatrixSet(m_[0], m_[1], m_[2], m_[3], m_[4], m_[5], m_[6],
                                m_[7], m_[8], m_[9], m_[10], m_[11], m_[12],
                                m_[13], m_[14], m_[15]);
  DirectX::XMFLOAT4X4 b;
  auto ans = DirectX::XMMatrixTranspose(a);
  DirectX::XMStoreFloat4x4(&b, ans);
  return Matrix(b.m[0]);
}

float Matrix::Det() const {
  auto a = DirectX::XMMatrixSet(m_[0], m_[1], m_[2], m_[3], m_[4], m_[5], m_[6],
                                m_[7], m_[8], m_[9], m_[10], m_[11], m_[12],
                                m_[13], m_[14], m_[15]);
  auto ans = DirectX::XMMatrixDeterminant(a);
  return ans.m128_f32[0];
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
  auto I = DirectX::XMMatrixIdentity();
  DirectX::XMFLOAT4X4 a;
  DirectX::XMStoreFloat4x4(&a, I);
  return Matrix(a.m[0]);
}
}  // namespace math
}  // namespace engine
}  // namespace ixr
