#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "plib/sg.h"

int moving[6];
int selected[6];
int sel_startX=-1;
int sel_startY;
int sel_endX;
int sel_endY;
int winw,winh;

GLfloat ctrlpoints[4][3] = {
        { -4.0, -4.0, 0.0}, { -2.0, 4.0, 0.0}, 
        {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}};
GLfloat oldctrlpoints[4][3];
float rotations[4][2];
float step[4][2];
sgMat4 rot[4];


void printmat4(sgMat4 r){
	 int j,k;
	 for (j=0;j<4;j++)
	 {
	   for (k=0;k<4;k++)
	      printf("%f " , r[j][k]);
	   printf("\n");    
	 }
	   printf("\n");    	 
}	 
	   


void quaf()
{
    float s=0.4;
    glColor3f(1,0,0);
    glVertex3f(-s, -s, -s);
    glColor3f(0,1,0);
    glVertex3f(-s, s, -s);
    glColor3f(0,0,1);
    glVertex3f(-s, -s, s);
    glColor3f(0,0,0);
    glVertex3f(-s, s, s);

    glColor3f(1,0,0);
    glVertex3f(s, -s, s);
    glColor3f(0,1,0);
    glVertex3f(s, s, s);
    glColor3f(0,0,1);
    glVertex3f(s, s, -s);
    glColor3f(0,0,0);
    glVertex3f(s, -s, -s);

    glColor3f(1,0,0);
    glVertex3f(s, -s, -s);
    glColor3f(0,1,0);
    glVertex3f(-s, -s, -s);
    glColor3f(0,0,1);
    glVertex3f(s, -s, s);
    glColor3f(0,0,0);
    glVertex3f(-s, -s, s);

    glColor3f(1,0,0);
    glVertex3f(s, s, -s);
    glColor3f(0,1,0);
    glVertex3f(-s, s, -s);
    glColor3f(0,0,1);
    glVertex3f(s, s, s);
    glColor3f(0,0,0);
    glVertex3f(-s, s, s);

    glColor3f(1,0,0);
    glVertex3f(s, -s, -s);
    glColor3f(0,1,0);
    glVertex3f(-s, -s, -s);
    glColor3f(0,0,1);
    glVertex3f(s, s, -s);
    glColor3f(0,0,0);
    glVertex3f(-s, s, -s);

    glColor3f(1,0,0);
    glVertex3f(s, -s, s);
    glColor3f(0,1,0);
    glVertex3f(-s, -s, s);
    glColor3f(0,0,1);
    glVertex3f(s, s, s);
    glColor3f(0,0,0);
    glVertex3f(-s, s, s);
}


void init(void)
{
   glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
   glEnable(GL_MAP1_VERTEX_3);
   glShadeModel(GL_SMOOTH);
   glEnable(GL_DEPTH_TEST);
}

void proj(int w, int h)
{
   if (w <= h)
      glOrtho(-5.0, 5.0, -5.0*(GLfloat)h/(GLfloat)w, 
               5.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
   else
      glOrtho(-5.0*(GLfloat)w/(GLfloat)h, 
               5.0*(GLfloat)w/(GLfloat)h, -5.0, 5.0, -5.0, 5.0);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   proj(w,h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   winw=w;
   winh=h;
}

void scene()
{
   int i;
   glInitNames();
   glPushName(1);

   glColor3f(selected[1], 1.0, 1.0);
   glBegin(GL_LINE_STRIP);
   for (i = 0; i <= 30; i++) 
         glEvalCoord1f((GLfloat) i/30.0);
   glEnd();
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   for (i = 0; i < 4; i++) 
   {
	if(isnan(rot[i][0][0]))exit(1);
         glLoadName(i+2);
	 glPushMatrix();
	 glTranslatef(ctrlpoints[i][0],ctrlpoints[i][1],0);
	 glMultMatrixf((GLfloat*)rot[i]);
	 glBegin(GL_QUADS);
	 quaf();
	 glEnd();
	 glPopMatrix();	 

   }
   glPopName();
}

void selelect()
{
    if(sel_startX==-1)
	return;
    GLuint fuf[500];
    glSelectBuffer(500, fuf);
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glMatrixMode (GL_PROJECTION);
    glRenderMode (GL_SELECT);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (sel_startX+(sel_endX-sel_startX)/2, 
      viewport[3]-(sel_startY+(sel_endY-sel_startY)/2),
      abs(sel_startX-sel_endX)+1, abs(sel_startY-sel_endY)+1, viewport);
    proj(winw,winh);
    scene();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    int i,j, k;
    for(i=0;i<6;i++)
	selected[i]=0;
    int numhits = glRenderMode(GL_RENDER);
    for(i=0,k=0;i<numhits;i++)
    {
	GLuint numnames=fuf[k++];
	k++;k++;
	for(j=0;j<numnames;j++)
	{
	    GLuint n=fuf[k];
	    if((n>0)&&(n<6))
		selected[n]=1;
	    k++;
	}
    }
}

void display(void)
{
    glClearColor(0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene();

    glColorMask(0,0,1,1);
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (sel_startX+(sel_endX-sel_startX)/2, 
      viewport[3]-(sel_startY+(sel_endY-sel_startY)/2),
      abs(sel_startX-sel_endX)+1, abs(sel_startY-sel_endY)+1, viewport);
    proj(winw,winh);
    scene();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();

    glColorMask(1,1,1,1);
    if(sel_startX!=-1)
    {
	glDisable(GL_MAP1_VERTEX_3);	
	 glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	   glLoadIdentity();
	   glOrtho(0,winw,winh,0,-5,5);
	   glBegin(GL_LINE_LOOP);
	    glColor3f(0.5,0.3,0.3);
	    glVertex2f(sel_startX, sel_startY);
	    glVertex2f(sel_endX, sel_startY);
	    glVertex2f(sel_endX, sel_endY);
            glVertex2f(sel_startX, sel_endY);
	   glEnd();
	  glPopMatrix();
	glEnable(GL_MAP1_VERTEX_3);	
   }
   glFlush();
   glutSwapBuffers();
}


int anym(void)
{
    int i;
    for(i=0;i<6;i++)
	if(moving[i])
	    return 1;
    return 0;
}

void mousemoved(int x, int y)
{
    sel_endX=x;
    sel_endY=y;
    int i;
    for(i=0;i<4;i++)
	if(moving[i+2])
	{
	    int msx=(sel_endX-sel_startX);
	    int msy=(sel_endY-sel_startY);
	    if(msx||msy)
	    {
		ctrlpoints[i][0]=oldctrlpoints[i][0]+((GLfloat)msx/(GLfloat)winw*10.0);
		ctrlpoints[i][1]=oldctrlpoints[i][1]-((GLfloat)msy/(GLfloat)winh*10.0);
		float xr=5*msx;
		float yr=-5*msy;
		step[i][0]=rotations[i][0]-xr;
		step[i][1]=rotations[i][1]-yr;
		rotations[i][0]=xr;
		rotations[i][1]=yr;
		sgVec3 axis={step[i][1],-step[i][0],0.0f};
		sgMat4 r;
		sgMakeRotMat4(r,sqrt((step[i][0]*step[i][0])+(step[i][1]*step[i][1])), axis);
		if(!i)printmat4(r);
		if(!isnan(r[0][0]))
		    sgPostMultMat4(rot[i],r);
	    }
	}
    if(anym())
      glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
	
    
    
    selelect();
    display();
}

int anys(void)
{
    int i;
    for(i=0;i<6;i++)
	if(selected[i])
	    return 1;
    return 0;
}

void moused (int button, int state, int x, int y)
{
    if((button==GLUT_LEFT_BUTTON)&(state==GLUT_DOWN))
    {
	sel_startX=x;
	sel_startY=y;
	sel_endX=x;
	sel_endY=y;
	int oldselected[6];
	memcpy(oldselected,selected,6*sizeof(int));
	selelect();
	display();
	if(anys())
	{
	    int i;
	    for(i=0;i<4;i++)
	    {
		if(oldselected[i])
		    selected[i]=1;
		rotations[i][0]=0;
		rotations[i][1]=0;
	    }
	}
	memcpy(moving,selected,6*sizeof(int));
	memcpy(oldctrlpoints, ctrlpoints,3*4*sizeof(GLfloat));
    }
}

int main(int argc, char** argv)
{
    int i;
    for(i=0;i<6;i++)
    {
	selected[i]=0;
	moving[i]=0;
    }
    for(i=0;i<4;i++)
	sgMakeIdentMat4(rot[i]);
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (200, 200);
   glutInitWindowPosition (100, 100);
   glutCreateWindow (argv[0]);
   init ();
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutMotionFunc(mousemoved);
   glutMouseFunc(moused);
   glutMainLoop();
   return 0;
}

