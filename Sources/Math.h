/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __MATH_H__
#define __MATH_H__
#include <iostream>

#define mathMax(f, s) (f > s ? f : s)
#define mathMin(f, s) (f > s ? s : f)
#define mathClamp(value, min, max) (value > max ? max : value < min ? min : value)
#define PI 3.14159265359

struct Vector3
{
  Vector3() : x(0), y(0), z(0) {}

  Vector3(float val) : x(val), y(val), z(val) {}

  Vector3(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}

  float Length() const;

  static Vector3 cross(const Vector3& v1, const Vector3& v2);

  static float dot(const Vector3& v1, const Vector3& v2);

  static void Normalize(Vector3& vec);

  Vector3 GetNormalized() const;


  static Vector3 ZeroVector()
  {
    Vector3(0.f);
  }

  Vector3 operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  Vector3 operator-(const Vector3& rhs) const
  {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  Vector3 operator+(const Vector3& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  void operator=(const Vector3& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
  }

  void operator+=(const Vector3& rhs)
  {
    x = x + rhs.x;
    y = y + rhs.y;
    z = z + rhs.z;
  }

  Vector3 operator*(float val) const
  {
    return Vector3(x * val, y * val, z * val);
  }

  friend Vector3 operator*(float val, const Vector3& rhs)
  {
    return Vector3(rhs.x * val, rhs.y * val, rhs.z * val);
  }

  Vector3 operator*(const Vector3& rhs) const
  {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
  }

  Vector3 operator/(float val) const
  {
    return Vector3(x / val, y / val, z / val);
  }

  Vector3 operator/(const Vector3& rhs) const
  {
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
  }

  void operator/=(const float& val)
  {
    x /= val;
    y /= val;
    z /= val;
  }

  bool operator==(const Vector3& val) const
  {
    return x == val.x && y == val.y && z == val.z;
  }

  bool operator!=(const Vector3& val) const
  {
    return x != val.x && y != val.y && z != val.z;
  }

  friend std::ostream& operator<<(std::ostream& out, const Vector3& val)
  {
    return out << "[" << val.x << ", " << val.y << ", " << val.z << "]";
  }

  float x, y, z;
};

struct Vector3i
{
  Vector3i() : x(0), y(0), z(0) {}

  Vector3i(const Vector3& vec3f)
  {
    x = (int)vec3f.x;
    y = (int)vec3f.y;
    z = (int)vec3f.z;
  }

  int x, y, z;
};

struct Vector4
{
  float x, y, z, w;
};

class Math
{
public:
  //static float Determinant(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d);
  static float Determinant(const Vector3& a, const Vector3& b, const Vector3& c);

};

#endif
