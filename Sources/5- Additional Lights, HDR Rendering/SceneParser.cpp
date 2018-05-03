/*
 *	Advanced ray-tracer algorithm
 *	Emre Baris Coskun
 *	2018
 */

#include "SceneParser.h"

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "BVH.h"
#include "Math.h"
#include "Mesh.h"
#include "PerlinNoise.h"
#include "Scene.h"
#include "Sphere.h"
#include "Transformations.h"
#include "tinyply.h"
#include "tinyxml2.h"

#include "AreaLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

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

    auto element = root->FirstChildElement("IntersectionTestEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> EPSILON;

    element = root->FirstChildElement("ShadowRayEpsilon");
    if (element)
    {
        stream << element->GetText() << std::endl;
    }
    else
    {
        stream << "0.001" << std::endl;
    }
    stream >> SHADOW_EPSILON;

    //Get MaxRecursionDepth
    element = root->FirstChildElement("MaxRecursionDepth");
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

        child = element->FirstChildElement("ImageResolution");
        stream << child->GetText() << std::endl;
        stream >> camera.imageWidth >> camera.imageHeight;

        child = element->FirstChildElement("NearDistance");
        stream << child->GetText() << std::endl;
        stream >> camera.nearDistance;

        const char *cameraType = element->Attribute("type");

        if(cameraType && std::string(cameraType) == "simple")
        {
            Vector3 gazePoint;
            float fovY;

            child = element->FirstChildElement("GazePoint");
            stream << child->GetText() << std::endl;
            stream >> gazePoint.x >> gazePoint.y >> gazePoint.z;

            child = element->FirstChildElement("FovY");
            stream << child->GetText() << std::endl;
            stream >> fovY;

            camera.gaze = (gazePoint - camera.position).GetNormalized();

            float resolutionProportion = camera.imageWidth / camera.imageHeight;

            float halfOfFovY = fovY * 0.5f;
            float top, bottom, left, right;
            top = camera.nearDistance * tan(DEGREE_TO_RADIAN(halfOfFovY));
            bottom = -top;
            left = bottom * resolutionProportion;
            right = top * resolutionProportion;

            camera.nearPlane = Vector4(left, right, bottom, top);
        }
        else
        {
            child = element->FirstChildElement("Gaze");
            stream << child->GetText() << std::endl;
            stream >> camera.gaze.x >> camera.gaze.y >> camera.gaze.z;
            
            child = element->FirstChildElement("NearPlane");
            stream << child->GetText() << std::endl;
            stream >> camera.nearPlane.x >> camera.nearPlane.y >> camera.nearPlane.z >> camera.nearPlane.w;
        }

        child = element->FirstChildElement("Up");
        stream << child->GetText() << std::endl;
        stream >> camera.up.x >> camera.up.y >> camera.up.z;
        
        // Set up the right vector and make forward and up vector perpenticular in case they are not
        camera.right = Vector3::Cross(camera.gaze.GetNormalized(), camera.up.GetNormalized());
        camera.up = Vector3::Cross(camera.right.GetNormalized(), camera.gaze.GetNormalized());
        camera.gaze = Vector3::Cross(camera.up.GetNormalized(), camera.right.GetNormalized());

        camera.right.Normalize();
        camera.up.Normalize();
        camera.right.Normalize();
        
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
        stream.clear();
    }
    
    //Get Ambient Light
    element = root->FirstChildElement("Lights");
    auto child = element->FirstChildElement("AmbientLight");
    stream << child->GetText() << std::endl;
    stream >> scene->ambientLight.x >> scene->ambientLight.y >> scene->ambientLight.z;

    //Get Point Lights
    element = element->FirstChildElement("PointLight");
    PointLight *pointLight;
    while (element)
    {
        pointLight = new PointLight();
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;

        stream >> pointLight->position.x >> pointLight->position.y >> pointLight->position.z;
        stream >> pointLight->intensity.x >> pointLight->intensity.y >> pointLight->intensity.z;

        scene->lights.push_back(pointLight);
        element = element->NextSiblingElement("PointLight");
        stream.clear();
    }

    //Get Area Lights
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("AreaLight");
    AreaLight *areaLight;
    while (element)
    {
        areaLight = new AreaLight();
        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        stream >> areaLight->position.x >> areaLight->position.y >> areaLight->position.z;

        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;
        stream >> areaLight->intensity.x >> areaLight->intensity.y >> areaLight->intensity.z;

        child = element->FirstChildElement("EdgeVector1");
        stream << child->GetText() << std::endl;
        stream >> areaLight->edgeVectorU.x >> areaLight->edgeVectorU.y >> areaLight->edgeVectorU.z;

        child = element->FirstChildElement("EdgeVector2");
        stream << child->GetText() << std::endl;
        stream >> areaLight->edgeVectorV.x >> areaLight->edgeVectorV.y >> areaLight->edgeVectorV.z;

        scene->lights.push_back(areaLight);
        element = element->NextSiblingElement("AreaLight");
        stream.clear();
    }

    //Get Directional Lights
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("DirectionalLight");
    DirectionalLight *directionalLight;
    while (element)
    {
        directionalLight = new DirectionalLight();
        child = element->FirstChildElement("Direction");
        stream << child->GetText() << std::endl;
        child = element->FirstChildElement("Radiance");
        stream << child->GetText() << std::endl;

        stream >> directionalLight->direction.x >> directionalLight->direction.y >> directionalLight->direction.z;
        stream >> directionalLight->radiance.x >> directionalLight->radiance.y >> directionalLight->radiance.z;

        scene->lights.push_back(directionalLight);
        element = element->NextSiblingElement("DirectionalLight");
        stream.clear();
    }

    //Get Spot Lights
    element = root->FirstChildElement("Lights");
    element = element->FirstChildElement("SpotLight");
    SpotLight *spotLight;
    while (element)
    {
        float coverage, falloff;

        child = element->FirstChildElement("CoverageAngle");
        stream << child->GetText() << std::endl;
        stream >> coverage;

        child = element->FirstChildElement("FalloffAngle");
        stream << child->GetText() << std::endl;
        stream >> falloff;

        spotLight = new SpotLight(coverage * 0.5f, falloff * 0.5f);

        child = element->FirstChildElement("Position");
        stream << child->GetText() << std::endl;
        stream >> spotLight->position.x >> spotLight->position.y >> spotLight->position.z;

        Vector3 direction;
        child = element->FirstChildElement("Direction");
        stream << child->GetText() << std::endl;
        stream >> direction.x >> direction.y >> direction.z;
        spotLight->direction = direction.GetNormalized();
        
        child = element->FirstChildElement("Intensity");
        stream << child->GetText() << std::endl;
        stream >> spotLight->intensity.x >> spotLight->intensity.y >> spotLight->intensity.z;


        scene->lights.push_back(spotLight);
        element = element->NextSiblingElement("SpotLight");
        stream.clear();
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

        child = element->FirstChildElement("Roughness");
        if(child)
        {
            stream << child->GetText() << std::endl;
        }
        else
        {
            stream << "0" << std::endl;
        }
        stream >> material.roughness;

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
        stream.clear();
    }

    // Get textures
    element = root->FirstChildElement("Textures");
    if(element)
    {
        element = element->FirstChildElement("Texture");
        while(element)
        {
            std::string imageName;

            child = element->FirstChildElement("ImageName");
            stream << child->GetText() << std::endl;
            stream >> imageName;

            Texture *texture;

            if(imageName == "perlin")
            {
                PerlinNoise *perlinNoise = new PerlinNoise();

                child = element->FirstChildElement("ScalingFactor");
                if(child)
                {
                    stream << child->GetText() << std::endl;
                }
                else
                {
                    stream << 1.0 << std::endl;
                }
                stream >> perlinNoise->scalingFactor;

                texture = perlinNoise;
            }
            else
            {
                texture = new Texture();
            }
            texture->imagePath = imageName;

            texture->bumpmap = element->BoolAttribute("bumpmap", false);
            texture->bumpmapMultiplier = element->FloatAttribute("bumpmapMultiplier", 1.f);
            
            std::string interpolation;
            child = element->FirstChildElement("Interpolation");
            if(child)
            {
                stream << child->GetText() << std::endl;
            }
            else
            {
                stream << "bilinear" << std::endl;
            }

            stream >> interpolation;
            
            texture->interpolationMethod = interpolation == "bilinear" ? INTERPOLATION::BILINEAR : INTERPOLATION::NEAREST;       

            std::string decalMode;
            child = element->FirstChildElement("DecalMode");
            if(child)
            {
                stream << child->GetText() << std::endl;
            }
            else
            {
                stream << "replace_kd" << std::endl;
            }
            stream >> decalMode;
            
            texture->decalMode = decalMode == "replace_kd" ? DECAL_MODE::REPLACE_KD :
                                decalMode == "blend_kd"   ? DECAL_MODE::BLEND_KD :
                                                            DECAL_MODE::REPLACE_ALL;

            child = element->FirstChildElement("Normalizer");
            if(child)
            {
                stream << child->GetText() << std::endl;
            }
            else
            {
                stream << 255 << std::endl;
            }
            stream >> texture->normalizer;

            std::string appearance;
            child = element->FirstChildElement("Appearance");
            if(child)
            {
                stream << child->GetText() << std::endl;
            }
            else
            {
                stream << "repeat" << std::endl;
            }
            stream >> appearance;

            texture->appearance = appearance == "repeat" ? APPEARANCE::REPEAT : appearance == "clamp" ? APPEARANCE::CLAMP :
                                  appearance == "vein"   ? APPEARANCE::VEIN   : APPEARANCE::PETCH;
            
            if(texture->imagePath != "perlin")
            {
                bool textureLoaded = texture->LoadTextureImage();

                if(!textureLoaded)
                {
                    std::cerr << "Texture is failed to load." << std::endl;
                }
            }

            scene->textures.push_back(texture);
            element = element->NextSiblingElement("Texture");
        }
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
    if(element)
    {
        stream << element->GetText() << std::endl;
        Vector3 vertex;
        while (!(stream >> vertex.x).eof())
        {
            stream >> vertex.y >> vertex.z;
            scene->vertices.push_back(vertex);
            scene->vertexNormals.push_back(Vector3::ZeroVector);
        }
    }
    
    stream.clear();

    // Get TexCoordData
    element = root->FirstChildElement("TexCoordData");
    if(element)
    {
        stream << element->GetText() << std::endl;
        Vector2 textureCoordinate;
        while (!(stream >> textureCoordinate.x).eof())
        {
            stream >> textureCoordinate.y;
            scene->textureCoordinates.push_back(textureCoordinate);
        }
        stream.clear();
    }

    //Get Meshes
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Mesh");

    unsigned int *vertexNormalDivider = new unsigned int[scene->vertices.size()];
    
    while (element)
    {
        stream.clear();
        Mesh *mesh = new Mesh();

        const char *shading = element->Attribute("shadingMode");

        if(shading && std::string(shading) == "smooth")
        {
            mesh->shadingMode = SHADING_MODE::SMOOTH;
        }

        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;

        int materialId;
        stream >> materialId;
        mesh->material = &scene->materials[materialId - 1];
        stream.clear();

        child = element->FirstChildElement("Texture");
        if(child)
        {
            int textureId;
            stream << child->GetText() << std::endl;
            stream >> textureId;
            mesh->texture = scene->textures[textureId - 1];
        }
        else
        {
            mesh->texture = nullptr;
        }
        stream.clear();

        /*element = root->FirstChildElement("MotionBlur");
        if (element)
        {
            stream << element->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }
        stream >> mesh->motionBlur.x >> mesh->motionBlur.y >> mesh->motionBlur.z;*/

        child = element->FirstChildElement("Transformations");
        if(child)
        {
            stream << child->GetText() << std::endl;
            
            std::string transformationEncoding;
            while(stream >> transformationEncoding && transformationEncoding.length() > 0) 
            {
                char transformationType = transformationEncoding[0];
                int transformationId = std::stoi (transformationEncoding.substr(1));
                switch (transformationType)
                {
                    case 't':
                        mesh->SetTransformationMatrix(Transformation::GetTranslationMatrix(scene->translations[transformationId - 1]) * mesh->transformationMatrix);
                        break;
                    case 'r':
                        mesh->SetTransformationMatrix(Transformation::GetRotationMatrix(scene->rotations[transformationId - 1]) * mesh->transformationMatrix);
                        break;
                    case 's':
                        mesh->SetTransformationMatrix(Transformation::GetScalingMatrix(scene->scalings[transformationId - 1]) * mesh->transformationMatrix);
                        break;
                }
            }
        }
        mesh->SetInverseTransformationMatrix();
        stream.clear();
 
        child = element->FirstChildElement("Faces");
        stream << child->GetText() << std::endl;

        unsigned int vertexOffset = (unsigned int)child->IntAttribute("vertexOffset", 0);
        mesh->vertexOffset = vertexOffset;

        unsigned int textureOffset = (unsigned int)child->IntAttribute("textureOffset", 0);
        mesh->textureOffset = textureOffset;

        const char *plyPath = child->Attribute("plyFile", nullptr);

        if(plyPath)
        {
            std::string fileName(plyPath);
            std::ifstream ss(fileName, std::ios::binary);

            if (ss.fail()) 
            {
                throw std::runtime_error("failed to open " + fileName);
            }

            tinyply::PlyFile plyFile;
            plyFile.parse_header(ss);
            
            std::shared_ptr<tinyply::PlyData> vertices, normals, colors, faces, quadrants, texcoords;
            try { vertices = plyFile.request_properties_from_element("vertex", { "x", "y", "z" }); }
            catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            // try { normals = plyFile.request_properties_from_element("vertex", { "nx", "ny", "nz" }); }
            // catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            // try { colors = plyFile.request_properties_from_element("vertex", { "red", "green", "blue", "alpha" }); }
            // catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            try { faces = plyFile.request_properties_from_element("face", { "vertex_index" }); }
            catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            try { quadrants = plyFile.request_properties_from_element("face", { "vertex_indices", "nx", "ny", "nz" }); }
            catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            // try { texcoords = plyFile.request_properties_from_element("face", { "texcoord" }); }
            // catch (const std::exception & e) {/*  std::cerr << "tinyply exception: " << e.what() << std::endl; */ }

            plyFile.read(ss);

            unsigned int plyVertexOffset = 0;

            if(vertices)
            {
                const size_t verticesInBytes = vertices->buffer.size_bytes();
                std::vector<Vector3> vertexVector(vertices->count);
                std::memcpy(vertexVector.data(), vertices->buffer.get(), verticesInBytes);

                plyVertexOffset = scene->vertices.size();

                for(auto value : vertexVector)
                {
                    scene->vertices.push_back(value);
                    scene->vertexNormals.push_back(Vector3::ZeroVector);
                }
            }
            

            if(faces)
            {
                const size_t facesInBytes = faces->buffer.size_bytes();
                struct uint4 { unsigned int v0, v1, v2, v3; };
                std::vector<uint4> faceVector(faces->count);
                std::memcpy(faceVector.data(), faces->buffer.get(), facesInBytes);

                for(auto value : faceVector)
                {
                    {
                        Face *face = new Face();

                        face->shadingMode = mesh->shadingMode;
                        face->material = mesh->material;
                        face->texture = mesh->texture;
                        face->vertexOffset = vertexOffset;
                        face->textureOffset = textureOffset;
                        
                        face->v0 = value.v0 + 1 + vertexOffset + plyVertexOffset;
                        face->v1 = value.v1 + 1 + vertexOffset + plyVertexOffset;
                        face->v2 = value.v2 + 1 + vertexOffset + plyVertexOffset;
                        
                        Vector3 a = scene->vertices[face->v0 - 1];
                        Vector3 b = scene->vertices[face->v1 - 1];
                        Vector3 c = scene->vertices[face->v2 - 1];
                        face->normal = Vector3::Cross(c - b, a - b);
                        Vector3::Normalize(face->normal);

                        scene->vertexNormals[face->v0 - 1] += face->normal;
                        scene->vertexNormals[face->v1 - 1] += face->normal;
                        scene->vertexNormals[face->v2 - 1] += face->normal;
                        
                        vertexNormalDivider[face->v0 - 1]++;
                        vertexNormalDivider[face->v1 - 1]++;
                        vertexNormalDivider[face->v2 - 1]++;

                        face->transformationMatrix = mesh->transformationMatrix;
                        face->inverseTransformationMatrix = mesh->inverseTransformationMatrix;
                        mesh->faces.push_back(face);
                    }

                    {
                        Face *face = new Face();

                        face->shadingMode = mesh->shadingMode;
                        face->material = mesh->material;
                        face->texture = mesh->texture;
                        face->vertexOffset = vertexOffset;
                        face->textureOffset = textureOffset;
                        
                        face->v0 = value.v0 + 1 + vertexOffset + plyVertexOffset;
                        face->v1 = value.v2 + 1 + vertexOffset + plyVertexOffset;
                        face->v2 = value.v3 + 1 + vertexOffset + plyVertexOffset;
                        
                        Vector3 a = scene->vertices[face->v0 - 1];
                        Vector3 b = scene->vertices[face->v1 - 1];
                        Vector3 c = scene->vertices[face->v2 - 1];
                        face->normal = Vector3::Cross(c - b, a - b);
                        Vector3::Normalize(face->normal);

                        scene->vertexNormals[face->v0 - 1] += face->normal;
                        scene->vertexNormals[face->v1 - 1] += face->normal;
                        scene->vertexNormals[face->v2 - 1] += face->normal;
                        
                        vertexNormalDivider[face->v0 - 1]++;
                        vertexNormalDivider[face->v1 - 1]++;
                        vertexNormalDivider[face->v2 - 1]++;

                        face->transformationMatrix = mesh->transformationMatrix;
                        face->inverseTransformationMatrix = mesh->inverseTransformationMatrix;
                        mesh->faces.push_back(face);
                    }
                }
            }

            /* if(quadrants)
            {
                const size_t quadrantsInBytes = quadrants->buffer.size_bytes();
                struct uint3float3 { unsigned int v0, v1, v2; float nv0, nv1, nv2; };
                std::vector<uint3float3> quadrantVector(quadrants->count);
                std::memcpy(quadrantVector.data(), quadrants->buffer.get(), quadrantsInBytes);

                
                for(auto value : quadrantVector)
                {
                    {
                        Face *face = new Face();

                        face->shadingMode = mesh->shadingMode;
                        face->material = mesh->material;
                        face->texture = mesh->texture;
                        face->vertexOffset = vertexOffset;
                        face->textureOffset = textureOffset;
                        
                        face->v0 = value.v0 + 1 + vertexOffset + plyVertexOffset;
                        face->v1 = value.v1 + 1 + vertexOffset + plyVertexOffset;
                        face->v2 = value.v2 + 1 + vertexOffset + plyVertexOffset;
                        
                        Vector3 a = scene->vertices[face->v0 - 1];
                        Vector3 b = scene->vertices[face->v1 - 1];
                        Vector3 c = scene->vertices[face->v2 - 1];
                        face->normal = Vector3::Cross(c - b, a - b);
                        Vector3::Normalize(face->normal);

                        scene->vertexNormals[face->v0 - 1] += face->normal;
                        scene->vertexNormals[face->v1 - 1] += face->normal;
                        scene->vertexNormals[face->v2 - 1] += face->normal;
                        
                        vertexNormalDivider[face->v0 - 1]++;
                        vertexNormalDivider[face->v1 - 1]++;
                        vertexNormalDivider[face->v2 - 1]++;

                        face->transformationMatrix = mesh->transformationMatrix;
                        face->inverseTransformationMatrix = mesh->inverseTransformationMatrix;
                        mesh->faces.push_back(face);
                    }
                }
            } */
        }

        if(!plyPath)
        {
            Face *face;
            unsigned int v0;
            while (!(stream >> v0).eof())
            {
                unsigned int v1, v2;
                stream >> v1 >> v2;
                face = new Face();

                face->shadingMode = mesh->shadingMode;
                face->material = mesh->material;
                face->texture = mesh->texture;
                face->vertexOffset = vertexOffset;
                face->textureOffset = textureOffset;
                
                face->v0 = v0 + vertexOffset;
                face->v1 = v1 + vertexOffset;
                face->v2 = v2 + vertexOffset;
                
                Vector3 a = scene->vertices[face->v0 - 1];
                Vector3 b = scene->vertices[face->v1 - 1];
                Vector3 c = scene->vertices[face->v2 - 1];
                face->normal = Vector3::Cross(c - b, a - b);
                Vector3::Normalize(face->normal);

                scene->vertexNormals[face->v0 - 1] += face->normal;
                scene->vertexNormals[face->v1 - 1] += face->normal;
                scene->vertexNormals[face->v2 - 1] += face->normal;
                
                vertexNormalDivider[face->v0 - 1]++;
                vertexNormalDivider[face->v1 - 1]++;
                vertexNormalDivider[face->v2 - 1]++;

                face->transformationMatrix = mesh->transformationMatrix;
                face->inverseTransformationMatrix = mesh->inverseTransformationMatrix;
                mesh->faces.push_back(face);
            }
            stream.clear();
        }

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

    delete[] vertexNormalDivider;

    // Get mesh instances
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("MeshInstance");
    
    while (element)
    {
        MeshInstance *meshInstance = new MeshInstance();

        unsigned int baseMeshId = element->IntAttribute("baseMeshId");
        if(baseMeshId == 0)
        {
            std::cerr << "Error: Base mesh id is not given." << std::endl;
            return;
        }

        /*
            Can fail if spheres are read from xml file before.
        */
        meshInstance->baseMesh = dynamic_cast<Mesh *>(scene->objects[baseMeshId - 1]);
        if(!meshInstance->baseMesh)
        {
            std::cerr << "Error: The object which has been tried to be set as base mesh is not a mesh." << std::endl;
            return;
        }

        const char *resetTransform = element->Attribute("resetTransform");
        if(!resetTransform || std::string(resetTransform) == "false")
        {
            meshInstance->SetTransformationMatrix(meshInstance->baseMesh->inverseTransformationMatrix.GetTranspose());
        }

        child = element->FirstChildElement("Material");
        if(child)
        {
            stream << child->GetText() << std::endl;

            int materialId;
            stream >> materialId;
            meshInstance->material = &scene->materials[materialId - 1];
        }
        else
        {
            meshInstance->material = meshInstance->baseMesh->material;
        }
        stream.clear();

        int textureId;
        child = element->FirstChildElement("Texture");
        if(child)
        {
            stream << child->GetText() << std::endl;
            stream >> textureId;
            meshInstance->texture = scene->textures[textureId - 1];
        }
        else
        {
            meshInstance->texture = nullptr;
        }
        stream.clear();

        /*element = root->FirstChildElement("MotionBlur");
        if (element)
        {
            stream << element->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }
        stream >> meshInstance->motionBlur.x >> meshInstance->motionBlur.y >> meshInstance->motionBlur.z;
        stream.clear();*/

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
                        meshInstance->SetTransformationMatrix(Transformation::GetTranslationMatrix(scene->translations[transformationId - 1]) * meshInstance->transformationMatrix);
                        break;
                    case 'r':
                        meshInstance->SetTransformationMatrix(Transformation::GetRotationMatrix(scene->rotations[transformationId - 1]) * meshInstance->transformationMatrix);
                        break;
                    case 's':
                        meshInstance->SetTransformationMatrix(Transformation::GetScalingMatrix(scene->scalings[transformationId - 1]) * meshInstance->transformationMatrix);
                        break;
                }
            }
        }
        meshInstance->SetInverseTransformationMatrix();

        scene->objects.push_back(meshInstance);
        element = element->NextSiblingElement("MeshInstance");
        stream.clear();
    }
    
    //Get Spheres
    element = root->FirstChildElement("Objects");
    element = element->FirstChildElement("Sphere");
    
    while (element)
    {
        Sphere *sphere = new Sphere();
        
        int materialId = 0;
        child = element->FirstChildElement("Material");
        stream << child->GetText() << std::endl;
        stream >> materialId;
        sphere->material = &scene->materials[materialId - 1];
        stream.clear();

        int textureId;
        child = element->FirstChildElement("Texture");
        if(child)
        {
            stream << child->GetText() << std::endl;
            stream >> textureId;
            sphere->texture = scene->textures[textureId - 1];
        }
        else
        {
            sphere->texture = nullptr;
        }
        stream.clear();

        child = element->FirstChildElement("Center");
        stream << child->GetText() << std::endl;
        unsigned int centerVertexId;
        stream >> centerVertexId;
        sphere->center = scene->vertices[centerVertexId - 1];
        stream.clear();

        child = element->FirstChildElement("Radius");
        stream << child->GetText() << std::endl;
        stream >> sphere->radius;
        stream.clear();

        /*element = root->FirstChildElement("MotionBlur");
        if (element)
        {
            stream << element->GetText() << std::endl;
        }
        else
        {
            stream << "0 0 0" << std::endl;
        }
        stream >> sphere->motionBlur.x >> sphere->motionBlur.y >> sphere->motionBlur.z;*/

        child = element->FirstChildElement("Transformations");
        if(child)
        {
            stream << child->GetText() << std::endl;
            
            std::string transformationEncoding;
            while(stream >> transformationEncoding && transformationEncoding.length() > 0)
            {
                char transformationType = transformationEncoding[0];
                int transformationId = std::stoi(transformationEncoding.substr(1));
                switch (transformationType)
                {
                    case 't':
                        sphere->transformationMatrix = Transformation::GetTranslationMatrix(scene->translations[transformationId - 1]) * sphere->transformationMatrix;
                        break;
                    case 'r':
                        sphere->transformationMatrix = Transformation::GetRotationMatrix(scene->rotations[transformationId - 1]) * sphere->transformationMatrix;
                        break;
                    case 's':
                        sphere->transformationMatrix = Transformation::GetScalingMatrix(scene->scalings[transformationId - 1]) * sphere->transformationMatrix;
                        break;
                }
            }
        }
        sphere->SetInverseTransformationMatrix();
        
        scene->objects.push_back(sphere);
        element = element->NextSiblingElement("Sphere");
        stream.clear();
    }
}