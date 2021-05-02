#include<windows.h>

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "assert.h"
#include "GL/glut.h"
//#include "GL/freeglut.h"
#include "imageloader.h"
#include "vec3f.h"

//-------------
#include <stdio.h>
#include <math.h>
#include <ctime>
#include "Particle.h"

using namespace std;

#pragma comment (lib,"freeglut.lib")
#pragma comment (lib,"opengl32.lib")
#pragma comment (lib,"gdi32.lib")
#pragma comment (lib,"user32.lib")
#pragma comment (lib,"glu32.lib")
#pragma comment (lib,"Winmm.lib")

float xpos,ypos,zpos,zoom,updateTime=20,mountainHeight=25;
Particle particleSystem;

void DrawParticles (void)
{
   int i;
   for (i = 1; i < particleSystem.getNumOfParticles(); i++)
   {
      glPushMatrix();
      // set color and fade value (alpha) of current particle
      glColor4f(particleSystem.getR(i), particleSystem.getG(i), particleSystem.getB(i), particleSystem.getAlpha(i));
      // move the current particle to its new position
//      glTranslatef(particleSystem.getXPos(i)+30.0f, particleSystem.getYPos(i)-5.0f, particleSystem.getZPos(i) + 60.0f);
      glTranslatef(particleSystem.getXPos(i), particleSystem.getYPos(i), particleSystem.getZPos(i));
      // scale the current particle (only used for smoke)
      glScalef(particleSystem.getScale(i), particleSystem.getScale(i), particleSystem.getScale(i));

      glDisable (GL_DEPTH_TEST);

//    glBegin (GL_QUADS);
//      glTexCoord2d (0, 0);
//      glVertex3f (-1, -1, 0);
//      glTexCoord2d (1, 0);                //set the current texture coordinates
//      glVertex3f (1, -1, 0);
//      glTexCoord2d (1, 1);
//      glVertex3f (1, 1, 0);
//      glTexCoord2d (0, 1);
//      glVertex3f (-1, 1, 0);
//      glEnd();

//      glBlendFunc (GL_ONE, GL_ONE);       //specify pixel arithmetic
      	glutSolidSphere(1.0,4,4);
//      glBegin (GL_QUADS);
//      glTexCoord2d (0, 0);
//      glVertex3f (-1, -1, 0);
//      glTexCoord2d (1, 0);
//      glVertex3f (1, -1, 0);
//      glTexCoord2d (1, 1);
//      glVertex3f (1, 1, 0);
//      glTexCoord2d (0, 1);
//      glVertex3f (-1, 1, 0);
//      glEnd();

      glEnable(GL_DEPTH_TEST);

      glPopMatrix();
   }
}
//-------------



//store a set of heights and normals at 2D locations
class Terrain
{
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date

	public:
		Terrain(int w2, int l2)
		{
			w = w2;
			l = l2;

			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}

			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}

			computedNormals = false;
		}

		~Terrain()
		{
			int i = 0;
			for( i = 0; i < l; i++)
			{
				delete[] hs[i];
			}
			delete[] hs;

			for( i = 0; i < l; i++)
			{
				delete[] normals[i];
			}
			delete[] normals;
		}

		int width()
		{
			return w;
		}

		int length()
		{
			return l;
		}

		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}

		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}

		//Computes the normals
		void computeNormals() {
			if (computedNormals) {
				return;
			}

			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}

			for(int z = 0; z < l; z++)
			{
				for(int x = 0; x < w; x++)
				{
					Vec3f sum(0.0f, 0.0f, 0.0f);

					Vec3f out;
					if (z > 0)
					{
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}

					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}

					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}

					Vec3f right;
					if (x < w - 1)
					{
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}

					if (x > 0 && z > 0)
					{
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}

					normals2[z][x] = sum;
				}
			}

			//Smooth out the normals
			const float FALLOUT_RATIO = 1.0f;
			for(int z = 0; z < l; z++)
			{
				for(int x = 0; x < w; x++)
				{
					Vec3f sum = normals2[z][x];

					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}

					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}

			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;

			computedNormals = true;
		}

		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z)
		{
			if (!computedNormals)
			{
				computeNormals();
			}
			return normals[z][x];
		}
};

//Loads a terrain from a heightmap.
Terrain* loadTerrain(const char* filename, float height)
{
	ofstream heightMap ("terrain.txt");
	cout<<"Load bmp";

	//Image* image = loadBMP("marbles.bmp");
	Image* image = loadBMP(filename);
	float max=0 ,tem=0;

	Terrain* t = new Terrain(image->width, image->height);

	for(int y = 0; y < image->height; y++)
	{
		for(int x = 0; x < image->width; x++)
		{
			unsigned char color =
				(unsigned char)image->pixels[((image->bits >= 8) ? image->bits/8 : 1)* (y * image->width + x)];

			float h = height * ((color / 255.0f) - 0.5f);

			tem = ((color / 255.0f) - 0.5f);

			if (heightMap.is_open()){
				heightMap<<"\t"<<h;
			}
			cout<<"\t"<<h;
			t->setHeight(x, y, h);

			if(h>max)
			{
				max = h;
				xpos = x;
				ypos = h;
				zpos = y;
			}

		}
		if (heightMap.is_open()){
			heightMap<<"\n";
		}
		cout<<"\n";
	}

	cout<<"\n\nMax Height:"<<max;

	delete image;

	if (heightMap.is_open())
	{
		heightMap.close();
	}

	t->computeNormals();
	return t;
}

float _angle = 60.0f;
float _top_angle=30.0f;
Terrain* _terrain;

void cleanup()
{
	delete _terrain;
}

//
void moveRight()
{
	_angle += 3.0f;
}

void moveUp()
{
	if (_top_angle < 90)
	{
		_top_angle += 3.0f;
	}
}

void moveDown()
{
	if (_top_angle > 10) {
		_top_angle -= 3.0f;
	}
}

void moveLeft() {
	_angle -= 3.0f;
}

//
void handleKeypress(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_END:
		 //Escape key
			cleanup();
			exit(0);

		case GLUT_KEY_RIGHT:
			moveRight();
			break;

		case GLUT_KEY_LEFT:
			moveLeft();
			break;
		case GLUT_KEY_UP:
			moveUp();
			break;

		case GLUT_KEY_DOWN:
			moveDown();
			break;

		case 27: exit(0);
		break;

	}
}


void initRendering()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D );
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25.0, (double)w / (double)h, 1.0, 100.0);

	// Music
	//PlaySound(TEXT("demo.wav"), NULL, SND_ASYNC);

//-------------------------------
//	glMatrixMode (GL_MODELVIEW);
//-------------------------------
}

void drawScene()
{
//-------------------------------
	glClearDepth (1);
	glClearColor (0.0,0.0,0.0,1.0);
//-------------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -15.0f);
	glRotatef(_top_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(-_angle, 0.0f, 1.0f, 0.0f);

	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);

	float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(_terrain->width() - 1) / 2,
				 0.0f,
				 -(float)(_terrain->length() - 1) / 2);

	glColor3f(0.4f, 0.5f, 0.5f);

	for(int z = 0; z < _terrain->length() - 1; z++)
	{
		// draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}

//-------------------------------
//	glTranslatef (12.0f,15.0f,-30.0f);
	glTranslatef ((xpos*0.88f),(ypos*0.8f),(zpos*0.93f));
	particleSystem.updateParticles();
	DrawParticles();
//-------------------------------


	glutSwapBuffers();
}


void update(int value)
{
//	_angle += 0.5f;
	if (_angle > 360)
	{
		_angle -= 360;
	}

	else if(_angle < -360)
	{
		_angle += 360;
	}

	//Translate Bubble to curve
	if(_angle == 360)
	{
		GLfloat midY = 240,midX = 320, y = 0,x;
		glClearColor(1.0f,1.0f,1.0f,0.0f);
		glColor3f(0.0f,0.1f,0.0f);

		glBegin(GL_POINTS);
		 for(x = -200; x<= 200; x+= 0.005f)
		 {
			 y = x*x;
			glVertex3f(midX+x,midY+y,0.0f);

		 }
		glEnd();
	}

	glutPostRedisplay();
	glutTimerFunc(updateTime, update, 0);
}

//----------------


void display (void)
{
   glClearDepth (1);
   glClearColor (0.0,0.0,0.0,1.0);
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glLoadIdentity();
   glTranslatef (0,0,-10);

   particleSystem.updateParticles();
   DrawParticles();

   glutSwapBuffers();
}


void init (void)
{
	glEnable( GL_TEXTURE_2D );
	glEnable(GL_DEPTH_TEST);

	zoom = -60.0f;
	particleSystem.createParticles();
}

//
void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (60, (GLfloat)w / (GLfloat)h, 0.0, 100.0);
	glMatrixMode (GL_MODELVIEW);
}


//----------------

int main(int argc, char *argv[])
{
	cout<<"start";
	cout<<"Enter Mountain Height(must be greater than zero):";
	cin>>mountainHeight;
	srand((unsigned int)time(0));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(950, 600);
	glutInitWindowPosition (100, 100);

	glutCreateWindow("Volcano");
	initRendering();
	init();

	_terrain = loadTerrain("<specify path to Volcano_HeightMap.bmp>", mountainHeight);

	glutDisplayFunc(drawScene);
	glutSpecialFunc(handleKeypress);
	//glutKeyboardFunc(handleKeypress);
	particleSystem.createParticles();
	glutReshapeFunc(handleResize);
	glutTimerFunc(updateTime, update, 0);

	glutMainLoop();
	return 0;
}
