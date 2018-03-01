SRC = 	Camera.cpp \
		Color.cpp \
		Light.cpp \
		Material.cpp \
		Math.cpp \
		Mesh.cpp \
		ObjectBase.cpp \
		PointLight.cpp \
		Raytracer.cpp \
		Renderer.cpp \
		Scene.cpp \
		Sphere.cpp \
		./Externals/tinyxml2.cpp \

#OBJ = $(SRC:.cpp=.o)

OBJ = $(SRC_OBJ)
CFLAGS = -std=c++11 -O3

.cpp.o:
	g++ -c $< $(CFLAGS)

all: clean $(OBJ)
			g++ $(OBJ) -o raytracer $(CFLAGS)

clean:
	rm -f *.o main

clean_everything:
	rm -f *.o main *.ppm
