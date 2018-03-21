#include "Sphere.h"

bool Sphere::Intersection(const Vector3& e, const Vector3& d, float &t, Vector3 &n, bool shadowCheck) const
{
    float t1, t2;

    Vector3 oMinusC = e - center;

    float minusB = -Vector3::Dot(d, oMinusC);
    float bSquare = std::pow(minusB, 2);
    float fourAC = Vector3::Dot(d, d) * (Vector3::Dot(oMinusC, oMinusC) - std::pow(radius, 2));
    float den = Vector3::Dot(d, d);

    float minusBOverDen = minusB / den;
    float sqrtBSquareMinusFourAcOverDen = sqrt(bSquare - fourAC) / den;

    t1 = minusBOverDen + sqrtBSquareMinusFourAcOverDen;
    t2 = minusBOverDen - sqrtBSquareMinusFourAcOverDen;

    if (t1 > 0 && t2 > 0)
    {
        t = t1 < t2 ? t1 : t2;

        Vector3 p = e + d * t;
        n = (p - center);
        Vector3::Normalize(n);

        return true;
    }
    else if (t1 > 0)
    {
        t = t1;

        Vector3 p = e + d * t;
        n = (p - center);
        Vector3::Normalize(n);

        return true;
    }
    else if (t2 > 0)
    {
        t = t2;

        Vector3 p = e + d * t;
        n = (p - center);
        Vector3::Normalize(n);

        return true;
    }

    return false;
}
