import os
import math
from enum import Enum

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
    
    def GetNormalized(self):
        length = math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
        if(length != 0):
            normalized = Vector3(self.x / length, self.y / length, self.z / length)
            return normalized
        return self
    
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
    
    nearPlane = Vector4(-0.5, 0.5, -0.5, 0.5)
    nearDistance = 1
    imageResolution = Vector2(0, 0)
    
    gaze = Vector3(0, 0, 0)
    up = Vector3(0, 0, 0)
    position = Vector3(0, 0, 0)
    
    fovY = 0
    
    gazePoint = Vector3(0, 0, 0)
    
    numOfSamples = 1
    
    imageName = ""
    
    def __init__(self):
        self.id = 0
    
        self.nearPlane = Vector4(-0.5, 0.5, -0.5, 0.5)
        self.nearDistance = 1
        self.imageResolution = Vector2(0, 0)
        
        self.gaze = Vector3(0, 0, 0)
        self.up = Vector3(0, 0, 0)
        self.position = Vector3(0, 0, 0)
        
        self.numOfSamples = 1
        
        self.imageName = ""
        
    def SetNearPlaneWithRatio(self, ratio):
        nearPlaneVertical = 0.5 / ratio
            
        self.nearPlane = Vector4(-0.5, 0.5, -nearPlaneVertical, nearPlaneVertical)
    
    def Export(self):
        fileBuffer = '\t\t<Camera id = "' + str(self.id) + '">\n'
        
        # Simple camera
        #fileBuffer = '\t\t<Camera id = "' + str(self.id) + '" type = "simple">\n'
        
        # fileBuffer += '\t\t\t<FovY>' + str(self.fovY) + '</FovY>\n'
        
        # fileBuffer += '\t\t\t<GazePoint>' 
        # fileBuffer += str(self.gazePoint.x) + ' ' + str(self.gazePoint.y) + ' ' + str(self.gazePoint.z) 
        # fileBuffer += '</GazePoint> \n'
        
        # Camera position
        fileBuffer += '\t\t\t<Position>'
        fileBuffer += str(self.position.x) + ' ' + str(self.position.y) + ' ' + str(self.position.z)
        fileBuffer += '</Position>\n'
        
        # Camera gaze vector
        fileBuffer += '\t\t\t<Gaze>'
        fileBuffer += str(self.gaze.x) + ' ' + str(self.gaze.y) + ' ' + str(self.gaze.z)
        fileBuffer += '</Gaze>\n'
        
        # Camera up vector
        fileBuffer += '\t\t\t<Up>'
        fileBuffer += str(self.up.x) + ' ' + str(self.up.y) + ' ' + str(self.up.z)
        fileBuffer += '</Up>\n'
        
        # Near plane is only for not simple cameras
        fileBuffer += '\t\t\t<NearPlane>' + str(self.nearPlane.x) + ' ' + str(self.nearPlane.y) + ' ' + str(self.nearPlane.z) + ' ' + str(self.nearPlane.w) + '</NearPlane>\n'
        
        fileBuffer += '\t\t\t<NearDistance>' + str(self.nearDistance) + '</NearDistance>\n'
        
        fileBuffer += '\t\t\t<ImageResolution>' + str(self.imageResolution.x) + ' ' + str(self.imageResolution.y) + '</ImageResolution>\n'
        
        fileBuffer += '\t\t\t<NumSamples>' + str(self.numOfSamples) + '</NumSamples>\n'
        
        fileBuffer += '\t\t\t<ImageName>' + self.imageName + '</ImageName>\n'
        
        fileBuffer += '\t\t</Camera>\n'
        
        return fileBuffer

class Light:
    id = 0
    intensity = [0, 0, 0]

class PointLight(Light):
    position = [0, 0, 0]
    
    def Export(self):
        fileBuffer = '\t\t<PointLight id = "' + str(self.id) + '">\n'
        
        # Position
        fileBuffer += '\t\t\t<Position>'
        fileBuffer += str(self.position.x) + ' ' + str(self.position.y) + ' ' + str(self.position.z)
        fileBuffer += '</Position>\n'
        
        # Intensity
        fileBuffer += '\t\t\t<Intensity>'
        fileBuffer += str(self.intensity.x) + ' ' + str(self.intensity.y) + ' ' + str(self.intensity.z)
        fileBuffer += '</Intensity>\n'
        
        fileBuffer += '\t\t</PointLight>\n'
        
        return fileBuffer
        
class DirectionalLight(Light):
    direction = [0, 0, 0]
    
    def Export(self):
        fileBuffer = '\t\t<DirectionalLight id = "' + str(self.id) + '">\n'
        
        # Direction
        fileBuffer += '\t\t\t<Direction>'
        fileBuffer += str(self.direction.x) + ' ' + str(self.direction.y) + ' ' + str(self.direction.z)
        fileBuffer += '</Direction>\n'
        
        # Radiance
        fileBuffer += '\t\t\t<Radiance>'
        fileBuffer += str(self.intensity.x) + ' ' + str(self.intensity.y) + ' ' + str(self.intensity.z)
        fileBuffer += '</Radiance>\n'
        
        fileBuffer += '\t\t</DirectionalLight>\n'
        
        return fileBuffer
    
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
    refractionIndex = 0
    specular = Vector3(0, 0, 0)
    phongExponent = 3
    roughness = 0
    
    def SetDefaultMaterial(self):
        self.ambient = Vector3(0.2, 0.2, 0.2)
        self.diffuse = Vector3(0.23, 0.46, 0.92)
        self.mirror = Vector3(0, 0, 0)
        self.transparency = Vector3(1, 1, 1)
        self.refractionIndex = 0
        self.specular = Vector3(0.2, 0.2, 0.2)
        self.phongExponent = 3
        self.roughness = 0
    
    def Export(self):
        fileBuffer = '\t\t<Material id = "' + str(self.id) + '">\n'
        
        fileBuffer += '\t\t\t<AmbientReflectance>'
        fileBuffer += str(self.ambient.x) + ' ' + str(self.ambient.y) + ' ' + str(self.ambient.z)
        fileBuffer += '</AmbientReflectance>\n'
        
        fileBuffer += '\t\t\t<DiffuseReflectance>'
        fileBuffer += str(self.diffuse.x) + ' ' + str(self.diffuse.y) + ' ' + str(self.diffuse.z)
        fileBuffer += '</DiffuseReflectance>\n'
        
        fileBuffer += '\t\t\t<SpecularReflectance>'
        fileBuffer += str(self.specular.x) + ' ' + str(self.specular.y) + ' ' + str(self.specular.z)
        fileBuffer += '</SpecularReflectance>\n'
        
        fileBuffer += '\t\t\t<PhongExponent>' + str(self.phongExponent) + '</PhongExponent>\n'
        
        fileBuffer += '\t\t\t<MirrorReflectance>'
        fileBuffer += str(self.mirror.x) + ' ' + str(self.mirror.y) + ' ' + str(self.mirror.z)
        fileBuffer += '</MirrorReflectance>\n'
        
        fileBuffer += '\t\t\t<Transparency>'
        fileBuffer += str(self.transparency.x) + ' ' + str(self.transparency.y) + ' ' + str(self.transparency.z)
        fileBuffer += '</Transparency>\n'
        fileBuffer += '\t\t\t<RefractionIndex>' + str(self.refractionIndex) + '</RefractionIndex>\n'
        
        fileBuffer += '\t\t\t<Roughness>' + str(self.roughness) + '</Roughness>\n'
        
        fileBuffer += '\t\t</Material>\n'
        
        return fileBuffer
    
class Mesh:
    id = 0
    faces = []
    translation = 0
    rotation = 0
    scaling = 0
    material = 0
    isSmoothShaded = None
    
    def __init__(self):
        self.id = 0
        self.faces = []
        self.translation = 0
        self.rotation = 0
        self.scaling = 0
        self.material = 0
        isSmoothShaded = None
    
    def Export(self):
        fileBuffer = '\t\t<Mesh id = "' + str(self.id) + '" shadingMode = "' + ('smooth' if self.isSmoothShaded else 'flat') + '">\n'
        
        fileBuffer += '\t\t\t<Material>' + str(self.material) + '</Material>\n'
                
        fileBuffer += '\t\t\t<Transformations>' + 't' + str(self.translation) + ' r' + str(self.rotation) + ' s' + str(self.scaling) + '</Transformations>\n'
                
        fileBuffer += '\t\t\t<Faces>\n'
        for face in self.faces:
            fileBuffer += '\t\t\t\t' + str(face.x) + ' ' + str(face.y) + ' ' + str(face.z) + '\n'
        fileBuffer += '\t\t\t</Faces>\n'
        
        fileBuffer += '\t\t</Mesh>\n'
        
        return fileBuffer
    
class Scene:
    
    def __init__(self):
        self.cameras = []
        self.lights = []
        self.materials = []
        self.vertices = []
        self.vertexNormals = []
        self.textureCoordinates = []
        self.textures = []
        self.meshes = []
        self.translations = []
        self.rotations = []
        self.scalings = []
    
    cameras = []
    lights = []
    materials = []
    vertices = []
    vertexNormals = []
    textureCoordinates = []
    textures = []
    meshes = []
    translations = []
    rotations = []
    scalings = []
    
    ambientLight = Vector3(100, 100, 100)
    bgColor = Vector3(0, 0, 0)
    
    maxRecursionDepth = 6
    
    def ExportScene(self, filePath):
        fileBuffer = '<Scene>\n'
        
        fileBuffer += '\t<BackgroundColor>' + str(int(self.bgColor.x)) + ' ' + str(int(self.bgColor.y)) + ' ' + str(int(self.bgColor.z)) + '</BackgroundColor>\n'

        # Cameras
        fileBuffer += '\t<Cameras>\n'
        for camera in self.cameras:
            fileBuffer += camera.Export()
        fileBuffer += '\t</Cameras>\n\n'

        # Lights
        fileBuffer += '\t<Lights>\n'
        fileBuffer += '\t\t<AmbientLight>' + str(self.ambientLight.x) + ' ' + str(self.ambientLight.y) + ' ' + str(self.ambientLight.z) + '</AmbientLight>\n'
        for light in self.lights:
            fileBuffer += light.Export()
        fileBuffer += '\t</Lights>\n\n'

        # Materials
        fileBuffer += '\t<Materials>\n'
        for material in self.materials:
            fileBuffer += material.Export()
        fileBuffer += '\t</Materials>\n\n'
        
        #Transformations
        fileBuffer += '\t<Transformations>\n'
        translationId = 0
        for translation in self.translations:
            translationId += 1
            fileBuffer += '\t\t' + '<Translation id = "' + str(translationId) + '">'
            fileBuffer += str(translation.x) + " " + str(translation.y) + " " + str(translation.z)
            fileBuffer += '</Translation>\n'
            
        scalingId = 0
        for scaling in self.scalings:
            scalingId += 1
            fileBuffer += '\t\t' + '<Scaling id = "' + str(scalingId) + '">'
            fileBuffer += str(scaling.x) + " " + str(scaling.y) + " " + str(scaling.z)
            fileBuffer += '</Scaling>\n'
            
        rotationId = 0
        for rotation in self.rotations:
            rotationId += 1
            fileBuffer += '\t\t' + '<Rotation id = "' + str(rotationId) + '">'
            fileBuffer += str(180) + " " + str(rotation.x / math.pi) + " " + str(rotation.y / math.pi) + " " + str(rotation.z / math.pi)
            fileBuffer += '</Rotation>\n'
        fileBuffer += '\t</Transformations>\n\n'
        
        # Vertex Data
        fileBuffer += '\t<VertexData>\n'
        for vertex in self.vertices:
            fileBuffer += '\t\t' + str(vertex.x) + ' ' + str(vertex.y) + ' ' + str(vertex.z) + '\n'
        fileBuffer += '\t</VertexData>\n\n'

        # Texture Coordinates
        fileBuffer += '\t<TexCoordData>\n'
        for coordinate in self.textureCoordinates:
            fileBuffer += '\t\t' + str(coordinate.x) + ' ' + str(coordinate.y) + '\n'
        fileBuffer += '\t</TexCoordData>\n\n'

        # Meshes
        fileBuffer += '\t<Objects>\n'
        for mesh in self.meshes:
            fileBuffer += mesh.Export()
        fileBuffer += '\t</Objects>\n\n'

        fileBuffer += '</Scene>'

        with open(filePath, 'w') as file:
            file.write(fileBuffer)