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

    void SetNearPlaneWrtDOF();

    // Left Right Bottom Top 
    Vector4 nearPlane;
    
    Vector3 gaze;
    Vector3 up;
    Vector3 right;
    Vector3 position;

    std::string imageName;

    float nearDistance;
    float focusDistance;
    float apartureSize;
    unsigned int imageWidth;
    unsigned int imageHeight;
    unsigned int numberOfSamples;

    bool dopEnabled;
private:
};

#endif