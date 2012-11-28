#include "CSCIx229.h"

int th = 0;         //  Azimuth of view angle
int ph = 0;         //  Elevation of view angle
// int zh = 0;
int fov=55;       //  Field of view (for perspective)
int light = 1;
double asp=1;     //  Aspect ratio


int projection = 1;     // 0 for ortho, 1 for perspective
int animation = 1;      // whether or not the sun animates

double dim=5.0;   //  Size of world

// Just some helpful little constants for perspective
int ORTHO = 0;
int PERSP = 1;

// constant for spotlight
int SPOTLIGHT = 1;

// for moving the movable ship
int movex = 0;
int movey = 0;
int movez = 1;

// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   10;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   16;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
int mode=0;       //  Texture mode
unsigned int texture[7]; // Texture names



static void projectify()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (projection == PERSP)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}


/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   double x = -Sin(th)*Cos(ph);
   double y =  Cos(th)*Cos(ph);
   double z =          Sin(ph);
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0,ph/180.0+0.5);
   glVertex3d(x,y,z);
}

static void ship_hexagon(double x, double y, double z, double scale) {

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   
   glScaled(scale, scale, scale);
   glTranslated(x,y,z);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

   
   // bottom
   glBegin(GL_POLYGON);

   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(0, 4, -1);
   glTexCoord2f(1,0); glVertex3f(-2, 2, -1);
   glTexCoord2f(1,1); glVertex3f(-2, -2, -1);
   glTexCoord2f(0,1); glVertex3f(0, -4, -1);

   glEnd();

   glBegin(GL_POLYGON);

   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(0, 4, -1);
   glTexCoord2f(1,1); glVertex3f(2, 2, -1);
   glTexCoord2f(1,0); glVertex3f(2, -2, -1);
   glTexCoord2f(0,1); glVertex3f(0, -4, -1);

   glEnd();

   // Top

   glBindTexture(GL_TEXTURE_2D,texture[3]);

   glBegin(GL_POLYGON);

   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(0, 4, 1);
   glTexCoord2f(1,0); glVertex3f(-2, 2, 1);
   glTexCoord2f(1,1); glVertex3f(-2, -2, 1);
   glTexCoord2f(0,1); glVertex3f(0, -4, 1);

   glEnd();

   glBegin(GL_POLYGON);

   glNormal3f(0,0,-1);
   glTexCoord2f(0,0); glVertex3f(0, 4, 1);
   glTexCoord2f(1,1); glVertex3f(2, 2, 1);
   glTexCoord2f(1,0); glVertex3f(2, -2, 1);
   glTexCoord2f(0,1); glVertex3f(0, -4, 1);

   glEnd();

   // "top left" side
   glBegin(GL_POLYGON);

   glNormal3f(-0.5,0,0.5);
   glTexCoord2f(0,0); glVertex3f(-3, 3, 0);
   glTexCoord2f(1,0); glVertex3f(-2, 2, 1);
   glTexCoord2f(1,1); glVertex3f(-2, -2, 1);
   glTexCoord2f(0,1); glVertex3f(-3, -3, 0);

   glEnd();

   // "top right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.5,0,0.5);
   glTexCoord2f(0,0); glVertex3f(3, 3, 0);
   glTexCoord2f(1,0); glVertex3f(2, 2, 1);
   glTexCoord2f(1,1); glVertex3f(2, -2, 1);
   glTexCoord2f(0,1); glVertex3f(3, -3, 0);

   glEnd();

   glBindTexture(GL_TEXTURE_2D,texture[1]);

   // "top front left" side

   glBegin(GL_POLYGON);

   glNormal3f(-0.25, 1, 0.5);
   glTexCoord2f(0,0); glVertex3f(-2, 2, 1);
   glTexCoord2f(1,0); glVertex3f(-3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(0, 5, 0);
   glTexCoord2f(0,1); glVertex3f(0, 4, 1);

   glEnd();

   // "top front right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.25, 1, 0.5);
   glTexCoord2f(0,0); glVertex3f(2, 2, 1);
   glTexCoord2f(1,0); glVertex3f(3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(0, 5, 0);
   glTexCoord2f(0,1); glVertex3f(0, 4, 1);

   glEnd();

   // "top back left" side

   glBegin(GL_POLYGON);

   glNormal3f(-0.25, -1, 0.5);
   glTexCoord2f(0,0); glVertex3f(-2, -2, 1);
   glTexCoord2f(1,0); glVertex3f(-3, -3, 0);
   glTexCoord2f(1,1); glVertex3f(0, -5, 0);
   glTexCoord2f(0,1); glVertex3f(0, -4, 1);

   glEnd();

   // "top back right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.25, -1, 0.5);
   glTexCoord2f(0,0); glVertex3f(2, -2, 1);
   glTexCoord2f(1,0); glVertex3f(3, -3, 0);
   glTexCoord2f(1,1); glVertex3f(0, -5, 0);
   glTexCoord2f(0,1); glVertex3f(0, -4, 1);

   glEnd();

   // "bottom left" side
   glBegin(GL_POLYGON);

   glNormal3f(-0.5,0,-0.5);
   glTexCoord2f(0,0); glVertex3f(-3, 3, 0);
   glTexCoord2f(1,0); glVertex3f(-2, 2, -1);
   glTexCoord2f(1,1); glVertex3f(-2, -2, -1);
   glTexCoord2f(0,1); glVertex3f(-3, -3, 0);

   glEnd();

   // "bottom right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.5,0,-0.5);
   glTexCoord2f(0,0); glVertex3f(3, 3, 0);
   glTexCoord2f(1,0); glVertex3f(2, 2, -1);
   glTexCoord2f(1,1); glVertex3f(2, -2, -1);
   glTexCoord2f(0,1); glVertex3f(3, -3, 0);

   glEnd();

   // "bottom front left" side

   glBegin(GL_POLYGON);

   glNormal3f(-0.25, 1, -0.5);
   glTexCoord2f(0,0); glVertex3f(-2, 2, -1);
   glTexCoord2f(1,0); glVertex3f(-3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(0, 5, 0);
   glTexCoord2f(0,1); glVertex3f(0, 4, -1);

   glEnd();

   // "bottom front right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.25, 1, -0.5);
   glTexCoord2f(0,0); glVertex3f(2, 2, -1);
   glTexCoord2f(1,0); glVertex3f(3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(0, 5, 0);
   glTexCoord2f(0,1); glVertex3f(0, 4, -1);

   glEnd();

   // "bottom back left" side

   glBegin(GL_POLYGON);

   glNormal3f(-0.25, -1, -0.5);
   glTexCoord2f(0,0); glVertex3f(-2, -2, -1);
   glTexCoord2f(1,0); glVertex3f(-3, -3, 0);
   glTexCoord2f(1,1); glVertex3f(0, -5, 0);
   glTexCoord2f(0,1); glVertex3f(0, -4, -1);

   glEnd();

   // "bottom back right" side

   glBegin(GL_POLYGON);

   glNormal3f(0.25, -1, -0.5);
   glTexCoord2f(0,0); glVertex3f(2, -2, -1);
   glTexCoord2f(1,0); glVertex3f(3, -3, 0);
   glTexCoord2f(1,1); glVertex3f(0, -5, 0);
   glTexCoord2f(0,1); glVertex3f(0, -4, -1);

   glEnd();

   glDisable(GL_TEXTURE_2D);


}

static void ship_tail(double x, double y, double z, double scale) {
	
	glScaled(scale, scale, scale);
	glTranslated(x,y,z);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

	// bottom
	glBegin(GL_POLYGON);
   glNormal3f(0, 1, -1);
	glTexCoord2f(0,0); glVertex3f(-3, -3, 0);
	glTexCoord2f(1,1); glVertex3f(3, -3, 0);
	glTexCoord2f(0,1); glVertex3f(0, -14, 0);
	glEnd();

	// "left" side

	glBegin(GL_POLYGON);
   glNormal3f(-0.5, 0, 0.25);
	glTexCoord2f(0,0); glVertex3f(-3, -3, 0);
	glTexCoord2f(1,1); glVertex3f(0, -14, 0);
	glTexCoord2f(0,1); glVertex3f(0, -4, 1);
	glEnd();

	// "right" side

	glBegin(GL_POLYGON);
   glNormal3f(1.4, 0, 0.25);
	glTexCoord2f(0,0); glVertex3f(3, -3, 0);
	glTexCoord2f(1,1); glVertex3f(0, -14, 0);
	glTexCoord2f(0,1); glVertex3f(0, -4, 1);
	glEnd();

   glDisable(GL_TEXTURE_2D);

}

static void ship_wings(double x, double y, double z, double scale) {
	glScaled(scale, scale, scale);
	glTranslated(x,y,z);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D,texture[1]);

	// left wing flat bit
	glBegin(GL_POLYGON);
   glNormal3f(0, 0, 1);
	glTexCoord2f(0,0); glVertex3f(-3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(-10, -3, 0);
   glTexCoord2f(0,1); glVertex3f(-8, -5, 0);
	glTexCoord2f(1,0); glVertex3f(-3, -3, 0);
   glEnd();

   glBegin(GL_POLYGON);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-11, -11, 0);
   glTexCoord2f(0,1); glVertex3f(-10, -3, 0);
   glTexCoord2f(1,1); glVertex3f(-8, -5, 0);
   glEnd();

	// right wing flat bit
   glBegin(GL_POLYGON);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(3, 3, 0);
   glTexCoord2f(1,1); glVertex3f(10, -3, 0);
   glTexCoord2f(0,1); glVertex3f(8, -5, 0);
   glTexCoord2f(1,0); glVertex3f(3, -3, 0);
   glEnd();

   glBegin(GL_POLYGON);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(11, -11, 0);
   glTexCoord2f(0,1); glVertex3f(10, -3, 0);
   glTexCoord2f(1,1); glVertex3f(8, -5, 0);
   glEnd();

   glDisable(GL_TEXTURE_2D);
}

static void ship(double x, double y, double z, double scale) {
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
	 //  Save transformation
   glPushMatrix();
   //  Offset
   glScaled(scale, scale, scale);

   glTranslated(x,y,z);

   glRotated(th,0,1,0);

   glColor3f(1, 1, 1);

   ship_hexagon(0, 0, 0,  0.2);
   ship_hexagon(0, 6, 0,  0.7);

   glPopMatrix();

   glPushMatrix();
   glScaled(scale, scale, scale);
   glTranslated(x,y,z);
   glRotated(th,0,1,0);

   ship_tail(0, 0, 0, 0.2);

   glPopMatrix();

   glPushMatrix();
   glScaled(scale, scale, scale);
   glTranslated(x,y,z);
   glRotated(th,0,1,0);

   ship_wings(0, 0, 0, 0.2);

   glPopMatrix();


}

static void sphere(double x,double y,double z,double r, unsigned int tex)
{
   const int d=5;
   int th,ph;
   float yellow[] = {1.0,1.0,0.0,1.0};
   float Emission[]  = {0.0,0.0,0.01*emission,5};

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   if (tex) {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
      glBindTexture(GL_TEXTURE_2D,tex);
   }

   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,yellow);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   //  Latitude bands
   for (ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();

   if (tex) {
      glDisable(GL_TEXTURE_2D);
   }
}

static void glowy_ball(float Glowiness[], float Diffuse[], float Specular[],
   float Position[], float radius, int light, int mode, float direction[]) {

   sphere(Position[0],Position[1],Position[2] , radius, 0);
   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);
   //  Location of viewer for specular calculations
   glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
   //  glColor sets ambient and diffuse color materials
   glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);
   //  Enable light 0
   glEnable(light);
   //  Set ambient, diffuse, specular components and position of light 0
   if(mode == SPOTLIGHT) {
      glLightf(light,GL_SPOT_CUTOFF, 60.0);
      glLightfv(light,GL_SPOT_EXPONENT, Glowiness);
      glLightfv(light,GL_SPOT_DIRECTION, direction);
   } else {
      glLightfv(light,GL_AMBIENT, Glowiness);
   }
   glLightfv(light,GL_DIFFUSE ,Diffuse);
   glLightfv(light,GL_SPECULAR,Specular);
   glLightfv(light,GL_POSITION,Position);

}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display() 
{
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();


   //  Set view angle
   if (projection == ORTHO) {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
      glRotatef((ph+th)/2,0.5,0.5,0);
   } else {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);

      gluLookAt(Ex+movex,Ey+movey,Ez+movez, movex,movey,movez, 0,Cos(ph),0);
   }

   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   ambient = 50;
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

      //  Light switch
   if (light)
   {
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      // Draw light position as ball (still no lighting here)
      glColor3f(5,5,0);
      glowy_ball(Ambient, Diffuse, Specular, Position, 1, GL_LIGHT0, 0, NULL);
   }

   ambient = 10;
   float Ambient2[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};

   glColor3f(1,1,1);

   float Position2[] = {-3, 3, -3};
   float direction[] = {-0.4, 1, -4.5};
   glowy_ball(Ambient2, Diffuse, Specular, Position2, 0.2, GL_LIGHT1, 1, direction);

   float Position3[] = {3, 3, 4};
   float direction2[] = {0.55, 0.55, 0.55};
   glowy_ball(Ambient2, Diffuse, Specular, Position3, 0.2, GL_LIGHT2, 1, direction2);

   float Position4[] = {4, -4, -2};
   float direction3[] = {5, 0, 0};
   glowy_ball(Ambient2, Diffuse, Specular, Position4, 0.2, GL_LIGHT3, 1, direction3);

   float Position5[] = {0, -3, -1};
   float direction4[] = {movex, movey, movez};
   glowy_ball(Ambient2, Diffuse, Specular, Position5, 0.2, GL_LIGHT4, 1, direction4);

   // Draw some dumb spheres to be the
   // background

   // draw the planet
   glEnable(GL_TEXTURE_2D);
   glColor3f(0, 0.671, 100);
   sphere(-0.4,1,-4.5 , 2.5, texture[0]);

   glDisable(GL_TEXTURE_2D);

   // draw the moon
   glColor3f(0.55, 0.55, 0.55);
   sphere(0.8, 0.6, -1, 1, texture[2]);

   // ***** THE EXCITING PART *********
   ship(movex, movey, movez, 1);
   ship(5, 0, 0, 0.5);

   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
   //glDisable(GL_LIGHTING);
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   if (animation == 1) {
      //  Elapsed time in seconds
      double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
      zh = fmod(90*t,360.0);
      //  Tell GLUT it is necessary to redisplay the scene
      glutPostRedisplay();
   }
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 4;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 4;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 4;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 4;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   projectify();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27 || ch == 'q' || ch == 'Q')
      exit(0);
   //  Reset view angle
   else if (ch == '0') {
   	th = ph = 0;
   	//zh = 45;
   }
   else if (ch == '>') {
  		th += 2;
      ph += 2;
   }
   else if (ch == '<') {
  		th -= 2;
      ph -=2;
   }
   else if (ch == 'p') {
      if (projection == ORTHO) 
         projection = PERSP;
      else
         projection = ORTHO;
   }
   else if (ch == 'a') {
      movex--;
   }
   else if (ch == 's') {
      movey--;
   }
   else if (ch == 'd') {
      movex++;
   }
   else if (ch == 'w') {
      movey++;
   }
   else if (ch == 'u') {
      movez++;
   }
   else if (ch == 'v') {
      movez--;
   }
   else if (ch == 'l') {
      // toggle the sun
      if (light == 0) {
         light = 1;
      } else {
         light = 0;
      }
   }
   else if (ch == 32) {
      if (animation == 0) {
         animation = 1;
      } else {
         animation = 0;
      }
   }

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;

   //  Tell GLUT it is necessary to redisplay the scene
   projectify();
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   const double dim=2.5;
   //  Ratio of the width to the height of the window
   double w2h = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   glOrtho(-w2h*dim,+w2h*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
   projectify();
}


int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Space Ships! Yay!");
   //  Tell GLUT to call "display" when the scene should be drawn
   glutDisplayFunc(display);
   //  Tell GLUT to call "reshape" when the window is resized
   glutReshapeFunc(reshape);
   //  Tell GLUT to call "special" when an arrow key is pressed
   glutSpecialFunc(special);
   //  Tell GLUT to call "key" when a key is pressed
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   texture[0] = LoadTexBMP("znorl.bmp");
   texture[1] = LoadTexBMP("metal.bmp");
   texture[2] = LoadTexBMP("falarn.bmp");
   texture[3] = LoadTexBMP("stripemetal.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}