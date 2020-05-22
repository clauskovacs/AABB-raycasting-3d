#include "iostream"
#include <algorithm>
#include "float.h"
#include <vector>
#include <GL/glut.h>
#include <GL/gl.h>

#include "grid_SaP.h"
#include "AABBraycast3d.h"

// draw a (wire)cube around the given min/max - coordinates
void pt3d::drawBoundaryCube(pt3d endpointMin, pt3d endpointMax)
{
	float fetch_minX = endpointMin.pt3d_get_x();
	float fetch_minY = endpointMin.pt3d_get_y();
	float fetch_minZ = endpointMin.pt3d_get_z();

	float fetch_maxX = endpointMax.pt3d_get_x();
	float fetch_maxY = endpointMax.pt3d_get_y();
	float fetch_maxZ = endpointMax.pt3d_get_z();

	// bottom
	glBegin(GL_LINE_STRIP);
		glVertex3f(fetch_minX, fetch_minY, fetch_minZ);
		glVertex3f(fetch_maxX, fetch_minY, fetch_minZ);
		glVertex3f(fetch_maxX, fetch_maxY, fetch_minZ);
		glVertex3f(fetch_minX, fetch_maxY, fetch_minZ);
		glVertex3f(fetch_minX, fetch_minY, fetch_minZ);
	glEnd();

	// top
	glBegin(GL_LINE_STRIP);
		glVertex3f(fetch_minX, fetch_minY, fetch_maxZ);
		glVertex3f(fetch_maxX, fetch_minY, fetch_maxZ);
		glVertex3f(fetch_maxX, fetch_maxY, fetch_maxZ);
		glVertex3f(fetch_minX, fetch_maxY, fetch_maxZ);
		glVertex3f(fetch_minX, fetch_minY, fetch_maxZ);
	glEnd();

	// connection lines top <-> bottom
	glBegin(GL_LINES);
		glVertex3f(fetch_minX, fetch_minY, fetch_minZ);
		glVertex3f(fetch_minX, fetch_minY, fetch_maxZ);

		glVertex3f(fetch_minX, fetch_maxY, fetch_minZ);
		glVertex3f(fetch_minX, fetch_maxY, fetch_maxZ);

		glVertex3f(fetch_maxX, fetch_minY, fetch_minZ);
		glVertex3f(fetch_maxX, fetch_minY, fetch_maxZ);

		glVertex3f(fetch_maxX, fetch_maxY, fetch_minZ);
		glVertex3f(fetch_maxX, fetch_maxY, fetch_maxZ);
	glEnd();
}



// retrieve informations about this point
void pt3d::print_info()
{
	std::cout << "x: " << x << std::endl;
	std::cout << "y: " << y << std::endl;
	std::cout << "z: " << z << std::endl;
}

// change x/y/z - values of the point
void pt3d::pt3d_set_x(float value_x) {	this->x = value_x; }
void pt3d::pt3d_set_y(float value_y) {	this->y = value_y; }
void pt3d::pt3d_set_z(float value_z) {	this->z = value_z; }

// fetch single x/y/z - coordinates of the point
float pt3d::pt3d_get_x() {return this->x; }
float pt3d::pt3d_get_y() {return this->y; }
float pt3d::pt3d_get_z() {return this->z; }

// ray-AABB intersection test
// https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms
bool pt3d::rayAABBintersecTest(pt3d rayOrigin, pt3d rayDirection, pt3d AABBptMin, pt3d AABBptMax, int gridSize)
{
//  	std::cout << "calling rayAABBintersecTest(" << instanceCount << ")" << std::endl;

// 	if (instanceCount > 10)
// 	{
// 		return false;
// 	}

	// generate the min/max - points of the AABB-box
// 	pt3d AABBptMin(grid::gridBoundaryMinX, grid::gridBoundaryMinY, grid::gridBoundaryMinZ);
// 	pt3d AABBptMax(grid::gridBoundaryMaxX, grid::gridBoundaryMaxY, grid::gridBoundaryMaxZ);
	
	/*
	std::cout << rayDirection.x << std::endl;
	std::cout << rayOrigin.x << std::endl;
	std::cout << AABBptMin.x << std::endl;
	std::cout << AABBptMax.x << std::endl;
	*/

	// distance of the collision
	float t = 0.0;
	// rayDirection is unit direction vector of ray

	// catch zeroes in the vector
	if (rayDirection.x == 0)
		rayDirection.x = FLT_MIN;

	if (rayDirection.y == 0)
		rayDirection.y = FLT_MIN;

	if (rayDirection.z == 0)
		rayDirection.z = FLT_MIN;

	// pre-calculate the divisor
	pt3d dirfrac(1.0f / rayDirection.x, 1.0f / rayDirection.y, 1.0f / rayDirection.z);

	// AABBptMin is the corner of AABB with minimal coordinates - left bottom, AABBptMax is maximal corner
	// rayOrigin is origin of ray
	float t1 = (AABBptMin.x - rayOrigin.x)*dirfrac.x;
	float t2 = (AABBptMax.x - rayOrigin.x)*dirfrac.x;
	float t3 = (AABBptMin.y - rayOrigin.y)*dirfrac.y;
	float t4 = (AABBptMax.y - rayOrigin.y)*dirfrac.y;
	float t5 = (AABBptMin.z - rayOrigin.z)*dirfrac.z;
	float t6 = (AABBptMax.z - rayOrigin.z)*dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
	if (tmax < 0)
	{
		t = tmax;
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax)
	{
		t = tmax;
		return false;
	}

	t = tmin;

	// ray is colliding with the AABB-box -> recursively test the four "sub-cubes" against the ray
	// only proceed when the considered AABB-box is bigger than the min "sub-cube"
	int numberCellsX = pow(2, gridSize);
	int minCellsizeX = (grid::gridBoundaryMaxX - grid::gridBoundaryMinX) / numberCellsX;

	float divStart = grid::gridBoundaryMaxX - grid::gridBoundaryMinX;
	for (int i = gridSize; i >= 0 ; i--)
	{
		divStart = divStart/2.0;
// 		std::cout << "i : " << i << " // " << divStart << std::endl;
	}

 	if ((AABBptMax.x - AABBptMin.x) > minCellsizeX)
	{
		// divide the AABB-box into eight sub-boxes
		float tx = (AABBptMax.x - AABBptMin.x)*0.5;
		float ty = (AABBptMax.x - AABBptMin.x)*0.5;
		float tz = (AABBptMax.x - AABBptMin.x)*0.5;

		for (int x = AABBptMin.x ; x < AABBptMax.x; x += (AABBptMax.x - AABBptMin.x)*0.5)
		{
			for (int y = AABBptMin.y ; y < AABBptMax.y; y += (AABBptMax.y - AABBptMin.y)*0.5)
			{
				for (int z = AABBptMin.z ; z < AABBptMax.z; z += (AABBptMax.z - AABBptMin.z)*0.5)
				{
					pt3d newAABBmin(x, y, z);
					pt3d newAABBmax(x+tx, y+ty, z+tz);

					// recursively call the four sub-AABB-boxes
					pt3d::rayAABBintersecTest(rayOrigin, rayDirection, newAABBmin, newAABBmax, gridSize);
				}
			}
		}
	}

	// only detect deepest AABB-boxes
	// draw the colliding AABB-box
	if ((AABBptMax.x - AABBptMin.x) == minCellsizeX)
	{
// 		std::cout << "INSTANCE: " << (AABBptMax.x - AABBptMin.x) << " / " << minCellsizeX << std::endl;
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);	// red
		drawBoundaryCube(AABBptMin, AABBptMax);
	}

	return true;
}

