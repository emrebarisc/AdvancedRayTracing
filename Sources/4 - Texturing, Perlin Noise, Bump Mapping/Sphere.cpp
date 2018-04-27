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

    bool isIntersecting = false;
    Vector3 intersectionPoint = Vector3::ZeroVector;

    if (t1 > 0 && t2 > 0)
    {
        t = t1 < t2 ? t1 : t2;

        intersectionPoint = ray.e + ray.dir * t;
        n = (intersectionPoint - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        isIntersecting = true;
    }
    else if (t1 > 0)
    {
        t = t1;

        intersectionPoint = ray.e + ray.dir * t;
        n = (intersectionPoint - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        isIntersecting = true;
    }
    else if (t2 > 0)
    {
        t = t2;
        intersectionPoint = ray.e + ray.dir * t;
        n = (intersectionPoint - center);
        n = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(n, 0.f));
        n.Normalize();
        *hitObject = this;

        isIntersecting = true;
    }

    if(isIntersecting)
    {
        if(texture->bumpmap)
        {
            float beta, gamma, u, v;
            GetIntersectingUV(intersectionPoint, beta, gamma, u, v);

            float tetha = v * PI;
            float phi = PI - TWO_PI * u;

            float x = radius * sin(tetha) * cos(phi);
            float y = radius * cos(tetha);
            float z = radius * sin(tetha) * sin(phi);

            Vector3 pU = Vector3::ZeroVector;
            pU.x = TWO_PI * z;
            pU.y = 0.f;
            pU.z = -TWO_PI * x;

            Vector3 pV = Vector3::ZeroVector;
            pV.x = PI * y * cos(phi);
            pV.y = -PI * radius * sin(tetha);
            pV.z = PI * y * sin(phi);

            /* if(n.x - Vector3::Cross(pV, pU).GetNormalized().x >= EPSILON || n.x - Vector3::Cross(pV, pU).GetNormalized().x <= -EPSILON || 
               n.y - Vector3::Cross(pV, pU).GetNormalized().y >= EPSILON || n.y - Vector3::Cross(pV, pU).GetNormalized().y <= -EPSILON || 
               n.z - Vector3::Cross(pV, pU).GetNormalized().z >= EPSILON || n.z - Vector3::Cross(pV, pU).GetNormalized().z <= -EPSILON )
            {
                std::cout << "ERROR! n != Pv x Pu" << std::endl;
            } */

            float referenceU = u < texture->width - 1 ? u + EPSILON : u - EPSILON; 
            float referenceV = v < texture->width - 1 ? v + EPSILON : v - EPSILON; 

            Vector3 textureColor = texture->GetInterpolatedUV(u, v);
            Vector3 referenceTextureColor = texture->GetInterpolatedUV(referenceU, v);
            float averageColorDU = (textureColor.x + textureColor.y + textureColor.z) /* / 3.f * texture->bumpmapMultiplier */;
            float averageReferenceDU = (referenceTextureColor.x + referenceTextureColor.y + referenceTextureColor.z)/*  / 3.f * texture->bumpmapMultiplier */;
            float du = (averageReferenceDU - averageColorDU) / 6.f * texture->bumpmapMultiplier;

            referenceTextureColor = texture->GetInterpolatedUV(u, referenceV);
            float averageColorDV = (textureColor.x + textureColor.y + textureColor.z) /* / 3.f * texture->bumpmapMultiplier */;
            float averageReferenceDV = (referenceTextureColor.x + referenceTextureColor.y + referenceTextureColor.z)/* / 3.f * texture->bumpmapMultiplier */;
            float dv = (averageReferenceDV - averageColorDV) / 6.f * texture->bumpmapMultiplier;

            Vector3 nPrime = Vector3::Cross(pV + dv * n, pU + du * n);
            //Vector3 nPrime = n + du * Vector3::Cross(pV, n).GetNormalized() + dv * Vector3::Cross(n, pU).GetNormalized();
            nPrime = Vector3(inverseTransformationMatrix.GetTranspose().GetUpper3x3() * Vector4(nPrime, 0.f));
            nPrime.Normalize();
            
            n = nPrime;

        }
        return true;
    }

    return false;
}

void Sphere::GetIntersectingUV(const Vector3 &intersectionPoint, float beta, float gamma, float &u, float &v) const 
{
    Vector3 worldCenteredPosition = intersectionPoint - center;
    worldCenteredPosition = inverseTransformationMatrix * Vector4(worldCenteredPosition, 1.f);
    
    float theta = acos(worldCenteredPosition.y / radius);
    float phi = atan2(worldCenteredPosition.z, worldCenteredPosition.x);
    u = (-phi + PI) / (2 * PI);
    v = theta / PI;
}

Vector3 Sphere::GetTextureColorAt(const Vector3 &intersectionPoint, float beta, float gamma) const
{
    if(texture->imagePath == "perlin")
    {
        return texture->GetInterpolatedUV(intersectionPoint.x, intersectionPoint.y, intersectionPoint.z);
    }

    float u = 0.f, v = 0.f;

    GetIntersectingUV(intersectionPoint, beta, gamma, u, v);

    return texture->GetInterpolatedUV(u, v);
}