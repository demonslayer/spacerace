#include "CSCIx229.h"

#define MAX_PARTICLES 1000 // max number of particles

int rainbow=1; // is it rainbowtacular?
int sp; // is the spacebar pressed?
int rp; // is the return key pressed?

float slowdown=2.0f; // just how slow are those particles?
float xspeed; // speed in the x direction (tail!!)
float yspeed; // speed in the y direction (tail!!)
float zoom=-40.0f; // zoom away from ye olde particles

unsigned int loop; // loop variable
unsigned int col; // current color
unsigned int delay; // double rainbow omg
unsigned int texture[1]; // for the particle texture

typedef struct {
	int active; // is the particle active
	float life; // how long the particle lives
	float fade; // how fast the particle fades out

	float r; // red
	float g; // green
	float b; //blue

	float x; // x position
	float y; // y position
	float z; // z position

	float xi; // x direction
	float yi; // y direction
	float zi; // z direction

	float xg; // x gravity
	float yg; // y gravity
	float zg; // z gravity
} particles;

particles particle[MAX_PARTICLES];

// copypasta from the tutorial because I ain't retyping that shit
static GLfloat colors[12][3]=               // Rainbow Of Colors
{
    {1.0f,0.5f,0.5f},{1.0f,0.75f,0.5f},{1.0f,1.0f,0.5f},{0.75f,1.0f,0.5f},
    {0.5f,1.0f,0.5f},{0.5f,1.0f,0.75f},{0.5f,1.0f,1.0f},{0.5f,0.75f,1.0f},
    {0.5f,0.5f,1.0f},{0.75f,0.5f,1.0f},{1.0f,0.5f,1.0f},{1.0f,0.5f,0.75f}
};

void idle() {
	glutPostRedisplay();
}

static void projectify()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

void reshape(int width,int height) {
	// prevent le divide by 0
	if (height == 0) {
	  height = 1;
	}

	//  Set the viewport to the entire window
	glViewport(0,0, width,height);
	//  Tell OpenGL we want to manipulate the projection matrix
	glMatrixMode(GL_PROJECTION);
	//  Undo previous transformations
	glLoadIdentity();
	//  Orthogonal projection
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,200.0f);
	//  Switch to manipulating the model matrix
	glMatrixMode(GL_MODELVIEW);
	//  Undo previous transformations
	glLoadIdentity();
	projectify();
}

void special(int key,int x,int y) {
	// the goggles do nothing
}

void key(unsigned char ch,int x,int y) {
	// the goggles do nothing
}

void init() {
	glShadeModel(GL_SMOOTH);                        // Enables Smooth Shading
	glClearColor(0.0f,0.0f,0.0f,0.0f);                  // Black Background
	glClearDepth(1.0f);                         // Depth Buffer Setup
	glDisable(GL_DEPTH_TEST);                       // Disables Depth Testing
	glEnable(GL_BLEND);                         // Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   // Type Of Blending To Perform
	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);           // Really Nice Perspective Calculations
	glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);                 // Really Nice Point Smoothing
	glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping
	glBindTexture(GL_TEXTURE_2D,texture[0]);                // Select Our Texture

	for (loop=0;loop<MAX_PARTICLES;loop++) {
		particle[loop].active = 1;
		particle[loop].life = 1.0f;
		particle[loop].fade = (float)(rand()%100)/100.0f+0.003f;

		particle[loop].r = colors[loop*(12/MAX_PARTICLES)][0];
		particle[loop].g = colors[loop*(12/MAX_PARTICLES)][1];
		particle[loop].b = colors[loop*(12/MAX_PARTICLES)][2];

		particle[loop].xi = (float)((rand()%50)-26.0f)*10.0f;
		particle[loop].yi = (float)((rand()%50)-25.0f)*10.0f;
		particle[loop].zi = (float)((rand()%50)-25.0f)*10.0f;

		particle[loop].xg = 0.0f;
		particle[loop].yg = -0.8f;
		particle[loop].zg = 0.0f;
	}
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	for (loop = 0; loop < MAX_PARTICLES; loop++) {
		if (particle[loop].active) {
			float x = particle[loop].x;
			float y = particle[loop].y;
			float z = particle[loop].z + zoom;

			glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);
	
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2d(1,1); glVertex3f(x+0.5f,y+0.5f,z);
			glTexCoord2d(0,1); glVertex3f(x-0.5f,y+0.5f,z);
			glTexCoord2d(1,0); glVertex3f(x+0.5f,y-0.5f,z);
			glTexCoord2d(0,0); glVertex3f(x-0.5f,y-0.5f,z);
			glEnd();
		}
	}
}

int main(int argc,char* argv[]) {

	//  Initialize GLUT and process user parameters
	glutInit(&argc,argv);
	//  Request double buffered, true color window with Z buffering at 600x600
	glutInitWindowSize(1000,1000);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	//  Create the window
	glutCreateWindow("Particulator");

	texture[0] = LoadTexBMP("particle.bmp");

	init();

	//  Tell GLUT to call "idle" when there is nothing else to do
	glutIdleFunc(idle);
	//  Tell GLUT to call "display" when the scene should be drawn
	glutDisplayFunc(display);
	//  Tell GLUT to call "reshape" when the window is resized
	glutReshapeFunc(reshape);
	//  Tell GLUT to call "special" when an arrow key is pressed
	glutSpecialFunc(special);
	//  Tell GLUT to call "key" when a key is pressed
	glutKeyboardFunc(key);

	glutMainLoop();
	return 0;
}