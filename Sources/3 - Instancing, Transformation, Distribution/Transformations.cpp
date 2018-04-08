/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Transformations.h"

#include "Math.h"
#include "Matrix.h"

Vector3 Transformation::Translate(const Vector3 &p, const Vector3 &t)
{
  Matrix translateMatrix = Matrix::IdentityMatrix;

  translateMatrix.m[3] = t.x;
  translateMatrix.m[7] = t.y;
  translateMatrix.m[11] = t.z;

  return Vector3(translateMatrix * Vector4(p));
}

Vector3 Transformation::Rotate(const Vector3 &p, const Vector4 &r)
{
  float theta = DEGREE_TO_RADIAN(r.x);

  Vector3 result(p);
  if(r.y != 0) result = RotateAroundX(result, theta * r.y);
  if(r.z != 0) result = RotateAroundY(result, theta * r.z);
  if(r.w != 0) result = RotateAroundZ(result, theta * r.w);

  if(result.x >= -EPSILON && result.x <= EPSILON) result.x = 0;
  if(result.y >= -EPSILON && result.y <= EPSILON) result.y = 0;
  if(result.z >= -EPSILON && result.z <= EPSILON) result.z = 0;

  return result;
}

Vector3 Transformation::RotateAroundX(const Vector3 &p, float theta)
{
  Matrix rotateMatrix = Matrix::IdentityMatrix;
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  rotateMatrix.m[5] = cosTheta;
  rotateMatrix.m[6] = -sinTheta;
  rotateMatrix.m[9] = sinTheta;
  rotateMatrix.m[10] = cosTheta;
  return Vector3(rotateMatrix * Vector4(p));
}

Vector3 Transformation::RotateAroundY(const Vector3 &p, float theta)
{
  Matrix rotateMatrix = Matrix::IdentityMatrix;
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  rotateMatrix.m[0] = cosTheta;
  rotateMatrix.m[2] = sinTheta;
  rotateMatrix.m[8] = -sinTheta;
  rotateMatrix.m[10] = cosTheta;
  return Vector3(rotateMatrix * Vector4(p));
}

Vector3 Transformation::RotateAroundZ(const Vector3 &p, float theta)
{
  Matrix rotateMatrix = Matrix::IdentityMatrix;
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  rotateMatrix.m[0] = cosTheta;
  rotateMatrix.m[1] = -sinTheta;
  rotateMatrix.m[4] = sinTheta;
  rotateMatrix.m[5] = cosTheta;
  return Vector3(rotateMatrix * Vector4(p));
}

Vector3 Transformation::Scale(const Vector3 &p, const Vector3 &s)
{
  Matrix scaleMatrix = Matrix::IdentityMatrix;
  scaleMatrix.m[0] = s.x;
  scaleMatrix.m[5] = s.y;
  scaleMatrix.m[10] = s.z;
  return Vector3(scaleMatrix * Vector4(p));
}

Vector3 Transformation::RotateSphere(const Vector3 &p, const Vector3 &r)
{
  Vector3 result(p);
  result = RotateAroundX(result, PI - (r.x * PI / 180.f));
  result = RotateAroundY(result, PI - (r.y * PI / 180.f));
  result = RotateAroundZ(result, PI - (r.z * PI / 180.f));
  return result;
}