import os

class Vector2:        
    def __init__(self, _x, _y):
        self.x = _x
        self.y = _y
    
    x = 0
    y = 0

class Vector3:        
    def __init__(self, _x, _y, _z):
        self.x = _x
        self.y = _y
        self.z = _z
    
    x = 0
    y = 0
    z = 0

class Vector4:        
    def __init__(self, _x, _y, _z, _w):
        self.x = _x
        self.y = _y
        self.z = _z
        self.w = _w
    
    x = 0
    y = 0
    z = 0
    w = 0

class Camera:
    id = 0
    
    nearPlane = Vector4(-0.1, 0.1, -0.1, 0.1)
    nearDistance = 1
    imageResolution = Vector2(0, 0)
    
    gaze = Vector3(0, 0, 0)
    up = Vector3(0, 0, 0)
    position = Vector3(0, 0, 0)
    
    numOfSamples = 1
    
    imageName = ""
    
    def ExportCamera(self, filePath):
        file = open(filePath, "a")
        file.write('\t\t<Camera id = "' + str(self.id) + '">\n')
        
        # Camera position
        file.write('\t\t\t<Position>')
        file.write(str(self.position.x) + ' ' + str(self.position.y) + ' ' + str(self.position.z))
        file.write('</Position>\n')
        
        # Camera gaze vector
        file.write('\t\t\t<Gaze>')
        file.write(str(self.gaze.x) + ' ' + str(self.gaze.y) + ' ' + str(self.gaze.z))
        file.write('</Gaze>\n')
        
        # Camera up vector
        file.write('\t\t\t<Up>')
        file.write(str(self.up.x) + ' ' + str(self.up.y) + ' ' + str(self.up.z))
        file.write('</Up>\n')
        
        file.write('\t\t\t<NearPlane>' + str(self.nearPlane.x) + ' ' + str(self.nearPlane.y) + ' ' + str(self.nearPlane.z) + ' ' + str(self.nearPlane.w) + '</NearPlane>\n')
        
        file.write('\t\t\t<NearDistance>' + str(self.nearDistance) + '</NearDistance>\n')
        
        file.write('\t\t\t<ImageResolution>' + str(self.imageResolution.x) + ' ' + str(self.imageResolution.y) + '</ImageResolution>\n')
        
        file.write('\t\t\t<NumSamples>' + str(self.numOfSamples) + '</NumSamples>\n')
        
        file.write('\t\t\t<ImageName>' + self.imageName + '</ImageName>\n')
        
        file.write('\t\t</Camera>\n')
        
        return file

class Light:
    id = 0
    intensity = [0, 0, 0]

class PointLight(Light):
    position = [0, 0, 0]
    
    def ExportLight(self, filePath):
        file = open(filePath, "a")
        file.write('\t\t<PointLight id = "' + str(self.id) + '">\n')
        
        # Position
        file.write('\t\t\t<Position>')
        file.write(str(self.position.x) + ' ' + str(self.position.y) + ' ' + str(self.position.z))
        file.write('</Position>\n')
        
        # Intensity
        file.write('\t\t\t<Intensity>')
        file.write(str(self.intensity.x) + ' ' + str(self.intensity.y) + ' ' + str(self.intensity.z))
        file.write('</Intensity>\n')
        
        file.write('\t\t</PointLight>\n')
        
        return file
        

class DirectionalLight(Light):
    direction = [0, 0, 0]
    
class SpotLight(Light):
    intensity = [0, 0, 0]
    coverageAngle = 0
    falloffAngle = 0
    
class Material:
    id = 0
    ambient = Vector3(0, 0, 0)
    diffuse = Vector3(0, 0, 0)
    mirror = Vector3(0, 0, 0)
    transparency = Vector3(0, 0, 0)
    specular = Vector3(0, 0, 0)
    phongExponent = 3
    
    def Export(self, filePath):
        file = open(filePath, "a")
        
        file.write('\t\t<Material id = "' + str(self.id) + '">\n')
        
        file.write('\t\t\t<AmbientReflectance>')
        file.write(str(self.ambient.x) + ' ' + str(self.ambient.y) + ' ' + str(self.ambient.z))
        file.write('</AmbientReflectance>\n')
        
        file.write('\t\t\t<DiffuseReflectance>')
        file.write(str(self.diffuse.x) + ' ' + str(self.diffuse.y) + ' ' + str(self.diffuse.z))
        file.write('</DiffuseReflectance>\n')
        
        # TODO
        file.write('\t\t\t<SpecularReflectance>')
        file.write(str(self.specular.x) + ' ' + str(self.specular.y) + ' ' + str(self.specular.z))
        file.write('</SpecularReflectance>\n')
        
        file.write('\t\t\t<PhongExponent>' + str(self.phongExponent) + '</PhongExponent>\n')
        
        file.write('\t\t</Material>\n')
        
        return file
    
class Mesh:
    faces = []
    material = None
    
class Scene:
    
    def __init__(self):
        del self.cameras[:]
        del self.lights[:]
        del self.materials[:]
        del self.vertices[:]
        del self.vertexNormals[:]
        del self.textureCoordinates[:]
        del self.textures[:]
        del self.meshes[:]
    
    cameras = []
    lights = []
    materials = []
    vertices = []
    vertexNormals = []
    textureCoordinates = []
    textures = []
    meshes = []
    
    ambientLight = Vector3(100, 100, 100)
    bgColor = [0, 0, 0]
    
    maxRecursionDepth = 6
    
    def ExportScene(self, filePath):
        file = open(filePath, 'w')

        file.write('<Scene>\n')
        
        # Cameras
        file.write('\t<Cameras>\n')
        for camera in self.cameras:
            file = camera.ExportCamera(filePath)
        file.write('\t</Cameras>\n\n')
        
        # Lights
        file.write('\t<Lights>\n')
        file.write('\t\t<AmbientLight>' + str(self.ambientLight.x) + ' ' + str(self.ambientLight.y) + ' ' + str(self.ambientLight.z) + '</AmbientLight>\n')
        for light in self.lights:
            file = light.ExportLight(filePath)
        file.write('\t</Lights>\n\n')
            
        # Materials
        file.write('\t<Materials>\n')
        for material in self.materials:
            file = material.Export(filePath)
        file.write('\t</Materials>\n\n')
                
        file.write('</Scene>')
        
        file.close()