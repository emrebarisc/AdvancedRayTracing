/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Transformations.h"

Matrix Transformation::GetTranslationMatrix(const Vector3 &translation)
{
  Matrix translationMatrix = Matrix::IdentityMatrix;

  translationMatrix.m[3] = translation.x;
  translationMatrix.m[7] = translation.y;
  translationMatrix.m[11] = translation.z;

  return translationMatrix;
}

Matrix Transformation::GetScalingMatrix(const Vector3 &scaling)
{
  Matrix scalingMatrix = Matrix::IdentityMatrix;
  scalingMatrix.m[0] = scaling.x;
  scalingMatrix.m[5] = scaling.y;
  scalingMatrix.m[10] = scaling.z;

  return scalingMatrix;
}

Matrix Transformation::GetRotationMatrix(const Vector4 &rotation)
{
  Matrix out = Matrix::IdentityMatrix;
  if(rotation.y != 0) out = out * GetRotationAroundXMatrix(rotation.x * rotation.y);
  if(rotation.z != 0) out = out * GetRotationAroundYMatrix(rotation.x * rotation.z);
  if(rotation.w != 0) out = out * GetRotationAroundZMatrix(rotation.x * rotation.w);

  return out;
}

Matrix Transformation::GetRotationAroundXMatrix(float degree)
{
  float radian = DEGREE_TO_RADIAN(degree);

  Matrix xRotation = Matrix::IdentityMatrix;
  float cosTheta = cos(radian);
  float sinTheta = sin(radian);
  xRotation.m[5] = cosTheta;
  xRotation.m[6] = -sinTheta;
  xRotation.m[9] = sinTheta;
  xRotation.m[10] = cosTheta;

  return xRotation;
}

Matrix Transformation::GetRotationAroundYMatrix(float degree)
{
  float radian = DEGREE_TO_RADIAN(degree);

  Matrix yRotation = Matrix::IdentityMatrix;
  float cosTheta = cos(radian);
  float sinTheta = sin(radian);
  yRotation.m[0] = cosTheta;
  yRotation.m[2] = sinTheta;
  yRotation.m[8] = -sinTheta;
  yRotation.m[10] = cosTheta;

  return yRotation;
}

Matrix Transformation::GetRotationAroundZMatrix(float degree)
{
  float radian = DEGREE_TO_RADIAN(degree);

  Matrix zRotation = Matrix::IdentityMatrix;
  float cosTheta = cos(radian);
  float sinTheta = sin(radian);
  zRotation.m[0] = cosTheta;
  zRotation.m[1] = -sinTheta;
  zRotation.m[4] = sinTheta;
  zRotation.m[5] = cosTheta;

  return zRotation;
}

Vector3 Transformation::Translate(const Vector3 &p, const Vector3 &t)
{
  Matrix translationMatrix = Matrix::IdentityMatrix;

  translationMatrix.m[3] = t.x;
  translationMatrix.m[7] = t.y;
  translationMatrix.m[11] = t.z;

  return Vector3(translationMatrix * Vector4(p));
}

Vector3 Transformation::Rotate(const Vector3 &p, const Vector4 &r)
{
  float theta = DEGREE_TO_RADIAN(r.x);

  Vector3 result(p);
  if(r.y != 0) result = RotateAroundX(result, theta * r.y);
  if(r.z != 0) result = RotateAroundY(result, theta * r.z);
  if(r.w != 0) result = RotateAroundZ(result, theta * r.w);

/*   if(result.x >= -EPSILON && result.x <= EPSILON) result.x = 0;
  if(result.y >= -EPSILON && result.y <= EPSILON) result.y = 0;
  if(result.z >= -EPSILON && result.z <= EPSILON) result.z = 0; */

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
  Matrix scalingMatrix = Matrix::IdentityMatrix;
  scalingMatrix.m[0] = s.x;
  scalingMatrix.m[5] = s.y;
  scalingMatrix.m[10] = s.z;
  return Vector3(scalingMatrix * Vector4(p));
}

Vector3 Transformation::RotateSphere(const Vector3 &p, const Vector3 &r)
{
  Vector3 result(p);
  result = RotateAroundX(result, PI - DEGREE_TO_RADIAN(r.x));
  result = RotateAroundY(result, PI - DEGREE_TO_RADIAN(r.y));
  result = RotateAroundZ(result, PI - DEGREE_TO_RADIAN(r.z));
  return result;
}