/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include <cstring>
#include <iostream>
#include <chrono>

#include "Scene.h"
#include "ObjectBase.h"
#include "Renderer.h"
#include "Transformations.h"

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <file name>." << std::endl;
        return -1;
    }

    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

    Scene mainScene;
    mainScene.ReadSceneData(argv[1]);

    for(unsigned char argIndex = 1; argIndex < argc; argIndex++)
    {
        if(strcmp(argv[argIndex], "--noBVH") == 0)
        {
            mainScene.useBVH = false;
        }
    }

    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
    auto elapsedTimeToReadTheScene = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
    std::cout << "Time elapsed to read the scene data: " << elapsedTimeToReadTheScene / pow(10, 6) << " seconds / " << elapsedTimeToReadTheScene << " microseconds." << std::endl;

    if(mainScene.useBVH) mainScene.CreateBVH();

    std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
    auto elapsedTimeToCreateBVH = std::chrono::duration_cast<std::chrono::microseconds>( t3 - t2 ).count();
    std::cout << "Time elapsed to create BVH of the scene: " << elapsedTimeToCreateBVH / pow(10, 6) << " seconds / " << elapsedTimeToCreateBVH << " microseconds." << std::endl;

    //Renderer::RenderScene();
    std::string imageName = mainScene.cameras[0].imageName;

    for(unsigned int i = 1; i < 360; i++)
    {
        mainScene.cameras[0].imageName = imageName + (i < 10 ? std::to_string(0) : "") + (i < 10 || i < 100 ? std::to_string(0) : "") + std::to_string(i) + ".png";
        mainScene.objects[0]->SetTransformationMatrix(Transformation::GetRotationMatrix(Vector4(1.f, 0.f, 1.f, 0.f)) * mainScene.objects[0]->transformationMatrix);
        mainScene.objects[0]->InvertTransformationMatrix();
        Renderer::RenderScene();
    }

    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    auto elapsedTimeToRender = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t3 ).count();
    std::cout << "Time elapsed to render the scene: " << elapsedTimeToRender / pow(10, 6) << " seconds / " << elapsedTimeToRender << " microseconds." << std::endl;

    return 0;
}