/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __BRDF_H__
#define __BRDF_H__

#include "Math.h"
#include "Material.h"

struct BRDF
{
    virtual Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const = 0;
    
    float exponent;
};

struct OriginalPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = Vector3::Dot(n, wi);
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 reflectionVector = wi - 2 * n * Vector3::Dot(n, wi);
            float cosAlphaR = Vector3::Dot(reflectionVector.GetNormalized(), wo);
            return diffuse * cosTethaI + specular * std::pow(cosAlphaR, exponent);
        }

        return Vector3::ZeroVector;
    }
};

struct ModifiedPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 reflectionVector = wi - 2 * n * Vector3::Dot(wi, n);
            float cosAlphaR = Vector3::Dot(reflectionVector, wo);
            return diffuse * cosTethaI + specular * std::pow(cosAlphaR, exponent) * cosTethaI;
        }

        return Vector3::ZeroVector;
    }
};

struct NormalizedModifiedPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 reflectionVector = wi - 2 * n * Vector3::Dot(wi, n);
            float cosAlphaR = Vector3::Dot(reflectionVector, wo);
            return diffuse * cosTethaI * ONE_OVER_PI + (specular * (exponent + 2) / TWO_PI) * std::pow(cosAlphaR, exponent) * cosTethaI;
        }

        return Vector3::ZeroVector;
    }
};

struct OriginalBlinnPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 wh = wi + wo;
            wh.Normalize();

            float cosAlphaH = mathMax(0, Vector3::Dot(n, wh));
            
            return diffuse * cosTethaI + specular * std::pow(cosAlphaH, exponent);
        }

        return Vector3::ZeroVector;
    }
};

struct ModifiedBlinnPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 wh = wi + wo;
            wh.Normalize();

            float cosAlphaH = mathMax(0, Vector3::Dot(n, wh));
            
            return (diffuse + specular * std::pow(cosAlphaH, exponent)) * cosTethaI;
        }

        return Vector3::ZeroVector;
    }
};

struct NormalizedModifiedBlinnPhong : public BRDF
{
    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTethaI = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTethaI);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 wh = wi + wo;
            wh.Normalize();

            Vector3 reflectionVector = wi - 2 * n * Vector3::Dot(wi, n);

            float cosAlphaH = mathMax(0, Vector3::Dot(n, wh));
            
            return (diffuse * ONE_OVER_PI + (specular * (exponent + 8) * std::pow(cosAlphaH, exponent) / (8 * PI))) * cosTethaI;
        }

        return Vector3::ZeroVector;
    }
};

struct Ops
{
    static float D(float p, float cosAlpha)
    {
        return (p + 2) * std::pow(cosAlpha, p) / TWO_PI;
    }

    static float F(float n, float cosBeta)
    {
        float rZero = (n - 1) * (n - 1) / ((n + 1) * (n + 1));
        return rZero + (1 - rZero) * std::pow(1 - cosBeta, 5);
    }

    static float G(Vector3 n, Vector3 wh, Vector3 wi, Vector3 wo)
    {
        return mathMin(1, mathMin(2 * Vector3::Dot(n, wh) * Vector3::Dot(n, wo) / Vector3::Dot(wo, wh), 2 * Vector3::Dot(n, wh) * Vector3::Dot(n, wi) / Vector3::Dot(wo, wh)));
    }
};

struct TorranceSparrow : public BRDF
{
    float refractiveIndex;

    Vector3 GetBRDF(const Vector3 &diffuse, const Vector3 &specular, const Vector3 &n, const Vector3 &wo, const Vector3 &wi) const override
    {
        float cosTetha = mathMax(0, Vector3::Dot(n, wi));
        float tethaI = acos(cosTetha);

        //if(DEGREE_TO_RADIAN(tethaI) < 90.f)
        if(tethaI < HALF_PI)
        {
            Vector3 wh = wi + wo;
            wh.Normalize();

            float cosAlpha = mathMax(0, Vector3::Dot(n, wh));
            float cosBeta = mathMax(0, Vector3::Dot(wo, wh));
            float cosPhi = mathMax(0, Vector3::Dot(n, wo));
            
            return (diffuse * ONE_OVER_PI + specular * Ops::D(exponent, cosAlpha) * Ops::F(refractiveIndex, cosBeta) * Ops::G(n, wh, wi, wo) / (4 * cosTetha * cosPhi)) * cosTetha;
        }

        return Vector3::ZeroVector;
    }
};

#endif