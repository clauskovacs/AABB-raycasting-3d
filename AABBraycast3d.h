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
		};

		// destructor
		~pt3d(){};

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

		// ray-AABB intersection test
		static bool rayAABBintersecTest(pt3d rayOrigin, pt3d rayDirection, pt3d AABBpt1, pt3d AABBpt2, int gridSize);

		// draw a (wire)cube around the given min/max - coordinates
		static void drawBoundaryCube(pt3d endpointMin, pt3d endpointMax);

		// rotate a pt3d around the z-axis
		pt3d rotPt3dAroundZ(float rotAngle);
};

#endif
