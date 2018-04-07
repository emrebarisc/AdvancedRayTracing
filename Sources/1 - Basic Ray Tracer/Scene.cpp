/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "Scene.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "tinyxml2.h"

using tinyxml2::XMLDocument;

Scene *mainScene = nullptr;

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
    stream >> bgColor.r >> bgColor.g >> bgColor.b;

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

        cameras.push_back(camera);
        element = element->NextSiblingElement("Camera");
    }
    
    //Get Ambient Light
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> ambientLight.x >> ambientLight.y >> ambientLight.z;


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
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }

        child = element->FirstChildElement("PhongExponent");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "1" << std::endl;
        }

        stream >> material.ambient.x >> material.ambient.y >> material.ambient.z;
        stream >> material.diffuse.x >> material.diffuse.y >> material.diffuse.z;
        stream >> material.specular.x >> material.specular.y >> material.specular.z;
        stream >> material.mirror.x >> material.mirror.y >> material.mirror.z;
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

            Vector3 a = mainScene->vertices[face.v0 - 1];
            Vector3 b = mainScene->vertices[face.v1 - 1];
            Vector3 c = mainScene->vertices[face.v2 - 1];

            face.normal = Vector3::Cross(c - b, a - b);
            Vector3::Normalize(face.normal);

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

bool Scene::SingleRayTrace(const Vector3& e, const Vector3& d, float &hitT, Vector3 &hitN, ObjectBase **hitObject, bool shadowCheck)
{
    unsigned int sphereCount = spheres.size();
    unsigned int meshCount = meshes.size();

    if(hitObject != nullptr) *hitObject = nullptr;
    hitT = 0;

    for (size_t sphereIndex = 0; sphereIndex < sphereCount; sphereIndex++)
	{
		Sphere *currentSphere = &spheres[sphereIndex];
		
        float t;
		if (currentSphere->Intersection(e, d, t, shadowCheck))
		{
			if ((hitT > 0 && hitT > t) || hitT <= 0)
			{
                hitT = t;
                Vector3 p = e + d * t;
                hitN = (p - currentSphere->center);
                Vector3::Normalize(hitN);
                *hitObject = currentSphere;
			}
		}
	}

    for (size_t meshIndex = 0; meshIndex < meshCount; meshIndex++)
	{
		Mesh *currentMesh = &meshes[meshIndex];
	
		size_t faceCount = currentMesh->faces.size();
		for (size_t faceIndex = 0; faceIndex < faceCount; faceIndex++)
		{
			Face *currFace = &currentMesh->faces[faceIndex];

            float t;
			if (currFace->Intersection(e, d, t, shadowCheck))
			{
				if ((hitT > 0 && hitT > t) || hitT <= 0)
				{
                    hitT = t;
                    hitN = currFace->normal;
                    *hitObject = currentMesh;
				}
			}
		}
	}

    return hitT > 0 ? true : false;
}