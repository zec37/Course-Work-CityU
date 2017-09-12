#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

//////////////////////////////////////////////////////////////////
// 
// Include the header file of our rotation user-interface.
// 
#include "gsrc.h"

#define PI			3.141592654  // Prime
#define WIN_POSX	50
#define WIN_POSY	100
#define WIN_WIDTH	1000
#define WIN_HEIGHT	1000
#define ImageWidth	512
#define ImageHeight 512
#define Scale 0.5

double phi;
double t_prev;
int Iter;
double ratio;
float distance = 300;
int sign = 1;
GLfloat LightPosWC[3] = { 0.0, 0.0, 0.0 };

unsigned char wood[ImageWidth][ImageHeight][4];
unsigned char iron[ImageWidth][ImageHeight][4];
GLuint texName;

GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat green[] = { 0.0, 1.0, 0.0, 1.0 };
GLfloat yellow[] = { 1.0, 1.0, 0.0, 1.0 };
GLfloat purple[] = { 1.0, 0.0, 1.0, 1.0 };
GLfloat cyan[] = { 0.0, 1.0, 1.0, 1.0 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };

GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat specular[] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat ambient[] = { 0.01, 0.01, 0.01, 1.0 };

/* Add value to a matrix. Used for tracing the position of the lamp light. */
void addVector(GLfloat *Vector, GLfloat x, GLfloat y, GLfloat z) {
	Vector[0] += x;
	Vector[1] += y;
	Vector[2] += z;
}

///////////////////////////////////////////////////////////
////// These are the functions used for textures.	//////
/* Read texture image file. */
void readTextureImg(char *filename, unsigned char img[][ImageHeight][4]) {
	FILE *fp = fopen(filename, "rb");
	unsigned char temp;

	for (int i = 0; i < ImageWidth; i++) {
		for (int j = 0; j < ImageWidth; j++) {
			for (int k = 0; k < 3; k++) {
				fscanf(fp, "%c", &temp);
				img[i][j][k] = (unsigned char)temp;
			}
			img[i][j][3] = (unsigned char) 0;
		}
	}
	fclose(fp);
}

/* Get the texture of the table. */
void texLinear(unsigned char texID[][512][4]) {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texID);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
}

/* The function to assign a texture to a plane. */
void assignTex(GLfloat *v1, GLfloat *v2, GLfloat *v3, GLfloat *v4) {
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);	glVertex3fv(v1);
		glTexCoord2f(0.0, 0.0);	glVertex3fv(v1);
		glTexCoord2f(0.0, 0.0);	glVertex3fv(v1);
		glTexCoord2f(0.0, 0.0);	glVertex3fv(v1);
	glEnd();
}

/* The function to assign a textue to a cube. */
void texCube(GLfloat x, GLfloat y, GLfloat z, unsigned char texID[][512][4]){
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texID);

	GLfloat Vertex1[3] = { 0.5 * x, 0.5 * y, 0.5 * z };
	GLfloat Vertex2[3] = { -0.5 * x, 0.5 * y, 0.5 * z };
	GLfloat Vertex3[3] = { 0.5 * x, -0.5 * y, 0.5 * z };
	GLfloat Vertex4[3] = { 0.5 * x, 0.5 * y, -0.5 * z };
	GLfloat Vertex5[3] = { -0.5 * x, -0.5 * y, 0.5 * z };
	GLfloat Vertex6[3] = { 0.5 * x, -0.5 * y, -0.5 * z };
	GLfloat Vertex7[3] = { -0.5 * x, 0.5 * y, -0.5 * z };
	GLfloat Vertex8[3] = { -0.5 * x, -0.5 * y, -0.5 * z };

	glEnable(GL_TEXTURE_2D);
	assignTex(Vertex1, Vertex2, Vertex5, Vertex3);
	assignTex(Vertex1, Vertex2, Vertex7, Vertex4);
	assignTex(Vertex1, Vertex3, Vertex6, Vertex4);
	assignTex(Vertex8, Vertex6, Vertex4, Vertex7);
	assignTex(Vertex8, Vertex6, Vertex3, Vertex5);
	assignTex(Vertex8, Vertex7, Vertex2, Vertex5);
}

///////////////////////////////////////////////////////////
////// These are the functions used to set lights.	//////
/* Set the environment light. */
void setLight0(){
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glEnable(GL_LIGHT0);
}

/* Set the lamp light. */
void setLight1(){
	GLfloat light1Pos[] = { 0.0, 0, 0.0, 0.9 };
	GLfloat light1Spot[] = { 0.0, 1.0, 0.0 };

	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, yellow);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 20);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1Spot);
	glEnable(GL_LIGHT1);
}

/////////////////////////////////////////////////////////////////////
////// These functions are used to draw some basic opbejects. //////
/* The function to draw a cube by given position. */
void cube(GLfloat x, GLfloat y, GLfloat z){
	glPushMatrix();

	glScalef(x, y, z);
	glutSolidCube(1);

	glPopMatrix();
}

/* The function to draw an ellipse. (Including circle*/
void ellipse(GLfloat x, GLfloat y, GLfloat z){
	glPushMatrix();

	glScalef(x, y, z);
	glutSolidSphere(1, 60, 60);

	glPopMatrix();
}

/* The function to draw a trancateCone. */
void trancateCone(GLfloat baseR, GLfloat topR, GLfloat height, int option){
	glPushMatrix();

	GLUquadricObj *p1, *p2, *p3;
	glRotatef(-90.0, 1.0, 0.0, 0.0);

	p1 = gluNewQuadric();
	gluCylinder(p1, baseR, topR, height, 60, 60);

	if (option == 1 || option == 2) {
		p2 = gluNewQuadric();
		gluDisk(p2, 0, baseR, 60, 60);
	}

	if (option == 2) {
		glPushMatrix();
		glTranslatef(0, 0, height);
		p3 = gluNewQuadric();
		gluDisk(p3, 0, topR, 60, 60);
		glPopMatrix();
	}

	glPopMatrix();
}

///////////////////////////////////////////////////////////////////
////// These functions are used to draw integrated objects. //////
/* The function to draw the table in the scene. */
void table(void){
	GLfloat surfX = 600 * Scale;
	GLfloat surfY = 600 * Scale;
	GLfloat thick = 20 * Scale;
	GLfloat LegHeight = 150 * Scale;
	GLfloat LegSize = 50 * Scale;

	GLfloat transX = 0.5 * surfX - 0.5 * LegSize;
	GLfloat transY = 0.5 * surfY - 0.5 * LegSize;
	GLfloat transHeight = 0.5 * LegHeight + 0.5 * thick;

	GLfloat specularTable[] = { 0.1, 0.1, 0.1, 1.0 };

	/* Draw the table. */
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glPushMatrix();

		texCube(surfX, thick, surfY, wood);
		cube(surfX, thick, surfY);

	/* Draw the four legs. */
	glPushMatrix();
		texCube(LegSize, LegHeight, LegSize, wood);
		glTranslatef(transX, -transHeight, transY);
		cube(LegSize, LegHeight, LegSize);
	glPopMatrix();

	glPushMatrix();
		texCube(LegSize, LegHeight, LegSize, wood);
		glTranslatef(-transX, -transHeight, transY);
		cube(LegSize, LegHeight, LegSize);
	glPopMatrix();

	glPushMatrix();
		texCube(LegSize, LegHeight, LegSize, wood);
		glTranslatef(transX, -transHeight, -transY);
		cube(LegSize, LegHeight, LegSize);
	glPopMatrix();

	glPushMatrix();
		texCube(LegSize, LegHeight, LegSize, wood);
		glTranslatef(-transX, -transHeight, -transY);
		cube(LegSize, LegHeight, LegSize);
	glPopMatrix();

	glEnd();
	glPopMatrix();
}

/* The function to draw the lamp on the table. */
void lamp(void){
	GLfloat BaseR = 40 * Scale;
	GLfloat BaseH = 20 * Scale;
	GLfloat ArmR = 5 * Scale;
	GLfloat LowArmH = 185 * Scale;
	GLfloat HighArmH = 30 * Scale;
	GLfloat ShadeBaseR = 10 * Scale;
	GLfloat ShadeTopR = 20 * Scale;
	GLfloat ShadeH = 30 * Scale;
	GLfloat BallR = 5 * Scale;
	GLfloat ShadeInnerBaseR = 10 * Scale;
	GLfloat ShadeThick = 10 * Scale;

	GLfloat rotateBase = 45;
	GLfloat rotateHighArm = 90;
	GLfloat rotateShade = 45;

	glPushMatrix();

	/* Draw base of the lamp. */
	texLinear(iron);
	
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glRotatef(rotateBase, 0.0, 1.0, 0.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
		trancateCone(BaseR, BaseR, BaseH, 2);

	glDisable(GL_TEXTURE_2D);

	/* Draw columns of the lamp. */
	texLinear(iron);

		glTranslatef(0, BaseH, 0);						
		trancateCone(ArmR, ArmR, LowArmH, 2);

		glTranslatef(0, LowArmH - 5 * Scale, 0);		
		glRotatef(rotateHighArm, 0.0, 0.0, 1.0);
		trancateCone(ArmR, ArmR, HighArmH, 2);

	glDisable(GL_TEXTURE_2D);

	/* Draw the shadecone of the lamp, light bulb, and set the light. */
		glTranslatef(0, HighArmH - 0.5*ShadeBaseR, 0);		
		glRotatef(rotateShade, 0.0, 0.0, 1.0);
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
		trancateCone(ShadeBaseR, ShadeTopR, ShadeH, 1);
		glTranslatef(0, ShadeThick, 0);						
		trancateCone(ShadeInnerBaseR, ShadeTopR, ShadeH - ShadeThick, 1);

		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
		ellipse(BallR, BallR, BallR);
		glTranslatef(0, -BallR, 0);
	
	setLight1();
	glEnd();
	glPopMatrix();

	/* Calculate the postion of light in world coordinate. */
	LightPosWC[0] += cos(rotateBase / 180 * PI) * sin(rotateHighArm / 180 * PI) * (HighArmH - 0.5*ShadeBaseR) + cos(rotateBase / 180 * PI) * sin((rotateHighArm + rotateShade) / 180 * PI) * (ShadeThick - BallR);
	LightPosWC[1] += BaseH + LowArmH - 5 * Scale + cos(rotateHighArm / 180 * PI) * (HighArmH - 0.5*ShadeBaseR) + cos((rotateHighArm + rotateShade) / 180 * PI) * (ShadeThick - BallR);
	LightPosWC[2] += sin(rotateBase / 180 * PI) * sin(rotateHighArm / 180 * PI + PI) * (HighArmH - 0.5*ShadeBaseR) + sin(rotateBase / 180 * PI) * sin((rotateHighArm + rotateShade + PI) / 180 * PI) * (ShadeThick - BallR);
}

/* The function that helps to draw the fraction	by drawing 6 cubes surrounding the center. */
void surroundCube(GLfloat size){
	GLfloat sizem = 0.5 * size;
	glPushMatrix();
	glTranslatef(-size, 0, 0);
	cube(sizem, sizem, sizem);
	glTranslatef(2 * size, 0, 0);
	cube(sizem, sizem, sizem);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -size, 0);
	cube(sizem, sizem, sizem);
	glTranslatef(0, 2 * size, 0);
	cube(sizem, sizem, sizem);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, - size);
	cube(sizem, sizem, sizem);
	glTranslatef(0, 0, 2 * size);
	cube(sizem, sizem, sizem);
	glPopMatrix();
}

/* The function that helps to draw the fraction by iteration. */
void iteration(GLfloat size, GLint nIter){
	if (nIter < 0) return;

	surroundCube(size);
	
	GLfloat sizem = 0.5 * size;
	glPushMatrix();
	glTranslatef(-size, 0, 0);
	iteration(0.5 * size, nIter - 1);
	glTranslatef(2 * size, 0, 0);
	iteration(0.5 * size, nIter - 1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -size, 0);
	iteration(0.5 * size, nIter - 1);
	glTranslatef(0, 2 * size, 0);
	iteration(0.5 * size, nIter - 1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -size);
	iteration(0.5 * size, nIter - 1);
	glTranslatef(0, 0, 2 * size);
	iteration(0.5 * size, nIter - 1);
	glPopMatrix();
}

void fractal() {
	glPushMatrix();

	glTranslatef(0 * Scale, 70 * Scale, 0 * Scale);
	glRotatef(phi, 0.0 * Scale, 1.0 * Scale, 0.0 * Scale);
	cube(30 * ratio * Scale, 30 * ratio * Scale, 30 * ratio * Scale);
	iteration(30 * ratio * Scale, Iter);

	glEnd();
	glPopMatrix();
}

///////////////////////////////
////// Assist funtions. //////
/* Zoom in and out operatoin. */
void distanceCal(GLubyte key, GLint xMouse, GLint yMouse){
	int step = 10;
	if (distance < 0)	
		sign = -1;
	else 
		sign = 1;
	if (distance < 0) {
		switch (key) {
		case 'w':			distance += step;
			break;
		case 's':			distance -= step;
			break;
		}
	}
	if (distance >= 0) {
		switch (key) {
		case 's':			distance += step;
			break;
		case 'w':			distance -= step;
			break;
		}
	}
}

////////////////////////////////////////////
////// This function is for shadows. //////
/* draw the shadow of the fractal. */
void shadow(){
	GLfloat M[16];
	GLfloat enlarge = 1;
	GLfloat x = LightPosWC[0] * enlarge;
	GLfloat y = LightPosWC[1] * enlarge;
	GLfloat z = LightPosWC[2] * enlarge;

	for (int i = 0; i < 16; i++)	M[i] = 0;
	M[0] = M[5] = M[10] = 1.0;
	M[7] = -1.0 / (y);

	glPushMatrix();
	glTranslatef(x, y + 10, z);
	glMultMatrixf(M);
	glTranslatef(-x, -y, -z);

	glColor4fv(black);
	fractal();
	glPopMatrix();
}

///////////////////////////////////////////////////
////// These are the main drawing funtions. //////
/* The drawscence. */
void drawscene(void){

	//////////////////////////////////////////////////////////////////
	// 
	// Setup perspective projection and the rotation
	// 
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); // viewport is by default the display window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, distance * sign, 0, 0, 0, 0, 1, 0);
	glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface
	//
	//////////////////////////////////////////////////////////////////


	/*  Enable Z buffer method for visibility determination. */
	//  Z buffer code starts

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Z buffer code ends */

	glClearColor(0.0, 0.0, 0.0, 0.0);	// Set display-window color to white.
	glClear(GL_COLOR_BUFFER_BIT);		// Clear display window.

	//put environment light
	glPushMatrix();
	glTranslatef(0, 300, 0);
	setLight0();
	glPopMatrix();

	//put the table
	table();

	//put the lamp
	glPushMatrix();
	glTranslatef(200 * Scale, 10 * Scale, -200 * Scale);
	addVector(LightPosWC, 130 * Scale, 10 * Scale, -130 * Scale);		//Calculate light1's position in WC.
	lamp();

	glPopMatrix();

	//put the fractal cube
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cyan);
	glMaterialfv(GL_FRONT, GL_SPECULAR, yellow);
	glMaterialfv(GL_FRONT, GL_SHININESS, yellow);
	fractal();

	glDisable(GL_LIGHTING);
	shadow();
	glEnable(GL_LIGHTING);

	glutSwapBuffers();
}

/* The animation function which achieves rotation, scaling and fraction tranforming. */
void animate(void){
	double t;
	double rad;

	t = glutGet(GLUT_ELAPSED_TIME) - t_prev;
	phi = (int) t % 7200;
	phi *= 0.05;
	rad = phi * PI / 180;
	ratio = (cos(rad) + 1) / 2.0;
	Iter = (int) ceil (ratio * 3);

	glutPostRedisplay();
}

void main(int argc, char** argv){

	glutInit(&argc, argv);                                      // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);  // Set display mode.
	glutInitWindowPosition(WIN_POSX, WIN_POSY);                // Set display-window position at (WIN_POSX, WIN_POSY) 
															   // where (0, 0) is top left corner of monitor screen
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);		           // Set display-window width and height.
	glutCreateWindow("CG_Project"); // Create display window.
	
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	readTextureImg("wood.raw", wood);
	readTextureImg("iron.raw", iron);

	t_prev = glutGet(GLUT_ELAPSED_TIME);
	phi = 0;

	glutKeyboardFunc(distanceCal);
	glutIdleFunc(animate);

//////////////////////////////////////////////////////////////////
// 
// Register mouse-click and mouse-move glut callback functions
// for the rotation user-interface.
// 2
	glutMouseFunc(gsrc_mousebutton);
	glutMotionFunc(gsrc_mousemove);
	//
	//////////////////////////////////////////////////////////////////

	glutDisplayFunc(drawscene);	 // Send planar mesh to display window.
	glutMainLoop();			               // Display everything and wait.
}