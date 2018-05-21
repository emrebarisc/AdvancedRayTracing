/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <string>

#include "Math.h"

enum TONE_MAPPING_TYPE : uint8_t
{
    FILMIC = 0,
    PHOTOGRAPHIC,
    NONE
};

class Camera
{
public:
    Camera()
    {
        
    }
    
    ~Camera()
    {

    }

    void SetNearPlaneWrtDOF();

    // Left Right Bottom Top 
    Vector4 nearPlane;
    
    Vector3 gaze;
    Vector3 up;
    Vector3 right;
    Vector3 position;

    std::string imageName;
    
    // TMOOptions::x = Tone mapping a constant, TMOOptions::y = Burnout if 1 : (100 - 1) / 100
    Vector2 TMOOptions;
    float saturation;
    float gamma;

    float nearDistance;
    float focusDistance;
    float apartureSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int numberOfSamples;
    
    TONE_MAPPING_TYPE TMO = TONE_MAPPING_TYPE::NONE;

    bool dopEnabled;
private:
};

#endif