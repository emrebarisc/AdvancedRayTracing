/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Camera.h"

void Camera::SetNearPlaneWrtDOF()
{
    float proportion = focusDistance / nearDistance;

    nearPlane.x *= proportion;
    nearPlane.y *= proportion;
    nearPlane.z *= proportion;
    nearPlane.w *= proportion;

    nearDistance = focusDistance;
}