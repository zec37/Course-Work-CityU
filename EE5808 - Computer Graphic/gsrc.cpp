#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <GL/glut.h>

#define PI 3.14159265358979323846f

float v0[3], v1[3];
float mo[16] = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };

float clamp( float x, float a, float b );
float dot( const float *a, const float *b );
float norm( const float *a );
void vassign( float *a, float x, float y, float z );
void vassign( float *a, const float *b );
bool vequal( float *a, float *b );
void cross( float *a, const float *b, const float *c );
void normalize( float *a );


float* gsrc_getmo(){ return mo; }

void gsrc_mousebutton(int button, int state, int x, int y )
{
  vassign( v0, 2.0*x/glutGet(GLUT_WINDOW_WIDTH)-1, -2.0*y/glutGet(GLUT_WINDOW_HEIGHT)+1, 1 );
  normalize(v0);
}

void gsrc_mousemove(int x, int y)
{
  float axis[3], angle;

  vassign( v1, 2.0*x/glutGet(GLUT_WINDOW_WIDTH)-1, -2.0*y/glutGet(GLUT_WINDOW_HEIGHT)+1, 1 );
  normalize(v1);
  if( vequal(v0,v1) )
    return;
  cross(axis,v0,v1);
  normalize(axis);
  angle = acosf( clamp(dot(v0,v1),-1,1) );
  vassign( v0, v1 );

  glPushMatrix();
    glLoadIdentity();
    glRotatef( angle*180/PI, axis[0], axis[1], axis[2] );
    glMultMatrixf( mo );
    glGetFloatv( GL_MODELVIEW_MATRIX, mo );
  glPopMatrix();
  glutPostRedisplay();
}


float clamp( float x, float a, float b ){ return x<a ? a : (x<b?x:b); }
float dot( const float *a, const float *b ){ return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]; }
float norm( const float *a ){ return sqrtf(dot(a,a)); }
void vassign( float *a, float x, float y, float z ){ a[0]=x; a[1]=y; a[2]=z; }
void vassign( float *a, const float *b ){ a[0]=b[0]; a[1]=b[1]; a[2]=b[2]; }
bool vequal( float *a, float *b ){ return memcmp(a,b,3*sizeof(float))==0; }
void cross( float *a, const float *b, const float *c ){ 
  a[0] = b[1]*c[2] - c[1]*b[2]; a[1] = -b[0]*c[2] + c[0]*b[2]; a[2] = b[0]*c[1] - c[0]*b[1]; }
void normalize( float *a ){ float l = norm(a); a[0]/=l; a[1]/=l; a[2]/=l; }

