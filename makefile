############################################################
CXX = g++
CXXFLAGS = -g -Wall -Wextra -pedantic -lGL -lGLU -lglut -lGLEW -lpng -std=c++11
############################################################

OUTPUT_NAME = AABB-raycasting-3d

all: main.o grid_SaP.o AABBraycast3d.o
	$(CXX) $(CXXFLAGS) -o $(OUTPUT_NAME) main.o grid_SaP.o AABBraycast3d.o

main.o: main.cpp grid_SaP.h AABBraycast3d.h
	$(CXX) $(CXXFLAGS) -c main.cpp

grid_SaP.o: grid_SaP.h

AABBraycast3d.o: AABBraycast3d.h

clean:
	$(RM) main.o grid_SaP.o

run: $(OUTPUT_NAME)
	./$(OUTPUT_NAME)

valgrind: $(OUTPUT_NAME)
	valgrind --tool=memcheck --track-origins=yes --leak-check=yes --num-callers=20 --track-fds=yes --suppressions=minimal.supp ./$(OUTPUT_NAME)
