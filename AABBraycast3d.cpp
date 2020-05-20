#include "AABBraycast3d.h"
#include "iostream"
#include <algorithm>
#include "float.h"

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
bool pt3d::rayAABBintersecTest(pt3d rayOrigin, pt3d rayDirection, pt3d AABBpt1, pt3d AABBpt2)
{
	std::cout << "calling rayAABBintersecTest" << std::endl;

	std::cout << rayDirection.x << std::endl;
	std::cout << rayOrigin.x << std::endl;
	std::cout << AABBpt1.x << std::endl;
	std::cout << AABBpt2.x << std::endl;

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

	// AABBpt1 is the corner of AABB with minimal coordinates - left bottom, AABBpt2 is maximal corner
	// rayOrigin is origin of ray
	float t1 = (AABBpt1.x - rayOrigin.x)*dirfrac.x;
	float t2 = (AABBpt2.x - rayOrigin.x)*dirfrac.x;
	float t3 = (AABBpt1.y - rayOrigin.y)*dirfrac.y;
	float t4 = (AABBpt2.y - rayOrigin.y)*dirfrac.y;
	float t5 = (AABBpt1.z - rayOrigin.z)*dirfrac.z;
	float t6 = (AABBpt2.z - rayOrigin.z)*dirfrac.z;

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

	std::cout << "t: " << t << std::endl;

	return true;
}

