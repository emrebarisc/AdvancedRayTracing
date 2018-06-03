import bpy
from mathutils import Vector
import os

# Missing parts
#   - Multi Sampling Amount
#   - Ambient Light(Blender does not have scene ambient light)

backgroundColor = 0
print(backgroundColor)

# Write XML file named same with the blender file
fileName = os.path.splitext(bpy.data.filepath)[0]
file = open(fileName + ".xml", 'w')

file.write('<Scene>\n')

scene = bpy.context.scene

#Cameras
cameraId = 1;
file.write('\t<Cameras>\n')

for obj in scene.objects:
    if(obj.type == 'CAMERA'):
        file.write('\t\t<Camera id = "' + str(cameraId) + '">\n')
        
        # Camera position
        file.write('\t\t\t<Position>')
        file.write(str(obj.location[0]) + ' ' + str(obj.location[1]) + ' ' + str(obj.location[2]))
        file.write('</Position>\n')
        
        # Camera gaze vector
        gazeVector = obj.matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0))
        file.write('\t\t\t<Gaze>')
        file.write(str(gazeVector.x) + ' ' + str(gazeVector.y) + ' ' + str(gazeVector.z))
        file.write('</Gaze>\n')
        
        # Camera up vector
        upVector = obj.matrix_world.to_quaternion() * Vector((0.0, 1.0, 0.0))
        file.write('\t\t\t<Up>')
        file.write(str(upVector.x) + ' ' + str(upVector.y) + ' ' + str(upVector.z))
        file.write('</Up>\n')

        file.write('\t\t\t<NearPlane>-0.1 0.1 -0.1 0.1</NearPlane>\n')
        
        file.write('\t\t\t<NearDistance>1</NearDistance>\n')
        
        file.write('\t\t\t<ImageResolution>' + str(int(scene.render.resolution_x * scene.render.resolution_percentage / 100)) + ' ' + str(int(scene.render.resolution_y * scene.render.resolution_percentage / 100)) + '</ImageResolution>\n')
        
        file.write('\t\t\t<NumSamples>1</NumSamples>\n')
        
        file.write('\t\t\t<ImageName>' + bpy.path.display_name_from_filepath(fileName) + '_' + str(cameraId) + '.png</ImageName>\n')
        
        file.write('\t\t</Camera>\n')
        cameraId += 1
        
file.write('\t</Cameras>\n')

# Lights
file.write('\t<Lights>\n')

# Ambient Light
file.write('\t\t\t<AmbientLight>100 100 100</AmbientLight>\n')

pointLightId = 1
directionalLightId = 1
spotLightId = 1

for obj in scene.objects:
    if(obj.type == 'LAMP'):
        
        objDataType = getattr(obj.data, 'type', '')
        
        # Point Light
        if(objDataType == 'POINT'):
            file.write('\t\t\t<PointLight id = "' + str(pointLightId) + '">\n')
            
            # Position
            file.write('\t\t\t\t<Position>')
            file.write(str(obj.location[0]) + ' ' + str(obj.location[1]) + ' ' + str(obj.location[2]))
            file.write('</Position>\n')
            
            # Intensity
            intensityColor = obj.data.node_tree.nodes["Emission"].inputs[0].default_value
            intensityValue = obj.data.node_tree.nodes["Emission"].inputs[1].default_value
            
            intensity = [intensityColor[0] * intensityValue, intensityColor[1] * intensityValue, intensityColor[2] * intensityValue]
            
            file.write('\t\t\t\t<Intensity>')
            file.write(str(intensity[0]) + ' ' + str(intensity[1]) + ' ' + str(intensity[2]))
            file.write('</Intensity>\n')
            
            pointLightId += 1
            file.write('\t\t\t</PointLight>\n')
            
        elif(objDataType == 'SUN'):
            print('Directional Light: ' + obj.name)

        elif(objDataType == 'SPOT'):
            print('Spot Light: ' + obj.name)
        
file.write('\t</Lights>\n')
        
for obj in scene.objects:  
    if(obj.type == 'MESH'):
        print(obj.data)
        

file.write('</Scene>')

file.close()