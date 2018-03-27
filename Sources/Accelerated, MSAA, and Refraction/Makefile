SRC = 	BoundingVolume.cpp \
		BVH.cpp \
		Camera.cpp \
		Color.cpp \
		Light.cpp \
		Material.cpp \
		Math.cpp \
		Mesh.cpp \
		ObjectBase.cpp \
		OutputManager.cpp \
		PointLight.cpp \
		Raytracer.cpp \
		Renderer.cpp \
		Scene.cpp \
		SceneParser.cpp \
		Sphere.cpp \
		tinyxml2.cpp

EXECUTABLES_FOLDER = Executables

OBJ = $(SRC:.cpp=.o)

CFLAGS = -std=c++11 -lpng -g -lpthread -O3

.cpp.o:
	g++ -c $< $(CFLAGS)

all: clean $(OBJ)
			g++ $(OBJ) -o raytracer $(CFLAGS)

clean:
	rm -f *.o raytracer

clean_everything:
	rm -f *.o raytracer *.ppm *.png
