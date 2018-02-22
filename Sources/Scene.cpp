/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Scene.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "Externals/tinyxml2.h"

using tinyxml2::XMLDocument;

void Scene::ReadSceneData(char *filePath)
{
    XMLDocument xmlFile;
    std::stringstream stream;

    auto res = xmlFile.LoadFile(filePath);
    if(res)
    {
        throw std::runtime_error("Error: XML file could not be loaded.");
    }

    auto root = xmlFile.FirstChild();
    if(!root)
    {
        throw std::runtime_error("Error: Root could not be found.");
    }

    //Get MaxRecursionDepth
    auto element = root->FirstChildElement("MaxRecursionDepth");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0" << std::endl;
    }
    stream >> maxRecursionDepth;

    //Get BackgroundColor
    element = root->FirstChildElement("BackgroundColor");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0 0 0" << std::endl;
    }
    stream >> bgColor.x >> bgColor.y >> bgColor.z;

    
}