/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __MATH_H__
#define __MATH_H__

#include <iostream>
#include <cmath>
#include <stdexcept>

#define mathMax(f, s) (f > s ? f : s)
#define mathMin(f, s) (f > s ? s : f)
#define mathClamp(value, min, max) (value > max ? max : value < min ? min : value)
#define PI 3.14159265359
#define EPSILON 0.00001

struct Vector3
{
  Vector3() : x(0), y(0), z(0) {}

  Vector3(float val) : x(val), y(val), z(val) {}

  Vector3(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}

  inline float Length() const
  {
    return sqrt(std::pow(x, 2) + std::pow(y, 2) + std::pow(z, 2));
  }

  static inline Vector3 Cross(const Vector3& v1, const Vector3& v2)
  {
    return Vector3(v1.y * v2.z - v1.z * v2.y,
                   v1.z * v2.x - v1.x * v2.z,
                   v1.x * v2.y - v1.y * v2.x);
  }

  static inline float Dot(const Vector3& v1, const Vector3& v2)
  {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
  }

  static inline void Normalize(Vector3& vec)
  {
    float vecLen = vec.Length();
    if(vecLen == 0) 
    {
      //throw std::runtime_error("Error: Vector length is 0. Division by zero.");
      std::cerr << "Division by zero." << std::endl;
      return;
    }

    vec = vec / vecLen;
  }

  Vector3 GetNormalized() const;

  static inline Vector3 ZeroVector()
  {
    Vector3(0.f);
  }

  inline Vector3 operator-() const
  {
    return Vector3(-x, -y, -z);
  }

  inline Vector3 operator-(const Vector3& rhs) const
  {
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
  }

  inline Vector3 operator+(const Vector3& rhs) const
  {
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
  }

  inline void operator=(const Vector3& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
  }

  inline void operator+=(const Vector3& rhs)
  {
    x = x + rhs.x;
    y = y + rhs.y;
    z = z + rhs.z;
  }

  inline Vector3 operator*(float val) const
  {
    return Vector3(x * val, y * val, z * val);
  }

  inline friend Vector3 operator*(float val, const Vector3& rhs)
  {
    return Vector3(rhs.x * val, rhs.y * val, rhs.z * val);
  }

  inline Vector3 operator*(const Vector3& rhs) const
  {
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
  }

  inline Vector3 operator/(float val) const
  {
    return Vector3(x / val, y / val, z / val);
  }

  inline Vector3 operator/(const Vector3& rhs) const
  {
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
  }

  inline void operator/=(const float& val)
  {
    x /= val;
    y /= val;
    z /= val;
  }

  inline bool operator==(const Vector3& val) const
  {
    return x == val.x && y == val.y && z == val.z;
  }

  inline bool operator!=(const Vector3& val) const
  {
    return x != val.x && y != val.y && z != val.z;
  }

  inline friend std::ostream& operator<<(std::ostream& out, const Vector3& val)
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

  static inline float Determinant(const Vector3& a, const Vector3& b, const Vector3& c)
  {
    return    a.x * ((b.y * c.z) - (c.y * b.z))
            + a.y * ((c.x * b.z) - (b.x * c.z))
            + a.z * ((b.x * c.y) - (c.x * b.y));
  }

  static inline float Determinant(const Vector4& a, const Vector4& b, const Vector4& c, const Vector4& d)
  {

    return a.x * Determinant(Vector3(b.y, b.z, b.w), Vector3(c.y, c.z, c.w), Vector3(d.y, d.z, d.w))
         - a.y * Determinant(Vector3(b.x, b.z, b.w), Vector3(c.x, c.z, c.w), Vector3(d.x, d.z, d.w))
         + a.z * Determinant(Vector3(b.x, b.y, b.w), Vector3(c.x, c.y, c.w), Vector3(d.x, d.y, d.w))
         - a.w * Determinant(Vector3(b.x, b.y, b.z), Vector3(c.x, c.y, c.z), Vector3(d.x, d.y, d.z));
  }
};

#endif