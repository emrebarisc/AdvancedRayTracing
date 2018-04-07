/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Math.h"

Vector3 Vector3::GetNormalized() const
{
    return *this / this->Length();
}
