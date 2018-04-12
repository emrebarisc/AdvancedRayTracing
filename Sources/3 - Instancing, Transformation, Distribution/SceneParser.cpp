/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "SceneParser.h"

#include <sstream>
#include <stdexcept>

#include "BVH.h"
#include "Mesh.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transformations.h"
#include "tinyxml2.h"

#include "BoundingVolume.h"

using tinyxml2::XMLDocument;

void SceneParser::Parse(Scene *scene, char *filePath)
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
    stream >> scene->maxRecursionDepth;

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
    stream >> scene->bgColor.r >> scene->bgColor.g >> scene->bgColor.b;

    stream.clear();

    //Get Cameras
    element = root->FirstChildElement("Cameras");
    element = element->FirstChildElement("Camera");
    Camera camera;
    while (element)
    {
        auto child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        stream >> camera.position.x >> camera.position.y >> camera.position.z;

        child = element->FirstChildElement("Gaze");
        stream << child->GetText() << std::endl;
        stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;

        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;

        // Set up the right veupctor and make forward and up vector perpenticular in case of the fact that they are not
        camera.right = Vector3::Cross(camera.gaze, camera.up);
        camera.up = Vector3::Cross(camera.right, camera.gaze);

        child = element->FirstChildElement("NearPlane");
        stream << child->GetText() << std::endl;
        stream >> camera.nearPlane.x >> camera.nearPlane.y >> camera.nearPlane.z >> camera.nearPlane.w;
        
        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        stream >> camera.nearDistance;
        
        child = element->FirstChildElement("FocusDistance");
        if(child)
        {
            stream << child->GetText() << std::endl;
            stream >> camera.focusDistance;

            camera.dopEnabled = true;
            camera.SetNearPlaneWrtDOF();
        }
        else
        {
            stream << 0.0f << std::endl;
            camera.dopEnabled = false;
            stream >> camera.focusDistance;
        }
        

        child = element->FirstChildElement("ApertureSize");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << 0.0f << std::endl;
        }
        stream >> camera.apartureSize;

        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        stream >> camera.imageWidth >> camera.imageHeight;

        child = element->FirstChildElement("NumSamples");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "1" << std::endl;;
        }
        stream >> camera.numberOfSamples;

        child = element->FirstChildElement("ImageName");
        stream << child->GetText() << std::endl;
        stream >> camera.imageName;

        scene->cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }
    
    //Get Ambient Light
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> scene->ambientLight.x >> scene->ambientLight.y >> scene->ambientLight.z;


    //Get Lights
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

        scene->pointLights.push_back(pointLight);
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
        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;

        child = element->FirstChildElement("DiffuseReflectance");
        stream << child->GetText() << std::endl;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;

        child = element->FirstChildElement("SpecularReflectance");
        stream << child->GetText() << std::endl;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;

        child = element->FirstChildElement("MirrorReflectance");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;

        child = element->FirstChildElement("Transparency");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }

        stream >> material.transparency.x >> material.transparency.y >> material.transparency.z;

        child = element->FirstChildElement("RefractionIndex");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "0.0" << std::endl;
        }
        stream >> material.refractionIndex;

        child = element->FirstChildElement("PhongExponent");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "1" << std::endl;
        }
        stream >> material.phongExponent;

        scene->materials.push_back(material);
        element = element->NextSiblingElement("Material");
    }

    // Get transformations
    element = root->FirstChildElement("Transformations");
    if(element)
    {
        element = element->FirstChildElement("Translation");
        while(element)
        {
            stream << element->GetText() << std::endl;
            
            Vector3 translation;
            stream >> translation.x >> translation.y >> translation.z;
            scene->translations.push_back(translation);

            element = element->NextSiblingElement("Translation");
        }
    }

    element = root->FirstChildElement("Transformations");
    if(element)
    {
        element = element->FirstChildElement("Scaling");
        while(element)
        {
            stream << element->GetText() << std::endl;
            
            Vector3 scaling;
            stream >> scaling.x >> scaling.y >> scaling.z;
            scene->scalings.push_back(scaling);

            element = element->NextSiblingElement("Scaling");
        }
    }

    element = root->FirstChildElement("Transformations");
    if(element)
    {            
        element = element->FirstChildElement("Rotation");
        while(element)
        {
            stream << element->GetText() << std::endl;
            
            Vector4 rotation;
            stream >> rotation.x >> rotation.y >> rotation.z >> rotation.w;
            scene->rotations.push_back(rotation);

            element = element->NextSiblingElement("Rotation");
        }
    }

    // Get VertexData
    element = root->FirstChildElement("VertexData");
    stream << element->GetText() << std::endl;
    Vector3 vertex;
    while (!(stream >> vertex.x).eof())
    {
        stream >> vertex.y >> vertex.z;
        scene->vertices.push_back(vertex);
        scene->vertexNormals.push_back(Vector3::ZeroVector);
    }
    
    stream.clear();

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");

    unsigned int vertexNormalDivider[scene->vertices.size()];
    
    Mesh *mesh;
    while (element)
    {
        mesh = new Mesh();

        const char *shading = element->Attribute("shadingMode");

        if(std::string(shading) == "smooth")
        {
            mesh->shadingMode = SHADING_MODE::SMOOTH;
        }

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;

        int materialId;
        stream >> materialId;
        mesh->material = &scene->materials[materialId - 1];

        child = element->FirstChildElement("Transformations");
        if(child)
        {
            stream << child->GetText() << std::endl;
            
            std::string transformationEncoding;
            while(stream >> transformationEncoding && transformationEncoding.length() > 0) {
                char transformationType = transformationEncoding[0];
                int transformationId = std::stoi (transformationEncoding.substr(1));
                switch (transformationType)
                {
                    case 't':
                        mesh->transformationMatrix = Transformation::GetTranslationMatrix(scene->translations[transformationId - 1]) * mesh->transformationMatrix;
                        break;
                    case 'r':
                        mesh->transformationMatrix = Transformation::GetRotationMatrix(scene->rotations[transformationId - 1]) * mesh->transformationMatrix;
                        break;
                    case 's':
                        mesh->transformationMatrix = Transformation::GetScalingMatrix(scene->scalings[transformationId - 1]) * mesh->transformationMatrix;
                        break;
                }
            }
        }

        mesh->inverseTransformationMatrix = mesh->transformationMatrix.Invert();
        stream.clear();
 
        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;

        unsigned int vertexOffset = (unsigned int)child->IntAttribute("vertexOffset", 0);

        Face *face;
        unsigned int v0;
        while (!(stream >> v0).eof())
        {
            unsigned int v1, v2;
            stream >> v1 >> v2;
            face = new Face();

            if(std::string(shading) == "smooth")
            {
                face->shadingMode = SHADING_MODE::SMOOTH;
            }
            
            face->v0 = v0 + vertexOffset;
            face->v1 = v1 + vertexOffset;
            face->v2 = v2 + vertexOffset;
            
            Vector3 a = mainScene->vertices[face->v0 - 1];
            Vector3 b = mainScene->vertices[face->v1 - 1];
            Vector3 c = mainScene->vertices[face->v2 - 1];
            face->normal = Vector3::Cross(c - b, a - b);
            Vector3::Normalize(face->normal);

            scene->vertexNormals[face->v0 - 1] += face->normal;
            scene->vertexNormals[face->v1 - 1] += face->normal;
            scene->vertexNormals[face->v2 - 1] += face->normal;
            
            mesh->faces.push_back(face);
        }
        stream.clear();

        scene->objects.push_back(mesh);

        element = element->NextSiblingElement("Mesh");
    }
    stream.clear();

    for(unsigned int normalIndex = 0; normalIndex < scene->vertices.size(); normalIndex++)
    {
        if(vertexNormalDivider[normalIndex] != 0)
        {
            scene->vertexNormals[normalIndex] /= vertexNormalDivider[normalIndex];
            scene->vertexNormals[normalIndex].Normalize();
        }
    }

    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    Sphere *sphere;
    while (element)
    {
        sphere = new Sphere();
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;

        int materialId;
        stream >> materialId;
        sphere->material = &scene->materials[materialId - 1];

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;

        unsigned int centerVertexId;
        stream >> centerVertexId;
        sphere->center = scene->vertices[centerVertexId - 1];

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere->radius;

        scene->objects.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
    }
}