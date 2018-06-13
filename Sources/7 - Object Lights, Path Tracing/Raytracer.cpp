/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include <cstring>
#include <iostream>
#include <chrono>

#include "Scene.h"
#include "Renderer.h"

#include "IOManager.h"

// temp
#include "IOManager.h"
#include "PerlinNoise.h"
#include "RandomGenerator.h"

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

/*     
    // For creating perlin noise image
    PerlinNoise pn;
    pn.scalingFactor = 0.01f;
    pn.appearance = APPEARANCE::PETCH;
    int perlinTest[1024 * 768 * 3];

    for(unsigned int y = 0; y < 768; y++)
    {
        for(unsigned int x = 0; x < 1024; x++)
        {
            float perlinValue = pn.GetPerlinNoiseAt((float)x, (float)y, 0.f);

            perlinTest[y * 3 * 1024 + x * 3] = (int)(255 * perlinValue);
            perlinTest[y * 3 * 1024 + x * 3 + 1] = (int)(255 * perlinValue);
            perlinTest[y * 3 * 1024 + x * 3 + 2] = (int)(255 * perlinValue);
        }
    }
    
    IOManager::WritePng("perlinTest.png", 1024, 768, perlinTest); 
*/

/*
    ReadEXR Test
*/
    int w = -1, h = -1;
    float *buffer;

    Renderer::RenderScene();

    std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
    auto elapsedTimeToRender = std::chrono::duration_cast<std::chrono::microseconds>( t4 - t3 ).count();
    std::cout << "Time elapsed to render the scene: " << elapsedTimeToRender / pow(10, 6) << " seconds / " << elapsedTimeToRender << " microseconds." << std::endl;
 
    return 0;
}