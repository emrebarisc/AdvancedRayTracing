#include "Sphere.h"

bool Sphere::Intersection(const Vector3& e, const Vector3& d, float &t)
{
    float t1, t2;

    Vector3 oMinusC = e - center;

    float minusB = -Vector3::dot(d, oMinusC);
    float bSquare = std::pow(minusB, 2);
    float fourAC = Vector3::dot(d, d) * (Vector3::dot(oMinusC, oMinusC) - std::pow(radius, 2));
    float den = Vector3::dot(d, d);

    float minusBOverDen = minusB / den;
    float sqrtBSquareMinusFourAcOverDen = sqrt(bSquare - fourAC) / den;

    t1 = minusBOverDen + sqrtBSquareMinusFourAcOverDen;
    t2 = minusBOverDen - sqrtBSquareMinusFourAcOverDen;

    if (t1 > 0 && t2 > 0)
    {
        t = t1 < t2 ? t1 : t2;
        return true;
    }
    else if (t1 > 0)
    {
        t = t1;
        return true;
    }
    else if (t2 > 0)
    {
        t = t2;
        return true;
    }

    return false;
}