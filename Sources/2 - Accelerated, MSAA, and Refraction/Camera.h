/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <string>

#include "Math.h"

class Camera
{
public:
    Camera()
    {

    }
    
    ~Camera()
    {

    }

    Vector4 nearPlane;
    Vector3 gaze;
    Vector3 up;
    Vector3 right;
    Vector3 position;

    // Left Right Bottom Top
    float nearDistance;
    
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int numberOfSamples;
    std::string imageName;
private:
};

#endif