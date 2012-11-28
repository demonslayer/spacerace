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

static void draw_obj(int num_vertices, int num_normals, int num_tex, int num_faces, char *filename) {
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   
   glScaled(0.6, 0.6, 0.6);
   glTranslated(0,0,0);

   FILE *file = fopen(filename, "r");

   double vertices[num_vertices][3];
   double normals[num_normals][3];
   double texs[num_tex][2];
   int faces[num_faces][4][3];
   int vertex = 0;
   int normal = 0;
   int tex = 0;
   int current_face = 0;

   int last_texture;

   if (file != NULL) {

      char line[128];

      while (fgets (line, sizeof line, file) != NULL) {
         if (line[0] == 'v' && line[1] == ' ') {
            char ** res = NULL;
            char * p = strtok(line, " ");
            int n_spaces = 0;

            while(p) {
               res = realloc(res, sizeof (char*) * ++n_spaces);
               if (res == NULL) 
                  exit(-1);
               res[n_spaces-1] = p;
               p = strtok(NULL, " ");
            }

            res = realloc(res, sizeof(char*) *(n_spaces+1));
            res[n_spaces] = 0;

            double x = strtod(res[1], NULL);
            double y = strtod(res[2], NULL);
            double z = strtod(res[3], NULL);

            vertices[vertex][0] = x;
            vertices[vertex][1] = y;
            vertices[vertex][2] = z;

            vertex++;
            
         }

         if (line[0] == 'f' && line[1] == ' ') {
            // do stuff with faces
            char ** res = NULL;
            char * p = strtok(line, " ");
            int n_spaces = 0;

            while(p) {
               res = realloc(res, sizeof (char*) * ++n_spaces);
               if (res == NULL) 
                  exit(-1);
               res[n_spaces-1] = p;
               p = strtok(NULL, " ");
            }

            int i;
            for (i = 1; i <= 3; i++) {
               char ** res2 = NULL;
               char * p2 = strtok(res[i], "/");
               int n_spaces2 = 0;

               while(p2) {
                  res2 = realloc(res2, sizeof (char*) * ++n_spaces2);
                  if (res2 == NULL) 
                     exit(-1);
                  res2[n_spaces2-1] = p2;
                  p2 = strtok(NULL, "/");
               }

               int vertex = atoi(res2[0]);
               int texture = atoi(res2[1]);
               int normal = atoi(res2[2]);

               faces[current_face][i-1][0] = vertex;
               faces[current_face][i-1][1] = texture;
               faces[current_face][i-1][2] = normal;
            }

            faces[current_face][3][0] = last_texture;
            current_face++;
         }

         if (line[0] == 'v' && line[1] == 'n') {
            // do stuff with normals
            char ** res = NULL;
            char * p = strtok(line, " ");
            int n_spaces = 0;

            while(p) {
               res = realloc(res, sizeof (char*) * ++n_spaces);
               if (res == NULL) 
                  exit(-1);
               res[n_spaces-1] = p;
               p = strtok(NULL, " ");
            }

            res = realloc(res, sizeof(char*) *(n_spaces+1));
            res[n_spaces] = 0;

            double x = strtod(res[1], NULL);
            double y = strtod(res[2], NULL);
            double z = strtod(res[3], NULL);

            normals[normal][0] = x;
            normals[normal][1] = y;
            normals[normal][2] = z;

            normal++;
         }

         if (line[0] == 'v' && line[1] == 't') {
            // texture coordinates
            char ** res = NULL;
            char * p = strtok(line, " ");
            int n_spaces = 0;

            while(p) {
               res = realloc(res, sizeof (char*) * ++n_spaces);
               if (res == NULL) 
                  exit(-1);
               res[n_spaces-1] = p;
               p = strtok(NULL, " ");
            }

            res = realloc(res, sizeof(char*) *(n_spaces+1));
            res[n_spaces] = 0;

            double x = strtod(res[1], NULL);
            double y = strtod(res[2], NULL);

            texs[tex][0] = x;
            texs[tex][1] = y;

            tex++;
         }

         if (line[0] == 'u') {
            //7
            last_texture = atoi(&line[7]);
         }
      }

      fclose(file);

   } else {
      perror(filename);
   }

   glColor3f(1,1,1);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);

   int i, j;
   for (i = 0; i < num_faces; i++) {
      int t = faces[i][3][0];
      glBindTexture(GL_TEXTURE_2D,texture[t]);
      glBegin(GL_POLYGON);
      for (j = 0; j < 3; j++) {

         int index = faces[i][j][0];
         int tex_index = faces[i][j][1];
         int normal_index = faces[i][j][2];
         double x = vertices[index-1][0];
         double y = vertices[index-1][1];
         double z = vertices[index-1][2];

         double tex_x = texs[tex_index-1][0];
         double tex_y = texs[tex_index-1][1];

         double normal_x = normals[normal_index-1][0];
         double normal_y = normals[normal_index-1][1];
         double normal_z = normals[normal_index-1][2];

         glVertex3f(x, y, z);
         glTexCoord2d(tex_x, tex_y);
         glNormal3d(normal_x, normal_y, normal_z);
      }
      glEnd();
   } 

   glDisable(GL_TEXTURE_2D);
   glPopMatrix();
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

void display() {
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);

   gluLookAt(Ex+movex,Ey+movey,Ez+movez, movex,movey,movez, 0,Cos(ph),0);

   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   ambient = 50;
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

   //  Light position
   float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
   // Draw light position as ball (still no lighting here)
   glColor3f(5,5,0);
   glowy_ball(Ambient, Diffuse, Specular, Position, 1, GL_LIGHT0, 0, NULL);

   //draw a cube
   //draw_obj(8, 24, 14, 6, "goddamncube.obj");
   //int num_vertices, int num_normals, int num_tex, int num_faces, char *filename
   // draw_obj(326, 365, 365, 646, "voyagertex.obj");
   draw_obj(1090, 1129, 1403, 2166, "voyagereng.obj");
   // draw_obj(8, 24, 14, 12, "cubetri.obj");
   // cube();

   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
   //glDisable(GL_LIGHTING);
}

void special(int key,int x,int y) {
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

void key(unsigned char ch,int x,int y) {
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

void reshape(int width,int height) {
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

int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(600,600);
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   //  Create the window
   glutCreateWindow("Objects");
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

   texture[0] = LoadTexBMP("znorl.bmp");
   texture[1] = LoadTexBMP("metal.bmp");
   texture[2] = LoadTexBMP("turquoise.bmp");
   texture[3] = LoadTexBMP("stripemetal.bmp");
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}