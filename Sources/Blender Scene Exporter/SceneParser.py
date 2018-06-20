import bpy
from mathutils import Vector
import os

from SceneData import *

# Writes to an XML file named same with the blender file

def ParseScene():
    fileName = os.path.splitext(bpy.data.filepath)[0] + ".xml"

    scene = bpy.context.scene

    sceneData = Scene()
    sceneData.bgColor = Vector3(255 - scene.render.stamp_foreground[0] * 255, 255 - scene.render.stamp_foreground[1] * 255, 255 - scene.render.stamp_foreground[2] * 255)
    sceneData.ambientLight = Vector3(scene.world.ambient_color[0], scene.world.ambient_color[1], scene.world.ambient_color[2])

    #Cameras
    cameraId = 1
    for obj in scene.objects:
        if(obj.type == 'CAMERA'):
            
            gazeVector = obj.matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0))
            upVector = obj.matrix_world.to_quaternion() * Vector((0.0, 1.0, 0.0))
            
            camera = Camera()
            camera.position = Vector3(obj.location[0], obj.location[1], obj.location[2])
            camera.gaze = Vector3(gazeVector.x, gazeVector.y, gazeVector.z)
            camera.up = Vector3(upVector.x, upVector.y, upVector.z)
            camera.imageName = bpy.path.display_name_from_filepath(fileName) + '_' + str(cameraId) + '.png'
            
            camera.imageResolution = Vector2(int(scene.render.resolution_x * scene.render.resolution_percentage / 100), int(scene.render.resolution_y * scene.render.resolution_percentage / 100))
            
            ratio = scene.render.resolution_x / scene.render.resolution_y
            camera.SetNearPlaneWithRatio(ratio)
            
            #camera.nearPlane = Vector4(scene.render.border_min_x, scene.render.border_max_x, scene.render.border_min_y, scene.render.border_max_y)
            #camera.nearDistance = obj.data.clip_start
            
            camera.numOfSamples = bpy.context.scene.render.antialiasing_samples
            
            camera.fovY = obj.data.angle_y / 3.01675 * 180
            
            cameraForwardVector = obj.matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0)) * camera.nearDistance
            camera.gazePoint = Vector3(camera.position.x + cameraForwardVector[0], camera.position.y + cameraForwardVector[1], camera.position.z + cameraForwardVector[2])
            
            camera.id = cameraId
            cameraId += 1
            
            sceneData.cameras.append(camera)

    # Lights
    pointLightId = 1
    for obj in scene.objects:
        if(obj.type == 'LAMP'):
            
            objDataType = getattr(obj.data, 'type', '')
            
            light = None
            
            # Point Light
            if(objDataType == 'POINT'):
                light = PointLight()
                
                light.id = pointLightId
                light.position = Vector3(obj.location[0], obj.location[1], obj.location[2])
                
                # Intensity
                intensityColor = obj.data.node_tree.nodes["Emission"].inputs[0].default_value
                intensityValue = obj.data.node_tree.nodes["Emission"].inputs[1].default_value
                intensity = [intensityColor[0] * intensityValue, intensityColor[1] * intensityValue, intensityColor[2] * intensityValue]
                
                light.intensity = Vector3(intensity[0], intensity[1], intensity[2])
            
                pointLightId += 1
                
            elif(objDataType == 'SUN'):
                light = DirectionalLight()
                light.id = pointLightId
                
                direction = Vector3(obj.rotation_euler[0], obj.rotation_euler[1], obj.rotation_euler[2]) 
                normalizedDirection = direction.GetNormalized()
                light.direction = Vector3(normalizedDirection.x, normalizedDirection.y, normalizedDirection.z) 
                           
                # Intensity
                intensityColor = obj.data.node_tree.nodes["Emission"].inputs[0].default_value
                intensityValue = obj.data.node_tree.nodes["Emission"].inputs[1].default_value
                intensity = [intensityColor[0] * intensityValue, intensityColor[1] * intensityValue, intensityColor[2] * intensityValue]
                
                light.intensity = Vector3(intensity[0], intensity[1], intensity[2])
                
                pointLightId += 1
                print('Directional Light: ' + obj.name)

            elif(objDataType == 'SPOT'):
                print('Spot Light: ' + obj.name)
                
            
            sceneData.lights.append(light)
    
    # Meshes
    meshId = 1
    materialId = 1
    translationId = 1
    rotationId = 1
    scalingId = 1
    vertexOffset = 0
    for blenderMesh in bpy.data.meshes:
        mesh = Mesh()
        mesh.id = meshId
        
        location = scene.objects[blenderMesh.name].location
        rotation = scene.objects[blenderMesh.name].rotation_euler
        scaling = scene.objects[blenderMesh.name].scale
        
        sceneData.translations.append(Vector3(location[0], location[1], location[2]))
        sceneData.rotations.append(Vector3(rotation[0], rotation[1], rotation[2]))
        sceneData.scalings.append(Vector3(scaling[0], scaling[1], scaling[2]))
        
        mesh.translation = translationId
        mesh.rotation = rotationId
        mesh.scaling = scalingId
        
        translationId += 1
        rotationId += 1
        scalingId += 1
        
        mesh.isSmoothShaded = blenderMesh.polygons[0].use_smooth
        
        material = Material()
        material.id = materialId
        # Get first face's material as mesh material
        if(len(blenderMesh.materials) > 0):
            blenderMaterial = blenderMesh.materials[blenderMesh.polygons[0].material_index]
            
            ambient = blenderMaterial.ambient
            material.ambient = Vector3(ambient, ambient, ambient)
            
            material.SetDefaultMaterial()
            diffuse = blenderMaterial.diffuse_color
            material.diffuse = Vector3(diffuse[0], diffuse[1], diffuse[2])
            
            specular = blenderMaterial.specular_color
            material.specular = Vector3(specular[0], specular[1], specular[2])
            material.phongExponent = blenderMaterial.specular_hardness
            
            transparency = blenderMaterial.alpha
            material.transparency = Vector3(transparency, transparency, transparency)
            material.refractionIndex = blenderMaterial.raytrace_transparency.fresnel
            
            mirror = blenderMaterial.raytrace_mirror.reflect_factor
            material.mirror = Vector3(mirror, mirror, mirror)
            
            material.roughness = blenderMaterial.roughness
        else:
            # Default material for the objects without any material
            material.SetDefaultMaterial()

        sceneData.materials.append(material)

        mesh.material = materialId
        materialId += 1
        
        for uv_layer in blenderMesh.uv_layers:
            for x in range(len(uv_layer.data)):
                #print(str(uv_layer.data[x].uv[0]) + " " + str(uv_layer.data[x].uv[1]))
                sceneData.textureCoordinates.append(Vector2(uv_layer.data[x].uv[0], uv_layer.data[x].uv[1]))
        
        for face in blenderMesh.polygons:
            mesh.faces.append(Vector3(face.vertices[0] + 1 + vertexOffset, face.vertices[1] + 1 + vertexOffset, face.vertices[2] + 1 + vertexOffset))
            
        for vertex in blenderMesh.vertices:
            sceneData.vertices.append(Vector3(vertex.co[0], vertex.co[1], vertex.co[2]))
            vertexOffset += 1
            
        sceneData.meshes.append(mesh)
        meshId += 1
    
    sceneData.ExportScene(fileName)
    
if __name__ == '__main__':
    ParseScene()
    
    print(bpy.context.scene)