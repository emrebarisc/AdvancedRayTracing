#include "Sphere.h"

#include "Texture.h"
#include "Transformations.h"

void Sphere::CreateBVH()
{
    bvh.root = this;
}

bool Sphere::Intersection(const Ray &ray, float &t, Vector3 &n, float &beta, float &gamma, const ObjectBase ** hitObject, bool shadowCheck) const
{
    float t1, t2;

    Vector3 oMinusC = ray.e - center;

    float minusB = -Vector3::Dot(ray.dir, oMinusC);
    float bSquare = std::pow(minusB, 2);
    float fourAC = Vector3::Dot(ray.dir, ray.dir) * (Vector3::Dot(oMinusC, oMinusC) - std::pow(radius, 2));
    float den = Vector3::Dot(ray.dir, ray.dir);

    float minusBOverDen = minusB / den;
    float sqrtBSquareMinusFourAcOverDen = sqrt(bSquare - fourAC) / den;

    t1 = minusBOverDen + sqrtBSquareMinusFourAcOverDen;
    t2 = minusBOverDen - sqrtBSquareMinusFourAcOverDen;

    if (t1 > 0 && t2 > 0)
    {
        t = t1 < t2 ? t1 : t2;

        Vector3 p = ray.e + ray.dir * t;
        n = (p - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        return true;
    }
    else if (t1 > 0)
    {
        t = t1;

        Vector3 p = ray.e + ray.dir * t;
        n = (p - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        return true;
    }
    else if (t2 > 0)
    {
        t = t2;

        Vector3 p = ray.e + ray.dir * t;
        n = (p - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        return true;
    }

    return false;
}

Vector3 Sphere::GetTextureColorAt(const Vector3 &intersectionPoint, float beta, float gamma) const
{
    if(texture->imagePath == "perlin")
    {
        return texture->GetInterpolatedUV(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
    }
    
    Vector3 worldCenteredPosition = intersectionPoint - center;
    worldCenteredPosition = inverseTransformationMatrix * Vector4(worldCenteredPosition, 1.f);
    
    float theta = acos(worldCenteredPosition.y / radius);
    float phi = atan2(worldCenteredPosition.z, worldCenteredPosition.x);
    float u = (-phi + PI) / (2 * PI);
    float v = theta / PI;

    return texture->GetInterpolatedUV(u, v);
}