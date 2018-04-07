/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "BoundingVolume.h"

bool BoundingVolume::Intersection(const Vector3& e, const Vector3& d, float& t, Vector3& n, bool shadowCheck) const
{
    // Liang-Barsky Algorithm

    Vector3 invD = Vector3(1 / d.x, 1 / d.y, 1 / d.z);
    
    float tmin, tmax, tymin, tymax, tzmin, tzmax;

    bool xLessThanZero, yLessThanZero, zLessThanZero;

    xLessThanZero = invD.x < 0;
    yLessThanZero = invD.y < 0;
    zLessThanZero = invD.z < 0;

    Vector3 bounds[2] = {min, max};

    tmin = (bounds[xLessThanZero].x - e.x) * invD.x; 
    tmax = (bounds[1-xLessThanZero].x - e.x) * invD.x; 
    tymin = (bounds[yLessThanZero].y - e.y) * invD.y; 
    tymax = (bounds[1 - yLessThanZero].y - e.y) * invD.y; 
 
    if ((tmin > tymax) || (tymin > tmax)) 
        return false; 
    if (tymin > tmin) 
        tmin = tymin; 
    if (tymax < tmax) 
        tmax = tymax; 
 
    tzmin = (bounds[zLessThanZero].z - e.z) * invD.z; 
    tzmax = (bounds[1-zLessThanZero].z - e.z) * invD.z; 
 
    if ((tmin > tzmax) || (tzmin > tmax)) 
        return false; 
    if (tzmin > tmin) 
        tmin = tzmin; 
    if (tzmax < tmax) 
        tmax = tzmax; 
 
    float tLeft, tRight;
    Vector3 nLeft, nRight;
    bool leftIntersection = false, rightIntersection = false;

    if(left) 
    {
        leftIntersection = left->Intersection(e, d, tLeft, nLeft, shadowCheck);
    }

    if(right)
    {
        rightIntersection = right->Intersection(e, d, tRight, nRight, shadowCheck);
    }
    
    if(leftIntersection && rightIntersection)
    {
        if(tLeft < tRight)
        {
            t = tLeft;
            n = nLeft;
        }
        else
        {
            t = tRight;
            n = nRight;
        }
    }
    else if(leftIntersection)
    {
        t = tLeft;
        n = nLeft;
    }
    else if(rightIntersection)
    {
        t = tRight;
        n = nRight;
    }

    return leftIntersection || rightIntersection;
}

Vector3 BoundingVolume::GetCentroid()
{
    return Vector3( min.x + (max.x - min.x) * 0.5f,
                    min.y + (max.y - min.y) * 0.5f,
                    min.z + (max.z - min.z) * 0.5f);
}