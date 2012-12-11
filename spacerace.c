#include "CSCIx229.h"

#define MAX_SUN_PARTICLES 100000
#define MAX_ATM_PARTICLES 500000
#define MAX_SHIP_PARTICLES 10000

// sizes of maya objects
#define num_vertices_voyager (1090)
#define num_tex_voyager (1403)
#define num_normals_voyager (1129)
#define num_faces_voyager (2166)

#define num_vertices_asteroid (382)
#define num_tex_asteroid (439)
#define num_normals_asteroid (382)
#define num_faces_asteroid (760)

#define num_vertices_city (3038)
#define num_tex_city (4088)
#define num_normals_city (9464)
#define num_faces_city (5916)

#define num_vertices_renegade (1924)
#define num_tex_renegade (2188)
#define num_normals_renegade (2194)
#define num_faces_renegade (3656)

#define num_vertices_phantom (1469)
#define num_tex_phantom (1637)
#define num_normals_phantom (1670)
#define num_faces_phantom (2804)

int th = 0;         //  Azimuth of view angle
int ph = 0;         //  Elevation of view angle
float orbrotate = 0;
float orbit = 0;
int xdist = 0;
int ydist = 12;
int zdist = 0;
// int zh = 0;
int fov=55;       //  Field of view (for perspective)
int light = 1;
double asp=1;     //  Aspect ratio


int projection = 1;     // 0 for ortho, 1 for perspective
int animation = 1;      // whether or not the sun animates

double dim=10;   //  Size of world

// Just some helpful little constants for perspective
int ORTHO = 0;
int PERSP = 1;

// constant for spotlight
int SPOTLIGHT = 1;

// for moving the movable ship
int movex = 5;
int movey = 5;
int movez = 5;

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
unsigned int texture[20]; // Texture names

int ship = 0; // the current ship

// Arrays for loading Maya objects
double voyager_vertices[num_vertices_voyager][3];
double voyager_normals[num_normals_voyager][3];
double voyager_texs[num_tex_voyager][2];
int voyager_faces[num_faces_voyager][4][3];

double asteroid_1_vertices[num_vertices_asteroid][3];
double asteroid_1_normals[num_normals_asteroid][3];
double asteroid_1_texs[num_tex_asteroid][2];
int asteroid_1_faces[num_faces_asteroid][4][3];

double asteroid_2_vertices[num_vertices_asteroid][3];
double asteroid_2_normals[num_normals_asteroid][3];
double asteroid_2_texs[num_tex_asteroid][2];
int asteroid_2_faces[num_faces_asteroid][4][3];

double asteroid_3_vertices[num_vertices_asteroid][3];
double asteroid_3_normals[num_normals_asteroid][3];
double asteroid_3_texs[num_tex_asteroid][2];
int asteroid_3_faces[num_faces_asteroid][4][3];

double asteroid_city_vertices[num_vertices_asteroid][3];
double asteroid_city_normals[num_normals_asteroid][3];
double asteroid_city_texs[num_tex_asteroid][2];
int asteroid_city_faces[num_faces_asteroid][4][3];

double city_vertices[num_vertices_city][3];
double city_normals[num_normals_city][3];
double city_texs[num_tex_city][2];
int city_faces[num_faces_city][4][3];

double renegade_vertices[num_vertices_renegade][3];
double renegade_normals[num_normals_renegade][3];
double renegade_texs[num_tex_renegade][2];
int renegade_faces[num_faces_renegade][4][3];

double phantom_vertices[num_vertices_phantom][3];
double phantom_normals[num_normals_phantom][3];
double phantom_texs[num_tex_phantom][2];
int phantom_faces[num_faces_phantom][4][3];

// for particle happy fun time
float sun_slowdown = 2.0f;
float ship_slowdown = 1.0f;
float atm_slowdown = 5.0f;

float sun_xspeed = 1.0f;
float sun_yspeed = 1.0f;
float atm_xspeed = 1.0f;
float atm_yspeed = 1.0f;
float ship_xspeed = 3.0f;
float ship_yspeed = 1.0f;

unsigned int loop;
unsigned int col;
unsigned int delay;

typedef struct {
   int active;
   float life;
   float fade;

   float r;
   float g;
   float b;

   float x;
   float y;
   float z;

   float xi;
   float yi;
   float zi;

   float xg;
   float yg;
   float zg;
} particles;

// big happy fun time pile of particles
particles sun_particle[MAX_SUN_PARTICLES];
particles ship_particle[MAX_SHIP_PARTICLES];

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
   glRotatef(270, 1, 0, 0);
   // glPopMatrix();
   // glPushMatrix();
   glRotatef(orbrotate, 0, 0, 1);
   orbrotate += 0.1;
   if (orbrotate >= 360) {
      orbrotate = 0;
   }

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

static void load_obj(int num_vertices, int num_normals, int num_tex, int num_faces, char *filename, double vertices[][3], double normals[][3], double texs[][2], int faces[][4][3]) {

   FILE *file = fopen(filename, "r");

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
}

static void draw_obj(int x, int y, int z, int num_faces, double vertices[][3], double normals[][3], double texs[][2], int faces[][4][3], float scale) {
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   glPushMatrix();

   glTranslated(x,y,z);
   
   glScaled(scale, scale, scale);
   glRotatef(180, 0, 1, 0);

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

static void cube(double x,double y,double z,
                 double scale) {
   glShadeModel(GL_SMOOTH);                        // Enables Smooth Shading
   glEnable(GL_BLEND);                         // Enable Blending
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   // Type Of Blending To Perform
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);           // Really Nice Perspective Calculations
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);                 // Really Nice Point Smoothing

   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(scale, scale, scale);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[3]);
   //  Front
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
}


static void init_particles(particles particle[], int max, float slowdown, float xspeed, float yspeed, float red, float green, float blue, float life, float xgrav) {
      // initialize the particle
   for (loop=0; loop < max; loop++) {
      particle[loop].active=1;

      particle[loop].xg= 0;                     // Set Horizontal Pull To Zero
      particle[loop].yg= 0;                    // Set Vertical Pull Downward
      particle[loop].zg= xgrav;                     // Set Pull On Z Axis To Zero
      
      particle[loop].life = life;
      particle[loop].fade = (float)(rand()%100)/1000.0f+0.003f;
      particle[loop].r = red;
      particle[loop].g = green;
      particle[loop].b = blue;
      particle[loop].x = 0.0f;
      particle[loop].y = 0.0f;
      particle[loop].z = 0.0f; 
      particle[loop].xi = xspeed + (float)((rand()%60)-32.0f);
      particle[loop].yi = yspeed + (float)((rand()%60)-32.0f);
      particle[loop].zi = (float)((rand()%60)-32.0f);
   }
}

static void draw_particles(particles particle[], int max, float slowdown, float xspeed, float yspeed, int x, int y, int z,
   int r, int g, int b, float life, float scale) {

   glShadeModel(GL_SMOOTH);                        // Enables Smooth Shading
   glEnable(GL_BLEND);                         // Enable Blending
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   // Type Of Blending To Perform
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);           // Really Nice Perspective Calculations
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);                 // Really Nice Point Smoothing

   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);

   ambient = 100;
   float Glowiness[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

   glPushMatrix();
   
   glScaled(scale, scale, scale);
   glTranslated(x,y,z);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glEnable(GL_TEXTURE_2D);                        // Enable Texture Mapping
   glBindTexture(GL_TEXTURE_2D,texture[4]);                // Select Our Texture

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

   // draw that sucker
   for (loop=0; loop < max; loop++) {
      if (particle[loop].active) {
         float x=particle[loop].x;
         float y=particle[loop].y;
         float z=particle[loop].z;

         glColor4f(particle[loop].r, particle[loop].g, particle[loop].b, particle[loop].life);

         glBegin(GL_TRIANGLE_STRIP);  
         glTexCoord2d(1,1); glVertex3f(x+0.02f,y+0.02f,z); // Top Right
         glTexCoord2d(0,1); glVertex3f(x-0.02f,y+0.02f,z); // Top Left
         glTexCoord2d(1,0); glVertex3f(x+0.02f,y-0.02f,z); // Bottom Right
         glTexCoord2d(0,0); glVertex3f(x-0.02f,y-0.02f,z); // Bottom Left
         glEnd();

         particle[loop].x += particle[loop].xi / (slowdown * 1000);
         particle[loop].y += particle[loop].yi / (slowdown * 1000);
         particle[loop].z += particle[loop].zi / (slowdown * 1000);

         particle[loop].xi += particle[loop].xg;
         particle[loop].yi += particle[loop].yg;
         particle[loop].zi += particle[loop].zg;

         particle[loop].life -= particle[loop].fade;
         if (particle[loop].b < 1) {
            particle[loop].b += 0.005;
         }

         if (particle[loop].r < 1) {
            particle[loop].r += 0.005;
         }

         if (particle[loop].g < 1) {
            particle[loop].g += 0.005;
         }

         if (particle[loop].life < 0.0f) {
            particle[loop].life = life;
            particle[loop].fade = (float)(rand()%100)/1000.0f+0.003f;
            particle[loop].r = r;
            particle[loop].g = g;
            particle[loop].b = b;
            particle[loop].x = 0.0f;
            particle[loop].y = 0.0f;
            particle[loop].z = 0.0f; 
            particle[loop].xi = xspeed + (float)((rand()%60)-32.0f);
            particle[loop].yi = yspeed + (float)((rand()%60)-32.0f);
            particle[loop].zi = (float)((rand()%60)-32.0f);
         }

         float Position[]  = {x,y,z,1.0};

         glLightfv(light,GL_AMBIENT, Glowiness);
         glLightfv(light,GL_DIFFUSE ,Diffuse);
         glLightfv(light,GL_SPECULAR,Specular);
         glLightfv(light,GL_POSITION,Position);
      }
   }

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);  
   glPopMatrix();
}


static void glowy_ball(float Glowiness[], float Diffuse[], float Specular[],
   float Position[], float radius, int light, int mode, float direction[], int tex) {

   sphere(Position[0],Position[1],Position[2] , radius, tex);
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

void draw_atmosphere(float x, float y, float z, float scale, float r, float g, float b) {
   float Position[]  = {x,y,z,1};
   ambient = 50;
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

   glShadeModel(GL_SMOOTH);
   glEnable(GL_BLEND);                
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);           
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST); 
   glColor4f(r,g,b,0.2);
   glowy_ball(Ambient, Diffuse, Specular, Position, scale, GL_LIGHT0, 0, NULL, texture[4]);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);
}

void special(int key,int x,int y) {
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th -= 4;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th += 4;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph -= 4;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph += 4;
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
      movex++;
   }
   else if (ch == 's') {
      movez--;
   }
   else if (ch == 'd') {
      movex--;
   }
   else if (ch == 'w') {
      movez++;
   }
   else if (ch == 'u') {
      movey--;
   }
   else if (ch == 'v') {
      movey++;
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
   } else if (ch == '+') {
      if (dim > 5) {
         dim--;
      }
   } else if (ch == '-') {
      if (dim < 50) {
         dim++;
      }
   } else if (ch == 'm') {
      if (ship == 0) {
         ship = 1;
      } else if (ship == 1) {
         ship = 2;
      } else if (ship == 2) {
         ship = 0;
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

void display() {
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Undo previous transformations
   glLoadIdentity();

   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);

   gluLookAt(Ex+xdist,Ey+ydist,Ez+zdist, 0,0,0, 0,Cos(ph),0);
   // gluLookAt(Ex,Ey,Ez, 0,0,0, 0,Cos(ph),0);

   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Translate intensity to color vectors
   ambient = 50;
   float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
   float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
   float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};

   //draw_sky();

   //  Light position
   float Position[]  = {movex,movey,movez,1};
   // Draw light position as ball (still no lighting here)

   // draw the ship
   //int num_faces, double *vertices, double *normals, double *texs, int *faces

   if (ship == 0) {
      draw_obj(0, 0, 0, num_faces_voyager, voyager_vertices, voyager_normals, voyager_texs, voyager_faces, 0.6);
   } else if (ship == 1) {
      draw_obj(0, 0, 0, num_faces_renegade, renegade_vertices, renegade_normals, renegade_texs, renegade_faces, 0.6);
   } else if (ship == 2) {
      draw_obj(0, 0, 0, num_faces_phantom, phantom_vertices, phantom_normals, phantom_texs, phantom_faces, 0.6);
   }

   glPopMatrix();
   glPushMatrix();

   if (movex > 0) {
      glRotatef(orbit, 0, movex, 0);
   } else if (movex < 0) {
      glRotatef(orbit, 0, 0 - movex, 0);
   } else {
      glRotatef(orbit, 0, 1, 0);
   }

   orbit += 0.1;
   if (orbit >= 360.0) {
      orbit = 0.0;
   }

   glColor3f(1,1,1);
   glowy_ball(Ambient, Diffuse, Specular, Position, 6, GL_LIGHT0, 0, NULL, texture[5]);

   glColor3f(1, 1, 1);
   sphere(movex+10,movey,movez+15 , 2, texture[6]);
   sphere(movex-12,movey,movez-16 , 3, texture[8]);
   sphere(movex-3,movey,movez+19 , 3, texture[7]);
   sphere(movex+7,movey,movez-20 , 3, texture[0]);

   sphere(movex-8,movey,movez-31 , 6, texture[9]);
   sphere(movex-28,movey,movez + 30, 6, texture[10]);
   sphere(movex+34,movey,movez + 28 , 6, texture[11]);

   draw_obj(movex + 15, movey+2, movez - 24, num_faces_asteroid, asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces, 3);
   draw_obj(movex - 12, movey-2, movez + 22, num_faces_asteroid, asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces, 5);
   draw_obj(movex + 22, movey, movez + 28, num_faces_asteroid, asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces, 3);
   draw_obj(movex - 6, movey+2, movez - 22, num_faces_asteroid, asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces, 5);
   draw_obj(movex - 2, movey-2, movez + 26, num_faces_asteroid, asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces, 4);

   draw_obj(movex + 18, movey+2, movez + 3, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex - 16, movey-2, movez - 8, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 5);
   draw_obj(movex + 4, movey+2, movez + 15, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex + 18, movey-2, movez - 12, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 4);

   draw_obj(movex + 24, movey+2, movez + 15, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex - 8, movey-2, movez - 12, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 5);
   draw_obj(movex + 16, movey+2, movez + 3, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex + 9, movey-2, movez - 8, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 4);

   draw_obj(movex - 24, movey-2, movez - 15, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex + 8, movey+2, movez + 12, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 5);
   draw_obj(movex - 16, movey-2, movez - 3, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 3);
   draw_obj(movex - 9, movey+2, movez + 8, num_faces_asteroid, asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces, 4);

   draw_obj(movex - 23, movey + 3, movez + 9, num_faces_asteroid, asteroid_city_vertices, asteroid_city_normals, asteroid_city_texs, asteroid_city_faces, 10);
   draw_obj(movex - 23, movey + 3, movez + 9, num_faces_city, city_vertices, city_normals, city_texs, city_faces, 10);


   glPopMatrix();
   glPushMatrix();
   cube(Ex+xdist, Ey+ydist,  Ez+zdist, 20);

   glPopMatrix();
   glPushMatrix();

   if (movex > 0) {
      glRotatef(orbit, 0, movex, 0);
   } else if (movex < 0) {
      glRotatef(orbit, 0, 0 - movex, 0);
   } else {
      glRotatef(orbit, 0, 1, 0);
   }

   glShadeModel(GL_SMOOTH);
   glEnable(GL_BLEND);                
   glBlendFunc(GL_SRC_ALPHA,GL_ONE);                   
   glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);           
   glHint(GL_POINT_SMOOTH_HINT,GL_NICEST); 
   glColor4f(1, 1, 1,0.2);
   sphere(movex-8,movey,movez-31 , 7.5, texture[9]);
   sphere(movex-28,movey,movez+30 , 7.5, texture[10]);
   sphere(movex+34,movey,movez + 28 , 7.5, texture[11]);
   sphere(movex - 23, movey + 4.5, movez + 9, 2.2, 0);
   glDisable(GL_BLEND);
   glEnable(GL_DEPTH_TEST);

   draw_atmosphere(movex+10, movey, movez+15, 3, 1, 1, 1);
   draw_atmosphere(movex-12, movey, movez-16, 4, 1, 0.6, 0.8);
   draw_atmosphere(movex-3, movey, movez+19, 4, 0.5, 0.5, 1);
   draw_atmosphere(movex+7, movey, movez-20, 4, 0.5, 0.5, 1);
   draw_atmosphere(movex-8, movey, movez-31, 9, 1, 1, 0);
   draw_atmosphere(movex-28, movey, movez+30, 9, 0.5, 0.5, 0.5);
   draw_atmosphere(movex+34, movey, movez+28, 9, 0, 1, 1);

   draw_particles(sun_particle, MAX_SUN_PARTICLES, sun_slowdown, sun_xspeed, sun_yspeed, movex/2, movey/2, movez/2, 1, 1, 0, 1.0, 2);

   glPopMatrix();
   draw_particles(ship_particle, MAX_SHIP_PARTICLES, ship_slowdown, ship_xspeed, ship_yspeed, 0, 0, 0, 1, 1, 1, 1.0, 2);

   //  Render the scene
   glFlush();
   //  Make the rendered scene visible
   glutSwapBuffers();
}

int main(int argc,char* argv[])
{
   //  Initialize GLUT and process user parameters
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitWindowSize(800,800);
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


   // Load Textures
   texture[0] = LoadTexBMP("znorl.bmp");
   texture[1] = LoadTexBMP("metal.bmp");
   texture[2] = LoadTexBMP("turquoise.bmp");
   texture[3] = LoadTexBMP("stars.bmp");
   texture[4] = LoadTexBMP("particle.bmp");
   texture[5] = LoadTexBMP("zekador.bmp");
   texture[6] = LoadTexBMP("ishthar.bmp");
   texture[7] = LoadTexBMP("nesk.bmp");
   texture[8] = LoadTexBMP("centura.bmp");
   texture[9] = LoadTexBMP("tenav.bmp");
   texture[10] = LoadTexBMP("tsarvia.bmp");
   texture[11] = LoadTexBMP("kolatanevat.bmp");
   texture[12] = LoadTexBMP("falarn.bmp");
   texture[13] = LoadTexBMP("grass.bmp");
   texture[14] = LoadTexBMP("wood.bmp");
   texture[15] = LoadTexBMP("pine.bmp");
   texture[16] = LoadTexBMP("water.bmp");
   texture[17] = LoadTexBMP("blue.bmp");
   texture[18] = LoadTexBMP("red.bmp");
   texture[19] = LoadTexBMP("darkmetal.bmp");

   // Load Maya objects
   //int num_vertices, int num_normals, int num_tex, int num_faces, char *filename
   // double *vertices, double *normals, double *texs, int *faces
   load_obj(num_vertices_voyager, num_normals_voyager, num_tex_voyager, num_faces_voyager, "voyagereng.obj", voyager_vertices, voyager_normals, voyager_texs, voyager_faces);
   load_obj(num_vertices_asteroid, num_normals_asteroid, num_tex_asteroid, num_faces_asteroid, "asteroid1.obj", asteroid_1_vertices, asteroid_1_normals, asteroid_1_texs, asteroid_1_faces);
   load_obj(num_vertices_asteroid, num_normals_asteroid, num_tex_asteroid, num_faces_asteroid, "asteroid2.obj", asteroid_2_vertices, asteroid_2_normals, asteroid_2_texs, asteroid_2_faces);
   load_obj(num_vertices_asteroid, num_normals_asteroid, num_tex_asteroid, num_faces_asteroid, "asteroid3.obj", asteroid_3_vertices, asteroid_3_normals, asteroid_3_texs, asteroid_3_faces);
   load_obj(num_vertices_asteroid, num_normals_asteroid, num_tex_asteroid, num_faces_asteroid, "asteroid_city.obj", asteroid_city_vertices, asteroid_city_normals, asteroid_city_texs, asteroid_city_faces);
   load_obj(num_vertices_city, num_normals_city, num_tex_city, num_faces_city, "city.obj", city_vertices, city_normals, city_texs, city_faces);
   load_obj(num_vertices_renegade, num_normals_renegade, num_tex_renegade, num_faces_renegade, "renegade.obj", renegade_vertices, renegade_normals, renegade_texs, renegade_faces);
   load_obj(num_vertices_phantom, num_normals_phantom, num_tex_phantom, num_faces_phantom, "phantom.obj", phantom_vertices, phantom_normals, phantom_texs, phantom_faces);




   init_particles(sun_particle, MAX_SUN_PARTICLES, 0.01, 10, 10, 1, 1, 0, 1.0, 0);
   init_particles(ship_particle, MAX_SHIP_PARTICLES, 0.01, 10, 10, 1, 1, 1, 1.0, 10);
   //  Pass control to GLUT so it can interact with the user
   glutMainLoop();
   return 0;
}