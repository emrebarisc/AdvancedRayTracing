SRC = 	AreaLight.cpp \
		BoundingVolume.cpp \
		BVH.cpp \
		Camera.cpp \
		Color.cpp \
		Light.cpp \
		Material.cpp \
		Math.cpp \
		Matrix.cpp \
		Mesh.cpp \
		ObjectBase.cpp \
		OutputManager.cpp \
		PointLight.cpp \
		Ray.cpp \
		Raytracer.cpp \
		Renderer.cpp \
		Scene.cpp \
		SceneParser.cpp \
		Sphere.cpp \
		Transformations.cpp \
		tinyxml2.cpp

EXECUTABLES_FOLDER = Executables

OBJ = $(SRC:.cpp=.o)

CFLAGS = -lpng -g -lpthread -Wall -Wextra -pedantic -ansi -std=c++11 -O3
#-fauto-inc-dec -fbranch-count-reg -fcombine-stack-adjustments -fcompare-elim -fcprop-registers \ -fdce -fdefer-pop -fdelayed-branch -fdse -fforward-propagate -fguess-branch-probability -fif-conversion2 -fif-conversion -finline-functions-called-once -fipa-pure-const -fipa-profile -fipa-reference -fmerge-constants -fmove-loop-invariants -fomit-frame-pointer -freorder-blocks -fshrink-wrap -fshrink-wrap-separate -fsplit-wide-types -fssa-backprop -fssa-phiopt -ftree-bit-ccp -ftree-ccp -ftree-ch -ftree-coalesce-vars -ftree-copy-prop -ftree-dce -ftree-dominator-opts -ftree-dse -ftree-forwprop -ftree-fre -ftree-phiprop -ftree-sink -ftree-slsr -ftree-sra -ftree-pta -ftree-ter -funit-at-a-time -fthread-jumps -falign-functions  -falign-jumps -falign-loops  -falign-labels -fcaller-saves -fcrossjumping -fcse-follow-jumps  -fcse-skip-blocks -fdelete-null-pointer-checks -fdevirtualize -fdevirtualize-speculatively -fexpensive-optimizations -fgcse  -fgcse-lm  -fhoist-adjacent-loads -finline-small-functions -findirect-inlining -fipa-cp -fipa-bit-cp -fipa-vrp -fipa-sra -fipa-icf -fisolate-erroneous-paths-dereference -flra-remat -foptimize-sibling-calls -foptimize-strlen -fpartial-inlining -fpeephole2 -freorder-blocks-algorithm=stc -freorder-blocks-and-partition -freorder-functions -frerun-cse-after-loop  -fsched-interblock  -fsched-spec -fschedule-insns  -fschedule-insns2 -fstore-merging -fstrict-aliasing -ftree-builtin-call-dce -ftree-switch-conversion -ftree-tail-merge -fcode-hoisting -ftree-pre -ftree-vrp -fipa-ra -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-loop-vectorize -ftree-loop-distribution -ftree-loop-distribute-patterns -floop-interchange -fsplit-paths -ftree-slp-vectorize -fvect-cost-model -ftree-partial-pre -fpeel-loops -fipa-cp-clone

.cpp.o:
	g++ -c $< $(CFLAGS)

all: clean $(OBJ)
			g++ $(OBJ) -o raytracer $(CFLAGS)

clean:
	rm -f *.o raytracer

clean_everything:
	rm -f *.o raytracer *.ppm *.png
