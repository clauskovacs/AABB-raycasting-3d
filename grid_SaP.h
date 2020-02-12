#ifndef __GRID_SAP_H_INCLUDED__
#define __GRID_SAP_H_INCLUDED__

#include <math.h>

struct colliders
{
	public:
		// spatial extension (coordinates) of the AABB-box
		float xmin;
		float xmax;

		float ymin;
		float ymax;

		float zmin;
		float zmax;

		// cells in which the AABB-box resides
		int cell_xMin;
		int cell_xMax;

		int cell_yMax;
		int cell_yMin;

		int cell_zMin;
		int cell_zMax;

		// velocities of the AABB-box
		float vx;
		float vy;
		float vz;

        // state of collision
 		bool colliding;
};

class grid
{
	public:
		// power of two is suggested / mandatory to work
        float gridBoundaryMinX = -64.0;
        float gridBoundaryMaxX = 64.0;

		float gridBoundaryMinY = -64.0;
        float gridBoundaryMaxY = 64.0;

		float gridBoundaryMinZ = -64.0;
        float gridBoundaryMaxZ = 64.0;


		// power of two is suggested / mandatory to work
		int factor = 3;
        int numberCellsX = pow(2, factor);
        int numberCellsY = pow(2, factor);
        int numberCellsZ = pow(2, factor);

        int cellsizeX = (gridBoundaryMaxX - gridBoundaryMinX) / numberCellsX;
        int cellsizeY = (gridBoundaryMaxY - gridBoundaryMinY) / numberCellsY;
        int cellsizeZ = (gridBoundaryMaxZ - gridBoundaryMinZ) / numberCellsZ;

		// grid structure to house the bins (each bin gives access to the elements in this bin)
		std::vector<std::vector<colliders*>> internal_grid = std::vector<std::vector<colliders*>>(numberCellsX * numberCellsY * numberCellsZ);

		void insert(colliders* obj);

		void draw_grid();

		void insertionSort(std::vector<colliders*>* data, int length);

		void relocate(colliders* test_insert, int newXmin, int newXmax, int newYmin, int newYmax, int newZmin, int newZmax);
};
#endif
