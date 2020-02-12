#include <vector>
#include <iostream>
#include <GL/glut.h>
#include <GL/gl.h>

#include "grid_SaP.h"

// takes one element and adds it to the corresponding bins
void grid::insert(colliders* obj)
{
	int cell_xMin = (obj->xmin - gridBoundaryMinX) / cellsizeX;
	int cell_yMin = (obj->ymin - gridBoundaryMinY) / cellsizeY;
	int cell_zMin = (obj->zmin - gridBoundaryMinZ) / cellsizeZ;

	int cell_xMax = (obj->xmax - gridBoundaryMinX) / cellsizeX;
	int cell_yMax = (obj->ymax - gridBoundaryMinY) / cellsizeY;
	int cell_zMax = (obj->zmax - gridBoundaryMinZ) / cellsizeZ;

	for (int x = cell_xMin; x <= cell_xMax; x++)
	{
		for (int y = cell_yMin; y <= cell_yMax; y++)
		{
			for (int z = cell_zMin; z <= cell_zMax; z++)
			{
				// add element to the corresponding bin
				internal_grid[x + y*numberCellsX + z*numberCellsX*numberCellsZ].push_back(obj);
			}
		}
	}
}

// draw the grid or parts of the grid (the bins)
void grid::draw_grid()
{
	glLineWidth(1.5);

    for (int j = numberCellsX*0.5; j < (numberCellsX*0.5)+1; j++)	// only draw cetain amount of planes (for clarity)
	{
		for (int i = 0; i < numberCellsX+1; i++)
		{
			for (int k = 0; k < numberCellsY+1; k++)
			{
				float z_elevation = gridBoundaryMinZ + j*cellsizeZ;

				glBegin(GL_LINES);
					glColor4f(1.0f, 0.0f, 0.0f, 0.5f);     // red
					glVertex3f(gridBoundaryMinX + i*cellsizeX, gridBoundaryMinY, z_elevation);
					glVertex3f(gridBoundaryMinX + i*cellsizeX, gridBoundaryMinY + k*cellsizeY, z_elevation);

					glColor4f(0.0f, 1.0f, 0.0f, 0.5f);     // green
					glVertex3f(gridBoundaryMinX + i*cellsizeX, gridBoundaryMinY + k*cellsizeY, z_elevation);
					glVertex3f(gridBoundaryMinX, gridBoundaryMinY + k*cellsizeY, z_elevation);
				glEnd();
			}
		}
	}

	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);	// black
}

// relocate an element, i.e., remove it from the old bins and add it to the new bin post movement
void grid::relocate(colliders* test_insert, int newXmin, int newXmax, int newYmin, int newYmax, int newZmin, int newZmax)
{
	int cell_xMin = (test_insert->xmin - gridBoundaryMinX) / cellsizeX;
	int cell_yMin = (test_insert->ymin - gridBoundaryMinY) / cellsizeY;
	int cell_zMin = (test_insert->zmin - gridBoundaryMinZ) / cellsizeZ;

	int cell_xMax = (test_insert->xmax - gridBoundaryMinX) / cellsizeX;
	int cell_yMax = (test_insert->ymax - gridBoundaryMinY) / cellsizeY;
	int cell_zMax = (test_insert->zmax - gridBoundaryMinZ) / cellsizeZ;

	// remove element
	for (int x = cell_xMin; x <= cell_xMax; x++)
	{
		for (int y = cell_yMin; y <= cell_yMax; y++)
		{
			for (int z = cell_zMin; z <= cell_zMax; z++)
			{
				std::vector<colliders*>* removeSearch = &internal_grid[x + y*numberCellsX + z*numberCellsX*numberCellsZ];

				int i = 0;
				bool foundElement = false;

				for (; i < (int)removeSearch->size(); i++)
				{
					// element found -> remove it from the vec
					if ((*removeSearch)[i] == test_insert)
					{
						foundElement = true;
						break;
					}
				}

				if (foundElement == true)
				{
					(*removeSearch).erase((*removeSearch).begin() + i);
				}
				else
				{
					std::cout << "Unable to find element" << std::endl;
					exit(1);
				}
			}
		}
	}

	// reentry in the new cells
	for (int x = newXmin; x <= newXmax; x++)
	{
		for (int y = newYmin; y <= newYmax; y++)
		{
			for (int z = newZmin; z <= newZmax; z++)
			{
				internal_grid[x + y*numberCellsX + z*numberCellsX*numberCellsZ].push_back(test_insert);
			}
		}
	}

	// update the cell_xMin, xmMax, ... info (cells in which the AAB resides changed)
	test_insert->cell_xMin = newXmin;
	test_insert->cell_yMin = newYmin;
	test_insert->cell_zMin = newZmin;

	test_insert->cell_xMax = newXmax;
	test_insert->cell_yMax = newYmax;
	test_insert->cell_zMax = newZmax;
}

// insertion sort
void grid::insertionSort(std::vector<colliders*>* data, int length) 
{
	for (int i = 1; i < length; i++)
	{
		for (int j = i; j > 0 && (*data)[j - 1]->xmin > (*data)[j]->xmin; j--)
 		{
 			colliders* temp = (*data)[j];

 			(*data)[j] = (*data)[j - 1];
 			(*data)[j - 1] = temp;
 		}
	}
}
