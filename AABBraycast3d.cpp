#include "AABBraycast3d.h"
#include "iostream"


// retrieve informations about this point
void pt3d::print_info()
{
	std::cout << "x: " << x << std::endl;
	std::cout << "y: " << y << std::endl;
	std::cout << "z: " << z << std::endl;
}

// change x/y/z - values of the point
void pt3d::pt3d_set_x(float value_x) {	this->x = value_x; }
void pt3d::pt3d_set_y(float value_y) {	this->x = value_y; }
void pt3d::pt3d_set_z(float value_z) {	this->x = value_z; }

// fetch single x/y/z - coordinates of the point
float pt3d::pt3d_get_x() {return this->x; }
float pt3d::pt3d_get_y() {return this->y; }
float pt3d::pt3d_get_z() {return this->z; }




/*

// rotates a given point around the x-axis by the angle rot_angle_x
pt3d pt3d::rotate_x(float rot_angle)
{
	pt3d return_pt;

// 	return_pt.x = 0;
// 	return_pt.y = 1;
// 	return_pt.z = 1;

	return return_pt;
}
*/




