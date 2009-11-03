#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "sys/time.h"
#include <stdio.h>
#include <rote.h>
#include <signal.h>
#include <X11/Xlib.h>
#include "Python.h"
#include "../more-mess/rdfl.c"
#include "modes.c"
#include "gltext.c"
#include "initsdl.c"
int wannaplay=0;
int active;
RoteTerm *t;
float plscroll;
float rot=9;

pyf(char *n)
{
    char *f=rdfl(n);
    if (f)
        PyRun_SimpleString(f);
}

 
void keyp(int ey)
{
    rote_vt_keypress(t,ey);
    printf("%c", ey);
}

void add_terminal()
{
    t= rote_vt_create(20, 80);
    rote_vt_forkpty((RoteTerm*) t, "$SHELL ./export");
}


Uint8 pressd(SDLKey k)
{
    return SDL_GetKeyState(0)[k];
}

void wm(int w,int h)
{
	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glFrustum(-1, 1, -1, 1, 1.5, 10);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0,0,-4);
	glPushMatrix();
}

void resize(int *w,int *h,int *bpp,Uint32 *video_flags,int *shrink, int *grow)
{
    xy r = parsemodes(*w,*h,"modes",0,*shrink,*grow);
    if ((r.x!=-1))
    {
	if (SDL_SetVideoMode( r.x,r.y, *bpp, *video_flags ) ) 
	{
	    *w=r.x;
	    *h=r.y;
	    glMatrixMode( GL_MODELVIEW );
	    glPopMatrix();
	    wm(*w,*h);
	}
    }
    *grow=*shrink=0;
}

	
int RunGLTest ()
{
	int bpp;
	int w = 500;
	int h = 500;
	int done = 0;
	int shrink=0;
	int grow=0;

	xy s	   	=		   parsemodes(w,h,"mode",1,0,0);
	if (s.x!=-1){w=s.x;h=s.y;};
	
	Uint32 video_flags=initsdl(w,h,&bpp);
    
	/* Set the window manager title bar */
	SDL_WM_SetCaption( "SDL GL test", "testgl" );
	SDL_EnableUNICODE(1);
	
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	
	wm(w,h);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glShadeModel(GL_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	add_terminal();
	glLineWidth(1);
	glClearColor( 0.0, 0.0, 0.0, 1.0 );
	while( !done )
	{
		GLenum gl_error;
		char* sdl_error;
		SDL_Event event;
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				
		rote_vt_update(t);
		glPushMatrix();
		glRotatef(rot,1,0,0);
		glPushMatrix();
		    glTranslatef(-2.52,1.92,0);
		    glPushMatrix();
		    activetext=(active==2);
		    if (active==2)
			glScalef(0.003,-0.003,1);
		    else
			glScalef(0.002,-0.002,1);
		    draw_terminal(t);
		    glPopMatrix();
		    glPushMatrix();
		    activetext=(active==1);
		    if (active==1)
			glScalef(0.003,-0.003,1);
		    else
			glScalef(0.002,-0.002,1);
		    glTranslatef(0,plscroll*60,0);
		    PyRun_SimpleString("dr_aw()");
		    glPopMatrix();
		glPopMatrix();
		glPopMatrix();

		SDL_GL_SwapBuffers( );
    
		gl_error = glGetError( );

		if( gl_error != GL_NO_ERROR )
			fprintf( stderr, "testgl: OpenGL error: %d\n", gl_error );

		sdl_error = SDL_GetError( );

		if( sdl_error[0] != '\0' ) 
		{
		    fprintf(stderr, "testgl: SDL error '%s'\n", sdl_error);
		    SDL_ClearError();
		}
		
		{
		}
		
		/* Check if there's a pending event. */
		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
			case SDL_KEYDOWN:
				printf("%i\n",event.key.keysym.unicode);
				int key=event.key.keysym.sym;
				if ( key==SDLK_UP )
				{
					PyRun_SimpleString("pl_up()");
					active=1;
				}
				if ( key==(SDLK_DOWN) )
				{
					PyRun_SimpleString("pl_down()");
					active=1;
				}
				if ( key==(SDLK_PAGEUP) )
				{
					PyRun_SimpleString("pl_pgup()");
					active=1;
				}
				if ( key==(SDLK_PAGEDOWN) )
				{
					PyRun_SimpleString("pl_pgdown()");
					active=1;
				}
				else
				if ( (key == SDLK_SPACE) )
				    keyp(32);
				else
				if ( (key == SDLK_q) )
				    rot++;
				else
				if ( (key == SDLK_w) )
				    rot--;
				else
				if ( (key == SDLK_e) )
				    glLineWidth(3);
				else
				if ( (key == SDLK_r) )
				    glLineWidth(4);
				else
				if ( (key == SDLK_t) )
				    glLineWidth(5);
				else
				if ( (key == SDLK_y) )
				    glLineWidth(6);
				else
				if ( (key == SDLK_F1) )
					shrink=1;
				else
				if ( (key == SDLK_F2) )
					grow=1;
				else
				if ( (key == SDLK_END) )
				{
					PyRun_SimpleString("pl_end()");
					active=1;
				}
				else
				if ( (key == SDLK_HOME) )
				{
					PyRun_SimpleString("pl_home()");
					active=1;
				}
				else
				if ( (key == SDLK_q) )
				{
					PyRun_SimpleString("wanna_play()");
					wannaplay=1;
					active=2;
				}
				else
				if ( (key == SDLK_RETURN) )
				{
					keyp(10);					
					PyRun_SimpleString("wanna_play()");
					wannaplay=1;
					active=2;
				}
				else
				if ( (key == SDLK_c) )
					done=1;
				break;
	 		case SDL_QUIT:
				done = 1;
			break;
			}
		}
		if (wannaplay)
		{
		    FILE *f;
		    if (f=fopen("/tmp/sftkoko","r"))
			fclose(f);
		    else
		    {
			wannaplay=0;
			PyRun_SimpleString("pl_ay()");
		    }
		}
		if (shrink||grow)
		    resize(&w,&h,&bpp,&video_flags,&shrink,&grow);
	}
	SDL_Quit( );
	return(0);
}

static PyObject *pprint_line(PyObject *self, PyObject* args)
{
    const char * s;
    xy lok;
    if(PyArg_ParseTuple(args, "iisi",&lok.x,&lok.y,&s,&selectedtext))
    {
	draw_line(lok.x,lok.y,s);
	return Py_BuildValue("i", 0);
    }
}

#include "stolenfromglu"

void adjustscrolling(int y)
{
	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	
	GLint viewport[4];
	viewport[0]=0;	viewport[1]=0;
	viewport[2]=4;	viewport[3]=4;	
	double a,b,c;
	gluProject(0,y,0.1,model_view, projection, viewport,&a,&b,&c);
	b-=2.0;
	if ((b>0.8)||(b<-0.8))
	    plscroll+=b*b*b;
}


static PyObject *padjustscrolling(PyObject *self, PyObject* args)
{
    int y;
    if(PyArg_ParseTuple(args, "i",&y))
    {
	adjustscrolling(y);
	return Py_BuildValue("i", 0);
    }
}


static PyObject * pplay(PyObject *self, PyObject* args)
{
    const char *s;
    if(PyArg_ParseTuple(args, "s",&s))
    {
    keyp(10);
    char *ch="echo 1> /tmp/sftkoko; mplayer \"";
    while (*ch)
	keyp(*ch++);
    while (*s)
	if(*s==34)
    		{keyp(92);keyp(34);s++;}
    	else if((*s==13)||(*s==10))
		break;
	else
    		keyp(*s++);
    keyp(34);
    char *i=";rm /tmp/sftkoko";
    while (*i)
	keyp(*i++);

    keyp(10);
    return Py_BuildValue("i", 0);
    }
}


static PyMethodDef xyzzy_methods[] = {
	{"play",pplay,	METH_VARARGS,	 "Return the meaning of everything."},
	{"printline",pprint_line,	METH_VARARGS,	 "Return the meaning of everything."},
	{"adjs",padjustscrolling,	METH_VARARGS,	 "Return the meaning of everything."},
	{NULL,		NULL}		/* sentinel */
};

int main(int argc, char *argv[])
{
	Py_Initialize();	
	PyImport_AddModule("xyzzy");
	Py_InitModule("xyzzy", xyzzy_methods);
	PyRun_SimpleString("import xyzzy");
	char *f=rdfl("yes");
	if (f)
	    PyRun_SimpleString(f);
	RunGLTest();
	Py_Exit(0);
}

