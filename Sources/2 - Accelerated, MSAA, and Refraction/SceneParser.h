/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#ifndef __SCENEPARSER_H__
#define __SCENEPARSER_H__

class Scene;

class SceneParser
{
public:
    static void Parse(Scene *scene, char *filePath);
};

#endif