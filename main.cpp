#include <GL/glut.h>
#include <GL/gl.h>

#include <iostream>
#include <string>
#include <cstring>
#include <time.h>
#include <math.h>
#include <vector>
#include <sstream>

#include "grid_SaP.h"
#include "AABBraycast3d.h"


// GLUT-functions / variables
void InitGL(void);
void Render(void);
void Resize(int width, int height);
void SpecialKey(int key, int x, int y);				// special keys (F1, F2, ... )
void keyPressed(unsigned char key, int x, int y);	// (keys a .. z, esc, ...)
void exitfunc();												// exit - callback function
// GLUT-functions / variables

// window size / position preferences
// position options
int wnd_pos_horizontal	= 750;
int wnd_pos_vertical	= 10;
// size options
float wnd_scaling		= 2.0;	// scaling factor for windows dimensions
int wnd_width			= 1280/wnd_scaling;	// window width
int wnd_height			= 720/wnd_scaling;	// window height
// window size preferences 

// mouse rotate/manipulation variables
float rotY			= 20;
float rotZ			= 50;
float deltaAngle	= 0.01;
float angle			= 0;
bool mouse_down		= false;
float cache_x		= 0;
float cache_y		= 0;
bool rot_check		= 0;
int window1;
float dx, dy;

float lookatx = 90;	// eye position x
float lookaty = 0;	// eye position y
float lookatz = 90;	// eye position z
// mouse rotate/manipulation variables

// S&P-Grid test variables
grid newgrid;
bool insert_initial = false;
bool moveElementsGrid = true;
bool drawGrid = true;

int maxAmtElements = 250;	// amount of elements
std::vector<colliders> colliders_storage(maxAmtElements);

float delta = 0.95;
// S&P-Grid test variables

// AABBraycast3d variables
// pt3d testpt1;
// AABBraycast3d variables



static bool paused = false;	// pause/unpause state variable

void drawCube(float xmin, float xmax, float ymin, float ymax, float zmin, float zmax, float lineWidth)
{
	glLineWidth(lineWidth);

	glBegin(GL_LINE_LOOP);
 		glVertex3f(xmin, ymin, zmin);
 		glVertex3f(xmax, ymin, zmin);
 		glVertex3f(xmax, ymax, zmin);
 		glVertex3f(xmin, ymax, zmin);
	glEnd();

	glBegin(GL_LINE_LOOP);
  		glVertex3f(xmin, ymin, zmax);
   		glVertex3f(xmax, ymin, zmax);
   		glVertex3f(xmax, ymax, zmax);
  		glVertex3f(xmin, ymax, zmax);
	glEnd();

	glBegin(GL_LINES);
 		glVertex3f(xmin, ymin, zmin);
  		glVertex3f(xmin, ymin, zmax);

 		glVertex3f(xmax, ymin, zmin);
 		glVertex3f(xmax, ymin, zmax);

 		glVertex3f(xmax, ymax, zmin);
 		glVertex3f(xmax, ymax, zmax);
 
		glVertex3f(xmin, ymax, zmin);
  		glVertex3f(xmin, ymax, zmax);
	glEnd();
}

void display2dText(float x, float y, int r, int g, int b, std::string print_text)
{
	glColor3f(r, g, b);
	glRasterPos2f(x, y); // (breite, höhe, tiefe)

    std::string str = print_text;
    char *writable = new char[str.size() + 1];
    std::copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0'; // don't forget the terminating 0

	for(unsigned int i = 0; i < (unsigned)strlen(writable); i++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, writable[i]);

    delete[] writable;
}

double GetTickCount(void) 
{
	struct timespec now;

	if (clock_gettime(CLOCK_MONOTONIC, &now))
	return 0;

	return now.tv_sec * 1000.0 + now.tv_nsec / 1000000.0;
}

float afps = 0.0;

std::string CalculateFrameRate()
{
	static float lastTime = 0.0f;                // This will hold the time from the last frame
	float currentTime = GetTickCount() * 0.001f;
	static float framesPerSecond = 0.0f;         // This will store our fps

	++framesPerSecond;

	if(currentTime - lastTime > 1.0f)
	{
		afps = framesPerSecond;
		lastTime = currentTime;

		char bufferf[sizeof(int)];
		sprintf (bufferf, "%i", int(afps));

		framesPerSecond = 0;
	}
	else
	{
		char bufferf[5];
		sprintf (bufferf, "%i", int(afps));
	}

	std::ostringstream return_fps_temp;
	return_fps_temp << "FPS: ";
	return_fps_temp << afps;
	std::string return_fps(return_fps_temp.str());

	return return_fps;
}
// FPS

// MOUSE (wheel)
void mouse(int button, int state, int x, int y)
{
    (void)x;
    (void)y;

   // button 3 = scroll up; button 4 = scroll down
	if ((button == 3) || (button == 4)) // It's a wheel event
	{
		if (state == GLUT_UP)
			return; // Disregard redundant GLUT_UP events

		if (button == 4) // scroll out
		{
			lookatx = lookatx * 1.1;
			lookatz = lookatz * 1.1;
		}
		else	// scroll in (button == 3)
		{
			lookatx = lookatx * 0.9;
			lookatz = lookatz * 0.9;
		}
	}
	else	// normal button event
	{
		if (button == GLUT_LEFT_BUTTON) // when the button is released
		{
			if (state == GLUT_UP)
			{
				angle += deltaAngle;
				mouse_down = false;
			}
			else	// state = GLUT_DOWN -> mouse button pressed DOWN
			{
				mouse_down = true;
				rot_check = 0;
			}
		}
   }
}
// MOUSE (wheel)

// MOUSE MOVE
void mouseMove(int x, int y)
{
	if (mouse_down == true)
	{
		if (rot_check == 0)
		{
			rot_check = 1;
			cache_x = x;
			cache_y = y;
		}
		else
		{
			rot_check = 0;
			dx = x - cache_x;
			dy = y - cache_y;

			cache_x = 0;
			cache_y = 0;

			if ((rotY < 30 && dy < 0) || (rotY > -30 && dy > 0))
				rotY -= dy * 0.3;

			rotZ += dx * 0.3;
		}
	}
}
// MOUSE MOVE

// GLUT idle function
void myIdleFunc()
{
	float rotation_speed = 0.05;
	bool rotate = false;

	if (rotate == true)
		rotZ += rotation_speed;

    glutPostRedisplay();
}
// GLUT idle function

int main(int argc, char **argv)
{
	srand (time(NULL));

	glutInit(&argc, argv);								// Initialsierung der GLUT-Bibliothek
	glutInitDisplayMode(GLUT_DOUBLE |					// doppelter Puffer
                        GLUT_DEPTH  |					// Tiefen Puffer vorhanden
                        GLUT_RGB    |					// Fpuffer mit Rot,Grün und Blau
						GLUT_MULTISAMPLE);
	glutInitWindowSize(wnd_width, wnd_height);			// windowdimentions (width, height)
	glutInitWindowPosition(wnd_pos_horizontal, wnd_pos_vertical);						// windowposition (horizontal, vertical)

	glDisable(GL_DEPTH_TEST);

	// smooth lines
	glEnable(GL_BLEND);
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_MULTISAMPLE);	// multisample antialiasing (MSAA)

	window1 = glutCreateWindow("S&P Grid Hybrid 3d");		// windowtitle

	InitGL();

	glutDisplayFunc(&Render);		// Funktion zum zeichnen des Bildes

	glutReshapeFunc(&Resize);		// Funktion die auf Größenänderungen des Fensers eingeht
	glutSpecialFunc(&SpecialKey);	// Funktion zur Behandlung von Sondertasten z.B.: Pfeiltasten
	glutKeyboardFunc(keyPressed);	// Tell GLUT to use the method "keyPressed" for key presses  
	atexit(exitfunc);

	//MOUSE
	glutMouseFunc(mouse); 			// mousewheel and buttons
	glutIdleFunc(myIdleFunc);
	//MOUSE

	// transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// transparency

    glutMainLoop();
}

// initialize OPENGL
void InitGL(void)
{
	//glClearColor(0.0, 0.0, 0.0, 0.0);		// Background color
	glClearColor(1.0, 1.0, 1.0, 0.0);		// Background color
	glEnable(GL_DEPTH_TEST); 				// Depth buffer
	glDisable(GL_CULL_FACE);				// disable backface culling
}

// Key manipulation
void SpecialKey(int key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key)
	{
		case GLUT_KEY_F1: // F1 -> exit the program
			rotY = 20;
			rotZ = 50;
			lookatx = 90;
			lookatz = 90;
			break;

		case GLUT_KEY_F12: // F12 -> reset rotation
			std::cout << "F12" << std::endl;
			break;
	}
   glutPostRedisplay();
}

void keyPressed(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    switch (key)
	{
        case 27: // Escape key
		{
			exit (0);
			break;
		}
        case 32: // Spacebar
		{
			paused = !paused;
			std::cout << " > > paused: " << paused << std::endl;

			if (moveElementsGrid == true)
			{
				moveElementsGrid = false;
			}
			else
			{
				moveElementsGrid = true;
			}

			break;
		}

		case 'g':
		{
			drawGrid = !drawGrid;
			break;
		}

	}

	glutPostRedisplay();
} 

// resize window (OPENGL)
void Resize(int width, int height)
{
	glViewport(0, 0, (GLint)width, (GLint)height); 
	wnd_width = width;
	wnd_height= height;
}
// resize window (OPENGL)

void draw_hud()
{
	// HUD
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

    float hudcolor_r = 0;
	float hudcolor_g = 0;
	float hudcolor_b = 0;
	glOrtho(0.0f, wnd_width, wnd_height, 0.0f, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    glPushMatrix();

 	float spacing_hud_text = 12;

    // FPS
	float spacing_hud_offet_y = 15;
 	std::string fetch_fps = CalculateFrameRate();
    char *cstr = &fetch_fps[0u];
 	display2dText(5, spacing_hud_offet_y, hudcolor_r, hudcolor_g, hudcolor_b, cstr);

    //rotZ
    // generate string
    std::string prefix_1 = "R_z: ";
    std::ostringstream string1;
    string1 << rotZ;
    std::string get_number(string1.str());
    prefix_1.append(get_number);
    char *cstr2 = &prefix_1[0u];
    // generate char (to pass to function display2dText)
    display2dText(5, spacing_hud_offet_y + spacing_hud_text, hudcolor_r, hudcolor_g, hudcolor_b, cstr2);

	//rotY
    // generate string
    std::string prefix_2 = "R_y: ";
    std::ostringstream string2;
    string2 << rotY;
    std::string get_number2(string2.str());
    prefix_2.append(get_number2);
    char *cstr3 = &prefix_2[0u];
    // generate char (to pass to function display2dText)
    display2dText(5, spacing_hud_offet_y + spacing_hud_text * 2, hudcolor_r, hudcolor_g, hudcolor_b, cstr3);
}


void Render(void)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(100.0, 2.0, 2.0, 500.0);
 	gluLookAt(lookatx, lookaty, lookatz, 0, 0, 0, 0, 0, 1);


// 	glScalef(mousezoom, mousezoom, mousezoom);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glRotatef(-rotY, 0.0, 1.0, 0.0);		// mouserotation (y-direction)
	glRotatef(rotZ , 0.0, 0.0, 1.0);		// mouserotation (z-direction)

	// coordinate system
	glBegin(GL_LINES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);	// red
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(2.0f,  0.0f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);	// green
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f,  2.0f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);	// blue
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f,  0.0f, 2.0f);
	glEnd();
	// coordinate system


// 	std::cout << "PRE: " << testpt1.pt3d_get_x() << std::endl;


    // S&P GRID TEST
	// test random insert
	if (insert_initial == false)
	{
		std::cout << "init structure" << std::endl;

		pt3d testpt(1.0, 2.0, 3.0);

		std::cout << std::endl;
		testpt.print_info();

		std::cout << std::endl;
		testpt.pt3d_set_x(5.001);


		std::cout << std::endl;
		testpt.print_info();


		exit(1);


// 		testpt1.pt3d_set_x(65);
// 		testpt1.pt3d_set_y(65);
// 		testpt1.pt3d_set_z(65);



		insert_initial = true;
		float rand_xmin, rand_ymin, rand_xmax, rand_ymax, rand_zmin, rand_zmax, rand_Vx, rand_Vy, rand_Vz;
		int i = 0;

		while(i < maxAmtElements)
		{
			std::cout << "gen -> " << i << " / " << maxAmtElements << std::endl;

			// populate elements via random numbers
			float tempRandMinX = newgrid.gridBoundaryMinX * delta;
			float tempRandMaxX = newgrid.gridBoundaryMaxX * delta;

			rand_xmin = tempRandMinX + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(tempRandMaxX - tempRandMinX)));

			float tempRandMinY = newgrid.gridBoundaryMinY * delta;
			float tempRandMaxY = newgrid.gridBoundaryMaxY * delta;
			rand_ymin = tempRandMinY + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(tempRandMaxY - tempRandMinY)));

			float tempRandMinZ = newgrid.gridBoundaryMinZ * delta;
			float tempRandMaxZ = newgrid.gridBoundaryMaxZ * delta;
			rand_zmin = tempRandMinZ + static_cast <float> (rand())/(static_cast <float> (RAND_MAX/(tempRandMaxZ - tempRandMinZ)));

			float sizeFactorAABBs = 10.0;	// scaling factor is proportional to the max size of an AABB box
			float rand_dx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(sizeFactorAABBs));
			float rand_dy = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(sizeFactorAABBs));
			float rand_dz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(sizeFactorAABBs));

			// set sizes to the same values
			/*
			rand_dx = 1;
			rand_dy = 1;
			rand_dz = 1;
			*/

			rand_xmax = rand_xmin + rand_dx;
			rand_ymax = rand_ymin + rand_dy;
			rand_zmax = rand_zmin + rand_dz;

			// velocities
			float speed_factor = 0.1;
			rand_Vx = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(speed_factor)))-(speed_factor*0.5);
			rand_Vy = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(speed_factor)))-(speed_factor*0.5);
			rand_Vz = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX/(speed_factor)))-(speed_factor*0.5);

			if (true == false)
			{
				rand_Vx = 0;
				rand_Vy = 0;
				rand_Vz = 0;
			}

			/*
			// set first element manually
			if (i == 1)
			{
				// set coordinates
				rand_xmin = -15;
				rand_xmax = 15;

				rand_ymin = -15;
				rand_ymax = 15;

				rand_zmin = -15;
				rand_zmax = 15;

				// static (dont move)
				rand_Vx  = 0;
				rand_Vy = 0;
				rand_Vz = 0;
			}
			*/

			// generate non-colliding starting elements, i.e., generate the initial postions so that they do not collide with each other!
			bool colliding_with_existing_elements = false;

            // check collision of the object against all previously generated objects
 			for (int k = 0; k < i; k++)
			{
				float genxmin2 = colliders_storage[k].xmin;
				float genxmax2 = colliders_storage[k].xmax;

				float genymin2 = colliders_storage[k].ymin;
				float genymax2 = colliders_storage[k].ymax;

				float genzmin2 = colliders_storage[k].zmin;
				float genzmax2 = colliders_storage[k].zmax;

				// AABB box-box collision detection
				if (
					(rand_xmin < genxmax2) &&
					(rand_xmax > genxmin2) &&

					(rand_ymin < genymax2) &&
					(rand_ymax > genymin2) &&

 					(rand_zmin < genzmax2) &&
 					(rand_zmax > genzmin2)
					)
				{
					colliding_with_existing_elements = true;
					break;
				}
			}

			// check collision against the "worldborder"
			if ((rand_xmin > (newgrid.gridBoundaryMaxX * delta)) or (rand_xmax > (newgrid.gridBoundaryMaxX * delta)))
            {
                colliding_with_existing_elements = true;
            }

			if ((rand_xmin < (newgrid.gridBoundaryMinX * delta)) or (rand_xmax < (newgrid.gridBoundaryMinX * delta)))
            {
                colliding_with_existing_elements = true;
            }

			if ((rand_ymin > (newgrid.gridBoundaryMaxY * delta)) or (rand_ymax > (newgrid.gridBoundaryMaxY * delta)))
            {
                colliding_with_existing_elements = true;
            }

			if ((rand_ymin < (newgrid.gridBoundaryMinY * delta)) or (rand_ymax < (newgrid.gridBoundaryMinY * delta)))
            {
                colliding_with_existing_elements = true;
            }

			if ((rand_zmin > (newgrid.gridBoundaryMaxZ * delta)) or (rand_zmax > (newgrid.gridBoundaryMaxZ * delta)))
            {
                colliding_with_existing_elements = true;
            }

			if ((rand_zmin < (newgrid.gridBoundaryMinZ * delta)) or (rand_zmax < (newgrid.gridBoundaryMinZ * delta)))
            {
                colliding_with_existing_elements = true;
            }

            // the generated object does neither collide with the world boarder nor with any previous generated object -> push the object into the storage
			if (colliding_with_existing_elements == false)
			{
				// insertion
				colliders coll_insert;
				coll_insert.xmin = rand_xmin;
				coll_insert.xmax = rand_xmax;

				coll_insert.ymin = rand_ymin;
				coll_insert.ymax = rand_ymax;

				coll_insert.zmin = rand_zmin;
				coll_insert.zmax = rand_zmax;

				// calculate in which cells the start/endpoints are residing
				coll_insert.cell_xMin = (rand_xmin - newgrid.gridBoundaryMinX) / newgrid.cellsizeX;
				coll_insert.cell_xMax = (rand_xmax - newgrid.gridBoundaryMinX) / newgrid.cellsizeX;

				coll_insert.cell_yMin = (rand_ymin - newgrid.gridBoundaryMinY) / newgrid.cellsizeY;
				coll_insert.cell_yMax = (rand_ymax - newgrid.gridBoundaryMinY) / newgrid.cellsizeY;

				coll_insert.cell_zMin = (rand_zmin - newgrid.gridBoundaryMinZ) / newgrid.cellsizeZ;
				coll_insert.cell_zMax = (rand_zmax - newgrid.gridBoundaryMinZ) / newgrid.cellsizeZ;

				// velocities
				coll_insert.vx = rand_Vx;
				coll_insert.vy = rand_Vy;
				coll_insert.vz = rand_Vz;

                // set colliding flag to false
				coll_insert.colliding = false;

				colliders_storage[i] = coll_insert;

				colliders* c_insert_gen = &colliders_storage[i];

				newgrid.insert(c_insert_gen);

				// successful insertion -> increment variable i
				i++;
			}
		}

		// perform first sort after insertion of all elements
		std::cout << "performing initial insertion sort ...";

		for (int i = 0; i < (int)newgrid.internal_grid.size(); i++)
		{
			std::vector<colliders*>* SAPcolliders = &newgrid.internal_grid[i];
	 		newgrid.insertionSort(SAPcolliders, SAPcolliders->size());
		}
		std::cout << " done" << std::endl;
    }

	// check each object against the boundary square
	if (moveElementsGrid == true)
	{
		// loop through ALL elements
		for (int i = 0; i < (int)colliders_storage.size(); i++)
		{
			colliders* test_insert = &colliders_storage[i];

			// test for collisions against the borders of the grid (just reverse the vector of movement when a collision appears)
			// X
			if ((test_insert->xmax + test_insert->vx >= (newgrid.gridBoundaryMaxX * delta)) and test_insert->vx > 0)
			{
				test_insert->vx *= -1;
			}

			if ((test_insert->xmin + test_insert->vx <= (newgrid.gridBoundaryMinX * delta)) and test_insert->vx < 0)
			{
				test_insert->vx *= -1;
			}

			// Y
			if ((test_insert->ymax + test_insert->vy >= (newgrid.gridBoundaryMaxY * delta)) and test_insert->vy > 0)
			{
				test_insert->vy *= -1;
			}

			if ((test_insert->ymin + test_insert->vy <= (newgrid.gridBoundaryMinY * delta)) and test_insert->vy < 0)
			{
				test_insert->vy *= -1;
			}

			// Z
			if ((test_insert->zmax + test_insert->vz >= (newgrid.gridBoundaryMaxZ * delta)) and test_insert->vz > 0)
			{
				test_insert->vz *= -1;
			}

			if ((test_insert->zmin + test_insert->vz <= (newgrid.gridBoundaryMinZ * delta)) and test_insert->vz < 0)
			{
				test_insert->vz *= -1;
			}

			// check, if the movement changes the cell(sub-grid) in which this element is stored
			int newXmin = ((test_insert->xmin + test_insert->vx) - newgrid.gridBoundaryMinX) / newgrid.cellsizeX;
			int newYmin = ((test_insert->ymin + test_insert->vy) - newgrid.gridBoundaryMinY) / newgrid.cellsizeY;
			int newZmin = ((test_insert->zmin + test_insert->vz) - newgrid.gridBoundaryMinZ) / newgrid.cellsizeZ;

			int newXmax = ((test_insert->xmax + test_insert->vx) - newgrid.gridBoundaryMinX) / newgrid.cellsizeX;
			int newYmax = ((test_insert->ymax + test_insert->vy) - newgrid.gridBoundaryMinY) / newgrid.cellsizeY;
			int newZmax = ((test_insert->zmax + test_insert->vz) - newgrid.gridBoundaryMinZ) / newgrid.cellsizeZ;

			// element has moved and the corresponding bins in which it resides must be adapted
			if ((newXmin != test_insert->cell_xMin) || (newXmax != test_insert->cell_xMax) || (newYmin != test_insert->cell_yMin) || (newYmax != test_insert->cell_yMax) || (newZmin != test_insert->cell_zMin) || (newZmax != test_insert->cell_zMax))
			{
				newgrid.relocate(test_insert, newXmin, newXmax, newYmin, newYmax, newZmin, newZmax);
			}

			// change(update) positions
			test_insert->xmin += test_insert->vx;
			test_insert->xmax += test_insert->vx;
			test_insert->ymin += test_insert->vy;
			test_insert->ymax += test_insert->vy;
			test_insert->zmin += test_insert->vz;
			test_insert->zmax += test_insert->vz;
		}
	}

	// insertion sort of every vec of every bin of the grid
	for (int i = 0; i < (int)newgrid.internal_grid.size(); i++)
	{
		std::vector<colliders*>* SAPcolliders = &newgrid.internal_grid[i];
 		newgrid.insertionSort(SAPcolliders, SAPcolliders->size());
	}

	// clear colliding flags
  	for (int i = 0; i < (int)colliders_storage.size(); i++)
	{
		colliders_storage[i].colliding = false;
	}

	// PERFORM SWEEP AND PRUNE FOR EACH BIN OF THE GRID
	// sweep through the bins in one axis, the other are checked via AABB box-box collision detection
	for (int k = 0; k < (int)newgrid.internal_grid.size(); k++)
	{
		// select pointer to vectors of this bin
		std::vector<colliders*>* SAPcolliders = &newgrid.internal_grid[k];

		for(int i = 0; i < ((int)SAPcolliders->size()-1); i++)
		{
			float obj1ymin = (*SAPcolliders)[i]->ymin;
			float obj1ymax = (*SAPcolliders)[i]->ymax;

 			float obj1zmin = (*SAPcolliders)[i]->zmin;
 			float obj1zmax = (*SAPcolliders)[i]->zmax;

			float max1 = (*SAPcolliders)[i]->xmax;

			for(int j = i + 1; j < (int)SAPcolliders->size() && (*SAPcolliders)[j]->xmin < max1; j++)
			{
				float obj2ymin = (*SAPcolliders)[j]->ymin;
				float obj2ymax = (*SAPcolliders)[j]->ymax;

				float obj2zmin = (*SAPcolliders)[j]->zmin;
				float obj2zmax = (*SAPcolliders)[j]->zmax;

				// check the boundary box of the other dimension (AABB box-box collision detection)
				if ((obj2ymax > obj1ymin) && (obj2ymin < obj1ymax) && (obj2zmax > obj1zmin) && (obj2zmin < obj1zmax))
				{
					(*SAPcolliders)[j]->colliding = true;
					(*SAPcolliders)[i]->colliding = true;
				}
			}
		}
	}

	// draw the bins of the grid
	if (drawGrid)
	{
		newgrid.draw_grid();
	}

	bool draw_elements = true;	// draw the elements

	int returnAmtCollisions = 0;	// how many collisions occured during each step

	for (int i = 0; i < (int)colliders_storage.size(); i++)
	{
		colliders* test_insert = &colliders_storage[i];

		if (test_insert->colliding == true)
		{
			test_insert->vx *= -1;
			test_insert->vy *= -1;
			test_insert->vz *= -1;

			returnAmtCollisions++;

			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);     // red
		}
		else
		{
 			glColor4f(0.0f, 0.0f, 0.0f, 1.0f);     // black
		}

		// draw the single element into a rect
		if (draw_elements == true)
		{
			drawCube(test_insert->xmin, test_insert->xmax, test_insert->ymin, test_insert->ymax, test_insert->zmin, test_insert->zmax, 1.0);
		}
	}

	// draw the collision boundary
	glColor4f(0.0f, 0.0f, 0.0f, 1.0f);	// black
	drawCube(newgrid.gridBoundaryMinX * delta, newgrid.gridBoundaryMaxX * delta, newgrid.gridBoundaryMinY * delta, newgrid.gridBoundaryMaxY * delta, newgrid.gridBoundaryMinY * delta, newgrid.gridBoundaryMaxY * delta,  3.0);

 	std::cout << "bins: " << (int)newgrid.internal_grid.size() << " | amount collisions: " << returnAmtCollisions << std::endl;	// amount of collisions detected
	// S&P GRID TEST

	draw_hud();	// Information on the top left of the screen (fps, dt, rotation angles, ...)
	glutMotionFunc(mouseMove);
	glutSwapBuffers();
}

void exitfunc()
{
    // close standard input, standard output, and standard error (valgrind)
    fclose( stdin );
    fclose( stdout );
    fclose( stderr ); 
}
