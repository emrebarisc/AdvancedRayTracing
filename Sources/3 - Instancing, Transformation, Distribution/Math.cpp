/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Math.h"

const Vector3 Vector3::ZeroVector = Vector3::ZeroVector;

Vector3::Vector3(const Vector4& rhs) : x(rhs.x), y(rhs.y), z(rhs.z)
{
    
}

Vector3 Vector3::GetNormalized() const
{
    return *this / this->Length();
}
