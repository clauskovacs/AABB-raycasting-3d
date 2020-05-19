#ifndef __AABB_RAYCAST_3D__H_INCLUDED__
#define __AABB_RAYCAST_3D_INCLUDED__

// #include <math.h>

class pt3d
{
	private:
		float x;
		float y;
		float z;

	public:
		// constructor
		pt3d(float initX, float initY, float initZ)
		{
			x = initX;
			y = initY;
			z = initZ;
		}

		// retrieve informations about this point
		void print_info();

		// change x/y/z - values of the point
		void pt3d_set_x(float value_x);
		void pt3d_set_y(float value_y);
		void pt3d_set_z(float value_z);

		// fetch single x/y/z - coordinates of the point
		float pt3d_get_x();
		float pt3d_get_y();
		float pt3d_get_z();

// 		pt3d rotate_x(float rot_angle);
};

#endif
