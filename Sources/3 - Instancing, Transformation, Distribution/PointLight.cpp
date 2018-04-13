/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "PointLight.h"

Vector3 PointLight::GetIntensityAtPosition(const Vector3& lightPosition, const Vector3& positionAt) const
{
    float distance = (positionAt - lightPosition).Length();

    // Inverse square law
    return intensity / (distance * distance);
}