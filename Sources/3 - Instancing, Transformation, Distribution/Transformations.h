/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __TRANSFORMATIONS_H__
#define __TRANSFORMATIONS_H__

#include "Math.h"

struct Scene;

class Transformation
{
public:
  static Vector3 Translate(const Vector3 &p, const Vector3 &t);
  static Vector3 Rotate(const Vector3 &p, const Vector4 &r);
  static Vector3 Scale(const Vector3 &p, const Vector3 &s);

  static Vector3 RotateSphere(const Vector3 &p, const Vector3 &r);
  static Vector3 RotateAroundX(const Vector3 &p, float theta);
  static Vector3 RotateAroundY(const Vector3 &p, float theta);
  static Vector3 RotateAroundZ(const Vector3 &p, float theta);

};

#endif