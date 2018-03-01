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

Scene::Scene()
{
    mainScene = this;
}

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


    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;

        stream >> camera.position.x >> camera.position.y >> camera.position.z;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        stream >> camera.nearPlane.x >> camera.nearPlane.y >> camera.nearPlane.z >> camera.nearPlane.w;
        stream >> camera.nearDistance;
        stream >> camera.imageWidht >> camera.imageHeight;
        stream >> camera.imageName;

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }
    
    //Get Lights
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambientLight.r >> ambientLight.g >> ambientLight.b;
    element = element->FirstChildElement("PointLight");
    PointLight pointLight;
    while (element)
    {
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> pointLight.position.x >> pointLight.position.y >> pointLight.position.z;
        stream >> pointLight.intensity.x >> pointLight.intensity.y >> pointLight.intensity.z;

        pointLights.push_back(pointLight);
        element = element->NextSiblingElement("PointLight");
    }

    //Get Materials
    element = root->FirstChildElement("Materials");
    element = element->FirstChildElement("Material");
    Material material;
    while (element)
    {
        child = element->FirstChildElement("AmbientReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("MirrorReflectance");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("PhongExponent");
        stream << child->GetText() << std::endl;

        stream >> material.ambient.r >> material.ambient.g >> material.ambient.b;
        stream >> material.diffuse.r >> material.diffuse.g >> material.diffuse.b;
        stream >> material.specular.r >> material.specular.g >> material.specular.b;
        stream >> material.mirror.r >> material.mirror.g >> material.mirror.b;
        stream >> material.phongExponent;

        materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    //Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vector3 vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        vertices.push_back(vertex);
    }
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");
    Mesh mesh;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;

        int materialId;
        stream >> materialId;
        mesh.material = &materials[materialId - 1];

        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;
        Face face;
        while (!(stream >> face.v0).eof())
        {
            stream >> face.v1 >> face.v2;
            mesh.faces.push_back(face);
        }
        stream.clear();

        meshes.push_back(mesh);
        mesh.faces.clear();
        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere sphere;
    while (element)
    {
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;

        int materialId;
        stream >> materialId;
        sphere.material = &materials[materialId - 1];

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;

        unsigned int centerVertexId;
        stream >> centerVertexId;
        sphere.center = vertices[centerVertexId - 1];

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere.radius;

        spheres.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}