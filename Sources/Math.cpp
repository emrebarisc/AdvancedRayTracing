#include "Math.h"
#include <cmath>
#include <stdexcept>
#include <iostream>

float Vector3::Length() const
{
  return sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2)
{
  return Vector3(v1.y * v2.z - v1.z * v2.y,
               v1.z * v2.x - v1.x * v2.z,
               v1.x * v2.y - v1.y * v2.x);
}

float Vector3::dot(const Vector3& v1, const Vector3& v2)
{
  return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

void Vector3::Normalize(Vector3& vec)
{
  if(vec.Length() == 0)
    throw std::runtime_error("Error: Vector length is 0. Division by zero.");

  vec = vec / vec.Length();
}

Vector3 Vector3::GetNormalized() const
{
    return *this / this->Length();
}

float Math::Determinant(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return   a.x * ((b.y * c.z) - (c.y * b.z))
		     + a.y * ((c.x * b.z) - (b.x * c.z))
		     + a.z * ((b.x * c.y) - (c.x * b.y));
}
