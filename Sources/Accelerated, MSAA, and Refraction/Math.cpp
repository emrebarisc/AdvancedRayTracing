/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Math.h"

const Vector3 Vector3::ZeroVector = Vector3::ZeroVector;

Vector3 Vector3::GetNormalized() const
{
    return *this / this->Length();
}
