/********************************************************************
* Description:
* Author: koom,,, <>
* Created at: Sat Jul 25 04:25:26 CEST 2009
* Computer: koom-desktop
* System: Linux 2.6.28-11-generic on i686
*
* Copyright (c) 2009 koom,,,  All rights reserved.
*
********************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/




#include <stdlib.h>

#include <stdio.h>


#include <string.h>

#include <math.h>
#include "SDL.h"
#include "SDL_events.h"
#include "sys/time.h"
#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <../roteterm/rote.h>
#include "../XY.h"
#include "modes.c"
#include "../more-mess/rdfl.c"
#include "../more-mess/resize.c"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#include "../tpl/tpl.h"
#ifdef GL
#include "SDL_opengl.h"
#include <GL/glu.h>
#ifdef nerve
#include "../toys/nerverot/stolenfromglu"
#include "../toys/nerverot/nerverot.c"
#endif
#else
#include "SDL_draw.h"
#endif

#ifdef libpng
#include "screenshot.c"
#endif
#include "initsdl.c"

#ifdef python
#include "Python.h"
#endif

#ifdef swallows3d
#include "../s3d-0.2.1.1/server/global.h"
#endif

#include <dirent.h>


#include "../roteterm/demo/sdlkeys.c"
char **buttons;
char **buttonnames;
int numbuttons;
int showbuttons=0;
char *fnfl="l2";
int do_l2=0;
int givehelp=1;
int blending=1;
char *stng;
char *mdfl;
char *fcfl;
char *pyth;
char *btns;
Uint32 lastclicktime;
int dblclick=400;

#ifdef GL
inline void dooooot(float x,float y)
{
    glVertex2f(x,y);
}
#endif


static
void
logit(const char * iFormat, ...)
{
    va_list argp;
    va_start(argp, iFormat);
    vprintf(iFormat, argp);
    va_end(argp);
    printf("\n");
}

#ifdef SDLD
#include "../sdldlines.c"
#endif

#include "../gltext.c"

#include "getexecname.c"

typedef struct
{
    RoteTerm* t;
    SDL_mutex *lock;
    SDL_Thread *thr;
}
moomoo;

typedef struct
{
    Uint32 last_resize;
    RoteTerm *t;
    int lastxresize;
    int lastyresize;
    double x,y;
    limits lim;
    void * next;
    moomoo upd_t_data;
    double scale;
    int scroll;
    int theme;
    int oldcrow, oldccol;
    double rotor;
    Uint32 lastrotor;
    int scripted;
    #ifdef python
    PyObject *showfunc;
    #endif
    char *label;
} face;

face *face1;


#include "linkedlist.c"
linkedlist(face)

#include "glterm.c"

char *ntfl;
char *newtermmsg;//
xy cam;//
int global_tabbing=0;
int selstartx, selstarty, selendx, selendy;
int clicksphase;
face * selface;
RoteTerm *clipout, *clipout2;


float floabs(float x)
{
    return x>0 ? x : -x;
}


float r1x=0;
float r1y=0;
float lv=1;

void keyp(face* f, char ey)
{
    if (f->t)
	rote_vt_keypress(f->t,ey);
}


int lines_r_dirty(RoteTerm *rt)
{
    int x;
    for (x=0;x<rt->rows;x++)
	if(rt->line_dirty[x])
	    return 1;
    return 0;
}
void lines_r_clean(RoteTerm *rt)
{
    int x;
    for (x=0;x<rt->rows;x++)
	rt->line_dirty[x]=0;
}




#define _mutexV( d ) {if(SDL_mutexV( d )) {logit("SDL_mutexV!\n");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {logit("SDL_mutexP!\n");}}

int update_terminal(void *data)
{
    while(1)
    {
      moomoo * d = (moomoo *)data;
      char buf[512];
      int br=-1;
//     logit("UNLOCKED SELECT\n");
      rote_vt_update_thready(buf, 512, &br, d->t);
//     logit("*end SELECT, locking %i*\n", d->lock);
	_mutexP(d->lock);
//        logit("LOCKED\n");

      if (br>0)
      {
	/* inject the data into the terminal */
//        logit("*locked injecting\n");
	rote_vt_inject(d->t, buf, br);
//        logit("*locked injected\n");
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=0;
	SDL_PushEvent(&e);
      }

      if(!d->t->childpid)
      {
//	logit("Segmentation Fault\n");
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=1;
	e.user.data1=d->t;
	SDL_PushEvent(&e);
	_mutexV(d->lock);
	return 777;
      }
      _mutexV(d->lock);
     }
      return 666;
}






void wm(void)
{
#ifdef GL

	glViewport( 0, 0, SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glOrtho(0,SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h,0,100,-100);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
#endif
}

SDL_Rect *SDLRect(Uint16 x,Uint16 y,Uint16 w,Uint16 h)
{
    static SDL_Rect xx ;
    xx.x=x;
    xx.y=y;
    xx.w=w;
    xx.h=h;
    return &xx;
}


void resizooo(face *f, int x, int y, Uint8* duck)
{
    if(!f->t)return;
    int up=duck[SDLK_HOME]||(y==-1);
    int dw=duck[SDLK_END]||(y==1);
    int lf=duck[SDLK_DELETE]||(x==-1);
    int ri=duck[SDLK_PAGEDOWN]||(x==1);

    if(up)
    y=-1;
    if(dw)
    y=1;
    if(lf)
    x=-1;
    if(ri)
    x=1;

    if((!x+f->lastxresize||!y+f->lastyresize)||(SDL_GetTicks()-f->last_resize>100)||!f->last_resize)
    {
	rote_vt_resize(f->t, f->t->rows+y,f->t->cols+x);
	f->last_resize=SDL_GetTicks();
    }

    f->lastxresize=x;
    f->lastyresize=y;

}

Uint32 NewTimerCallback(Uint32 interval, void *param)
{
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=0;
	SDL_PushEvent(&e);
	return interval;
}




face * new_face(void)
{
    face * f=(face*)malloc(sizeof(face));
    memset(f,0,sizeof(face));
    f->scale=0.75;
    f->theme=4;
    return f;
}

void add_terminal(face * f)
{
    logit("adding terminal|");
    RoteTerm* t;
    logit("%i\n",SDL_GetVideoSurface()->h);
    t= rote_vt_create(SDL_GetVideoSurface()->h/26/f->scale,SDL_GetVideoSurface()->w/13/f->scale);
    rote_vt_forkpty((RoteTerm*) t, "bash");
    f->t=t;
#ifdef threaded
    f->upd_t_data.lock=SDL_CreateMutex();
    f->upd_t_data.t=t;
//    logit("upd_t_data.lock=%i", f->upd_t_data.lock);
    f->upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&f->upd_t_data);
    logit("|added.\n");
#endif
}
void add_term(face * f,int c,int r)
{
    logit("adding terminal|");
    RoteTerm* t;
    t= rote_vt_create(c,r);
    rote_vt_forkpty((RoteTerm*) t, "bash");
    f->t=t;
#ifdef threaded
    f->upd_t_data.lock=SDL_CreateMutex();
    f->upd_t_data.t=t;
//    logit("upd_t_data.lock=%i", f->upd_t_data.lock);
    f->upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&f->upd_t_data);
    logit("|added.\n");
#endif
}

void cycle(face *face1, face **g)
{
    *g=(*g)->next;
    if(*g==0)
	*g=face1;
}

int term_dirty(RoteTerm *t)
{
    return t->curpos_dirty || lines_r_dirty(t);
}
int faces_dirty(face * f)
{
    while(f&&f->t)
    {
	if(term_dirty(f->t))
	    return 1;
	f=f->next;
    }
    return 0;
}

void lockterm(face * f)
{
#ifdef threaded
	_mutexP(f->upd_t_data.lock);
#endif
}
void lockterms(face * f)
{
#ifdef threaded

//    logit("locking terms\n");
    while(f&&f->t)
    {
	_mutexP(f->upd_t_data.lock);
	f=f->next;
//      logit(".\n");
    }
//    logit("done\n");
#endif
}
void unlockterms(face * f)
{
#ifdef threaded

    while(f&&f->t)
    {
	_mutexV(f->upd_t_data.lock);
	f=f->next;
    }
#endif
}

void faceclean(face *f)
{
	f->t->curpos_dirty=false;
	lines_r_clean(f->t);
}

void facesclean(face * f)
{
    while(f&&f->t)
    {
	faceclean(f);
	f=f->next;
    }
}

void showface(face *g)
{

    if(g->t)
    {
        if(g->label)
        {
	    draw_text_az(g->label, g->scale*g->t->cols/strlen(g->label),g->scale*g->t->rows);
	    
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	draw_terminal(g,selstartx,selstarty,selendx,selendy,selface);
    }
    if(g->scripted)
    {
	#ifdef python
	if(g->showfunc)
	        PyEval_CallFunction(g->showfunc, "()");
	#endif
    }	
    if(!g->t&&!g->scripted)
	draw_text(newtermmsg);
}


int countfaces(face* f)
{
    int i=0;
    while(f)
    {
	i++;
	f=f->next;
    }
    return i;
}

void focusrect(face * activeface)
{
#ifdef GL

    if(activeface->t)
    {
    glBegin(GL_LINES);
    glColor3f(1,0,0);
    glVertex2f(activeface->x, activeface->y);
    glVertex2f(activeface->x+activeface->scale*13*activeface->t->cols, activeface->y);
    glVertex2f(activeface->x+activeface->scale*13*activeface->t->cols, activeface->y);
    glVertex2f(activeface->x+activeface->scale*13*activeface->t->cols, activeface->y+activeface->scale*26*activeface->t->rows);
    glVertex2f(activeface->x+activeface->scale*13*activeface->t->cols, activeface->y+activeface->scale*26*activeface->t->rows);
    glVertex2f(activeface->x, activeface->y+activeface->scale*26*activeface->t->rows);
    glVertex2f(activeface->x, activeface->y+activeface->scale*26*activeface->t->rows);
    glVertex2f(activeface->x, activeface->y);
    glEnd();
    }
#endif
}
 

void focusline(face * activeface)
{
    static double angel=0;
    float csize=30;
#ifdef GL

    glBegin(GL_POINTS);
    glColor3f(1,0,0);
//    glVertex2i(0,0);
    glVertex2f(csize*sin(angel)+activeface->x,csize*cos(angel)+activeface->y);
//    glVertex2i(0,0);
    glVertex2f(csize*sin(angel+3.14159265358979323846264/2)+activeface->x,csize*cos(angel+3.14159265358979323846264/2)+activeface->y);
//    glVertex2i(0,0);
    glVertex2f(csize*sin(angel+3.14159265358979323846264)+activeface->x,csize*cos(angel+3.14159265358979323846264)+activeface->y);
//    glVertex2i(0,0);
    glVertex2f(csize*sin(angel+1.5*3.14159265358979323846264)+activeface->x,csize*cos(angel+1.5*3.14159265358979323846264)+activeface->y);
   angel+=0.1;
    if(angel>2*3.14159265358979323846264)angel=0;
    glEnd();
#else
    DrawLine(gltextsdlsurface,cam.x+activeface->x,cam.y+activeface->y,    cam.x+activeface->x+100,cam.y+activeface->y,barvicka);
    DrawLine(gltextsdlsurface,cam.x+activeface->x,cam.y+activeface->y+100,cam.x+activeface->x,    cam.y+activeface->y,barvicka);
#endif

}

face *getprevface(face* f1,face* f)
{
    face *prev, *cyc;
    cyc=f1;
    prev=0;
    while(cyc)
    {
	if(cyc==f)
	    return prev;
	prev=cyc;
	cyc=cyc->next;
    }
    return 0;
}



face* removeface(face *face1, face *f)
{
    if(f->t)
    {
	rote_vt_destroy(f->t);
#ifdef threaded
	SDL_DestroyMutex(f->upd_t_data.lock);
	SDL_KillThread(f->upd_t_data.thr);
#endif
    }
    face * ret= facelistremove(face1,f);
    free(f);
    if(f==selface)selface=0;
    return ret;
}

face * RemoveTerm(face* f1, RoteTerm * Term)
{
    face *next=f1;
    while(next)
    {
	if((next)->t==Term)
	{
	    return removeface(f1, next);
	}
	next=next->next;
    }
    return 0;
}
void  showfaces(face * g, face * activeface)
{
	while(g)
	{
	    #ifdef GL
		glPushMatrix();
		glTranslatef(g->x,g->y,0);
	    #endif
	    #ifdef SDLD
		gltx=g->x+cam.x;
		glty=g->y+cam.y;
	    #endif
	    int th=0;
	    if(global_tabbing)
	    {
		th=activeface->theme;
		activeface->theme=1;
	    }
	    if(
	    #ifdef python
	    g->showfunc||
	    #endif
	    g->t||g==activeface)showface(g);
	    if(global_tabbing)
		activeface->theme=th;
	    g=g->next;
	    #ifdef GL
		glPopMatrix();
	    #endif
	}
}

void clipin(face *f,int noes, int sel)
{
    char * r=rotoclipin(sel);
    char *s=r;
    if(s)
    {
    while(*r)
    {
	if((noes && (*r)!=10 && (*r)!=13 ) || !noes) keyp(f,*r);
	r++;
    }
    free(s);
    }
}

void type(face *f, char * r)
{
    while(*r)
    {
	keyp(f,*r);
	r++;
    }
}

void clipoutlastline(face *f)
{
    if(f->t->crow<1)return;
    int i;
    char *s=malloc(f->t->cols+1);
    s[f->t->cols]=0;
    for (i=0;i<f->t->cols;i++)
	s[i]=f->t->cells[f->t->crow-1][i].ch;
    rote_vt_forsake_child(clipout);
    rotoclipout(s,clipout, 1);
    logit("%s\n",s);
    free(s);
}




#ifdef nerve
void shownerv(struct state *nerv)
{
    nerverot_update(nerv);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glFrustum(-1, 1, -1, 1, 1.5, 10);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef(0,0,-2.5);
    glPushAttrib(GL_BLEND);
    glEnable(GL_BLEND);
    nerverot_draw(3,nerv);
    glPopAttrib();
    wm();
}
#endif

#ifndef threaded
void updateterminal(RoteTerm *t)
{
    rote_vt_update(t);
      if(!t->childpid)
      {
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=1;
	e.user.data1=t;
	SDL_PushEvent(&e);
      }
}
void updateterminals(face *g)
{
	while(g)
	{
	    if(g->t)
		updateterminal(g->t);
	    g=g->next;
	}
}
#endif

void freefaces(face *g)
{
	while(g)
	    g=removeface(g,g);

}

void loadsettings(void)
{
    tpl_node *tn;
    int32_t b;
    tn=tpl_map("i", &b);
    if(!tpl_load(tn, TPL_FILE, stng))
	if(tpl_unpack(tn,0)>0)
	    blending=b;
    tpl_free(tn);
}
void savesettings(void)
{
    tpl_node *tn;
    int32_t b;
    tn=tpl_map("i", &b);
    b=blending;
    tpl_pack(tn,0);
    tpl_dump(tn, TPL_FILE, stng);
    tpl_free(tn);
}


face * loadfaces(void)
{
    face * result=0, * next=0;
    face *g;

    tpl_node *tn;
    double a,b,c;
    int32_t t,cols,rows;
    tn = tpl_map("A(fffiii)",&a,&b,&c,&t,&cols,&rows);
    tpl_load(tn,TPL_FILE, fcfl);
    while(tpl_unpack(tn,1)>0)
    {
	g=new_face();
	g->scale=a;
	g->x=b;
	g->y=c;
	if(t)
	{
	    logit("adding term with %i %i, scale %f", rows,cols,a);
	    add_term(g,rows,cols);
	}
	g->theme=4;
	if(!result)
	    result=g;
	if(next)
	    next->next=g;
	next=g;
    }
    tpl_free(tn);
    return result;
}

void savefaces(face * f1)
{
    tpl_node *tn;
    double a,b,c;
    int32_t t,cols,rows;
    tn = tpl_map("A(fffiii)",&a,&b,&c,&t,&cols,&rows);
    while(f1)
    {

	if(!f1->scripted)
	{
	    a=f1->scale;
	    b=f1->x;
	    c=f1->y;
	    t=f1->t?1:0;
	    logit("saving face with t:%i,scale %f", t,a);
	    cols=f1->t?f1->t->cols:0;
	    rows=f1->t?f1->t->rows:0;

	    tpl_pack(tn,1);
	}
	f1=f1->next;
    }
    tpl_dump(tn, TPL_FILE, fcfl);
    tpl_free(tn);
}

face *mousefocus(face *af, face *f1)
{
    int mx;
    int my;
    SDL_GetMouseState(&mx,&my);
    face * result=af;
    while(f1)
    {
	int ax=(mx-cam.x);
	int ay=(my-cam.y);

	if(f1->t)
	{
	    if((f1->x<ax)&&(f1->y<ay)&&(f1->y+f1->scale*26*f1->t->rows>ay)&&(f1->x+f1->scale*13*f1->t->cols>ax))
		result=f1;
	}                                                   
	else
	{
	    if((f1->x<ax)&&(f1->y<ay)&&(f1->y+f1->scale*26*10>ay)&&(f1->x+f1->scale*13*10>ax))
		result=f1;
	}
	f1=f1->next;
    }
    return result;
}

void zoomem(face *z,double y)
{
    while(z)
    {
	z->scale+=y;
	z->x=z->x+z->x*y;
	z->y=z->y+z->y*y;
	z=z->next;
    }
}

void gle(void)
{
#ifdef GL
		GLenum gl_error;
		gl_error = glGetError( );
		if( gl_error != GL_NO_ERROR )
		{
			if(gl_error==GL_STACK_UNDERFLOW)
				logit("QUACK QUACK QUACK, UNDERFLOVING STACK\n");
			if(gl_error==GL_STACK_OVERFLOW)
				logit("QUACK QUACK QUACK, OVERFLOVING STACK\n");
			else if(gl_error==GL_INVALID_OPERATION)
				logit("INVALID OPERATION, PATIENT EXPLODED\n");
			else
				logit("testgl: OpenGL error: 0x%X\n", gl_error );
		}
#endif
}

#ifdef GL
void show_button(int x, int y, char * n, int picking)
{
	glPushMatrix();
	glTranslatef(x,y,0);
	if(picking)
		glBegin(GL_QUADS);
	else
		glBegin(GL_LINE_LOOP);
	glColor3f(1,1,0);
	int w=strlen(n)*13+13;
	glVertex2f(0,0);	glVertex2f(w,0);	glVertex2f(w,26);	glVertex2f(0,26);
	glEnd();
	setcolor(1,1,0,1);
	draw_text(n);
	glPopMatrix();

}
void show_buttons(int picking)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	int n=numbuttons;
	int y=0;
	glInitNames();
	glPushName(-1);
//	logit("yep\n");
	while(n)
	{
		glLoadName(n-1);
		show_button(-cam.x+SDL_GetVideoSurface()->w-100,-cam.y+(y+=100), buttonnames[--n],picking);
	}
	glLoadName(-1);
	glBlendFunc(GL_ONE, GL_ZERO);
}


int testbuttonpress(int x, int y,int test)
{
    GLuint fuf[500];
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glSelectBuffer(500, fuf);
    glMatrixMode (GL_PROJECTION);
    if(!test)glRenderMode (GL_SELECT);
    glPushMatrix ();
    glLoadIdentity ();

    gluPickMatrix (x,y,10,10, viewport);
    glOrtho(0,SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h,0,100,-100);
    glMatrixMode (GL_MODELVIEW);
    glTranslatef(cam.x,cam.y,0);
    glMatrixMode (GL_PROJECTION);
    
    show_buttons(1);
    glPopMatrix();
    int i,j, k;
    int numhits = glRenderMode(GL_RENDER);
//    logit("%i\n", numhits);
    for(i=0,k=0;i<numhits;i++)
    {
	GLuint numnames=fuf[k++];
	k++;k++;
	for(j=0;j<numnames;j++)
	{
	    GLuint n=fuf[k];
//	    logit("%i\n", n);
	    return n;
	    k++;
	}
    }
    wm();
    return -1;
}

#endif


void camtoface(face * activeface)
{
    cam.x=-activeface->x;
    cam.y=-activeface->y;
}

face *lastface(face *f)
{
	while(f&&f->next)
	    f=f->next;
	return f;
}
face *add_face(void)
{
    return lastface(face1)->next=new_face();
}
void initpython(void);
void freepython(void);
void reloadpythons(void);
void reloadbuttons(void);
int RunGLTest (void)
{
	int startup=1;
#ifdef GL
//	int mm=1;
#else
	int mm = 0;
#endif
	cam.x=0;
	cam.y=0;
	int bpp=8;
	int w = 1280;
	int h = 800;
	int done = 0;
	int shrink=0;
	int grow=0;
	int gofullscreen=0;
	int escaped = 0;

	int mustresize = 1;
	int justresized = 0;
	SDL_Surface* s;
	face *activeface;

	void myinit(void)
	{
		loadcolors();
		logit("mmm..\n");
		xy ss = parsemodes(w,h,mdfl,1,0,0);
		if (ss.x!=-1){w=ss.x;h=ss.y;};
	    #ifdef GL
		s=initsdl(w,h,&bpp,SDL_OPENGL
	    #else
		gltextsdlsurface=s=initsdl(w,h,&bpp,
	    #endif
		+0);logit("inito\n");
		SDL_EnableUNICODE(1);
		SDL_InitSubSystem( SDL_INIT_TIMER);
		SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2, SDL_DEFAULT_REPEAT_INTERVAL*2);
		if(ntfl)newtermmsg=GetFileIntoCharPointer1(ntfl);
		logit("pretty far\n");
		loadl2(fnfl);
	}
	void initgl(void)
	{
	    #ifdef GL
		wm();
//		int down=0;
		glDisable(GL_BLEND);
		//glShadeModel(GL_FLAT);
		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glLineWidth(lv);
//		glDisable (GL_LINE_SMOOTH);
//		glHint(GL_LINE_SMOOTH_HINT,GL_FASTEST);
	    #endif
	}
	void initfaces(void)
	{
		face1=new_face();
		activeface =face1 ;
		face1->next=new_face();
		logit("still?\n");
	}
#ifdef nerve
	struct state *nerv=0;
	nerv=nerverot_init(w,h);
#endif

	myinit();
	initgl();
	
#ifdef swallows3d
	init();//s3d
#endif
	
	activeface=face1=loadfaces();
	if(!face1)
	    initfaces();
	initpython();

	int dirty=1;
	logit("mainloop descent commencing\n");
	while( !done )
	{
		lockterms(face1);
		Uint8 * k=SDL_GetKeyState(NULL);
		if(dirty||faces_dirty(face1))
		{
			dirty=0;
			facesclean(face1);
			#ifdef GL
			if(blending)
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        else
				glBlendFunc(GL_ONE, GL_ZERO);
			glClear(GL_COLOR_BUFFER_BIT);
			#else
			SDL_FillRect    ( s, NULL, SDL_MapRGB( s->format, 0,0,0) );
			#endif
			#ifdef GL
			#ifdef swallows3d
			network_main();
			graphics_reshape(w,h);
			graphics_main();
			wm();
			#endif
			#ifdef nerve
			if(nerv)
			{	
				glPushAttrib(GL_BLEND);
				glEnable(GL_BLEND);
				glLineWidth(1);
				shownerv(nerv);
				glLineWidth(lv);
				dirty=1;
				glPopAttrib();
			}
			#endif
			glPushMatrix();
			glTranslatef(cam.x,cam.y,0);
			#endif



			if(k[SDLK_RCTRL])
			    focusrect(activeface);
			focusline(activeface);

			showfaces(face1, activeface);
			facesclean(face1);
			#ifdef GL
			int dol2=do_l2;
			do_l2=1;
			if(givehelp)
			{	glPushMatrix();
				glRotatef(90,0,0,1);
				glTranslatef(0,-w,0);
				if(!(escaped||k[SDLK_RCTRL]))
					draw_text("\npress right ctrl for more fun...");
				else
					draw_text("\nnow press tab to cycle thru terminals\nf12 to quit\nl to get readable font\nf9, 10, +. -, del end home and pgdn to resize terminal...\nmove terminal with left and middle, camera with right and middle mouse\nmove camera with arrows\ndo something weird with a s d f\nf1 to switch off that NERVEROT!\n/ to show buttons\nt to tile faces");
					
				glPopMatrix();
				
			}
			if(showbuttons)
				show_buttons(0);
			do_l2=dol2;
			glPopMatrix();
//			int x,y;
//			SDL_GetMouseState(&x,&y);
//			testbuttonpress(x,h-y,1);
			SDL_GL_SwapBuffers( );
			#else
			SDL_UpdateRect(s,0,0,0,0);
			#endif

		}
		gle();
		char* sdl_error;
		sdl_error = SDL_GetError( );
		if( sdl_error[0] != '\0' )
		{
			logit("testgl: SDL error '%s'\n", sdl_error);
			SDL_ClearError();
		}


		SDL_Event event;
		SDL_TimerID x=0;
#ifdef threaded
		if(dirty)
#endif
		    x= SDL_AddTimer(55, NewTimerCallback, 0);

		unlockterms(face1);
//              logit("---------unlocked wating\n");
		if(SDL_WaitEvent( &event ))
		{
		    lockterms(face1);
//                  logit("---------locked goooin %i\n", event.type);
		    if(x)SDL_RemoveTimer(x);
		    x=0;
		    do {
			int mod=event.key.keysym.mod;
			int key=event.key.keysym.sym;

			switch( event.type )
			{

				case SDL_MOUSEMOTION:
					if(escaped||k[SDLK_RCTRL])
					{
						escaped=0;
						if((SDL_BUTTON(1)|SDL_BUTTON(2))&SDL_GetMouseState(0,0))
						{
							dirty=1;
							activeface->x+=event.motion.xrel;
							activeface->y+=event.motion.yrel;
						}
						if((SDL_BUTTON(3)|SDL_BUTTON(2))&SDL_GetMouseState(0,0))
						{
							dirty=1;
							cam.x-=event.motion.xrel;
						    	cam.y-=event.motion.yrel;
						}
					}
					else
					{
					    if(activeface->t)
					    {
						int tx=-1+(event.button.x-cam.x-activeface->x)/activeface->scale/13;
						int ty=(event.button.y-cam.y-activeface->y)/activeface->scale/26;
						rote_vt_mousemove  (activeface->t,tx,ty);
					    }
                    			}
					if(!SDL_GetMouseState(0,0))
						activeface=mousefocus(activeface,face1);


				break;
				case SDL_KEYUP:
				{
					if ( (key == SDLK_RCTRL) )
					{
						dirty=1;
						global_tabbing=0;
					}
				}
				break;
				case SDL_KEYDOWN:
					if(mod&KMOD_RSHIFT&&(key==SDLK_HOME||key==SDLK_END||key==SDLK_PAGEUP||key==SDLK_PAGEDOWN))
					{
						dirty=1;
						if(key==SDLK_PAGEUP)
							activeface->scroll+=9;
						if(key==SDLK_PAGEDOWN)
							activeface->scroll-=9;
						if(key==SDLK_END)
							activeface->scroll=0;
						if(key==SDLK_HOME)
							if(activeface->t)
							    activeface->scroll=activeface->t->logl;
						if(activeface->scroll<0)activeface->scroll=0;
//                                              logit("scroll:%i,logl:%i, log&%i, t:%i ,b:%i\n", tscroll,activeface->t->logl, activeface->t->log,activeface->t->scrolltop,activeface->t->scrollbottom);
					}
					else
					if(mod&KMOD_RSHIFT&&(key==SDLK_INSERT))
					{
					    clipin(activeface,0,1);
					}
					else
					if(key==SDLK_RCTRL||mod&KMOD_RCTRL||escaped)
					{
						dirty=1;
						escaped=0;
						if(key==SDLK_RCTRL) escaped=1;
						if(k[SDLK_UP])
						{
						    dirty=1;
						    cam.y+=50;
						    activeface=mousefocus(activeface,face1);
						}
						if(k[SDLK_DOWN])
						{
						    dirty=1;
						    cam.y-=50;
						    activeface=mousefocus(activeface,face1);
						}
						if(k[SDLK_LEFT])
						{
						    dirty=1;
						    cam.x+=50;
						    activeface=mousefocus(activeface,face1);
						}
						if(k[SDLK_RIGHT])
						{
						    dirty=1;
						    cam.x-=50;
						    activeface=mousefocus(activeface,face1);
						}



						switch (key)
						{
							case SDLK_SLASH:
							    showbuttons=!showbuttons;
							    break;
							case SDLK_INSERT:
							    clipoutlastline(activeface);
							    break;
							case SDLK_TAB:
							    cycle(face1, &activeface);
							    global_tabbing=1;
							    camtoface(activeface);
							    dirty=1;
							break;
							case SDLK_F2:
							    gofullscreen=1;
							break;
							case SDLK_F3:
							    
							    dirty=1;
							break;
							case SDLK_F4:
							    
							    dirty=1;
							break;
							case SDLK_F5:
							    reloadpythons();
							    reloadbuttons();
							    dirty=1;
							break;
							case SDLK_F6:
							    ;
							    dirty=1;
							break;

							case SDLK_F7:
							    savemode(w,h,mdfl);
							break;
							case SDLK_F8:
							    loadl2(fnfl);
							break;
							case SDLK_F9:
							    activeface->scale-=0.1;
							    if(activeface->t)
								rote_vt_resize(activeface->t, h/26/activeface->scale,w/13/activeface->scale);
							    dirty=1;


							break;
							case SDLK_F10:
							    activeface->scale+=0.1;
							    if(activeface->t)
								rote_vt_resize(activeface->t, h/26/activeface->scale,w/13/activeface->scale);
							    dirty=1;

							break;
							case SDLK_RETURN:
							    zoomem(face1,0.05);
							    dirty=1;
							    break;
							case SDLK_BACKSPACE:
							    zoomem(face1,-0.05);
							    dirty=1;
							    break;
							
							case SDLK_MINUS:
							    activeface->scale-=0.05;
							    dirty=1;


							break;
							case SDLK_EQUALS:
							    activeface->scale+=0.05;
							    dirty=1;

							break;
							case SDLK_F11:
							    shrink=1;
							break;
							case SDLK_F12:
							    //grow=1;
							    done=1;

							break;
							case SDLK_t:
							{
							    face * fa = face1;
							    int yy=0;
							    while(fa)
							    {
								fa->x=0;
								fa->y=yy;
								if(fa->t)
								    yy+=26*fa->t->rows;
								else
								    yy+=26*100;
								fa=fa->next;
							    }
							    cam.x=-activeface->x;
							    cam.y=-activeface->y;
							    
							    break;
							}

							case SDLK_p:
							    #ifdef libpng
							    saveScreenshot();
							    #endif
							break;
							case SDLK_l:
							    do_l2=!do_l2;
							    dirty=1;
							break;
							case SDLK_r:
							    freefaces(face1);
							    initfaces();
							    dirty=1;
							break;
							case SDLK_n:
							    activeface=new_face();
							    activeface->next=face1;
							    face1=activeface;
							    add_terminal(face1);
							    camtoface(activeface);
							    dirty=1;
							break;
							case SDLK_PERIOD:
							    cam.x=-activeface->x;
							    cam.y=-activeface->y;
							    dirty=1;
							break;
							case SDLK_a:
							    activeface->theme--;
							    if (activeface->theme<0)activeface->theme=0;

							    break;
							case SDLK_s:
							    activeface->theme++;
							    if (activeface->theme>4)activeface->theme=4;

							    break;
#ifdef GL

							case SDLK_d:
							    lv++;
							    if(lv>5)lv=5;
							    glLineWidth(lv);

							    break;
							case SDLK_f:
							    lv--;
							    if(lv<1)lv=1;
							    glLineWidth(lv);

							    break;
							case SDLK_b:
							    blending=!blending;
							    break;
#endif

							case SDLK_END:
							    resizooo(activeface, 0,1,k);
							break;
							case SDLK_HOME:
							    resizooo(activeface, 0,-1,k);
							break;
							case SDLK_DELETE:
							    resizooo(activeface, -1,0,k);
							break;
							case SDLK_PAGEDOWN:
							    resizooo(activeface, 1,0,k);
							break;
#ifdef nerve
							case SDLK_F1:
								if(nerv)
								{
									nerverot_free(nerv);
									dirty=1;
									nerv=0;
								}
								else
								{
									nerv=nerverot_init(w,h);
									dirty=1;
								}
							break;
							case SDLK_m:
								if(nerv)
									nerverot_cycledown(nerv);
							break;
							case SDLK_COMMA:
								if(nerv)
									nerverot_cycleup(nerv);
							break;
#endif

						}
					}
					else
					{
					    activeface->scroll=0;
					    if(activeface->t==0&&!activeface->scripted)
					    {
						logit("debug messages r fun\n");
						add_terminal(activeface);
						activeface->next=new_face();
						dirty=1;
					    }
					    if(activeface->t)
					    {
						sdlkeys(activeface->t, key, event.key.keysym.unicode, mod);
//						logit("%i\n", key);
					    }
					}
				break;
				case SDL_QUIT:
					done = 1;
					break;
				case SDL_MOUSEBUTTONDOWN:
				{
				    if(activeface->t)
				    {
					#ifdef GL
					int b;
					if(showbuttons&&(b=testbuttonpress(event.button.x,h-event.button.y,0))!=-1)
					{
						logit("pressed %i\n",b);
						if(!strncmp(buttons[b], "!python", 7))
						{
						#ifdef python
							PyRun_SimpleString(&buttons[b][8]);
						#endif
                                                }
						else
							type(activeface, buttons[b]);
						showbuttons=0;
					}
					#endif
					{
						int tx=-1+(event.button.x-cam.x-activeface->x)/activeface->scale/13;
						int ty=(event.button.y-cam.y-activeface->y)/activeface->scale/26;
						logit("docellmoyuse: %i", activeface->t->docellmouse);
						if(event.button.button==SDL_BUTTON_LEFT&&(!activeface->t->docellmouse||k[SDLK_RSHIFT]||k[SDLK_LSHIFT]))
						{
						    if(clicksphase==0)
						    {
							if(lastclicktime+dblclick>SDL_GetTicks())
							{
							    clicksphase++;
							    selendx=activeface->t->cols;
							    selstartx=activeface->t->ccol;
							    selendy=ty;
							    selstarty=ty;
							    selface=activeface;
							}	
							else
							{
							    selface=0;
							    selstartx=tx;
							    selstarty=ty;
							    lastclicktime=SDL_GetTicks();
							}
						    }
						    else
						    {
							if(lastclicktime+dblclick>SDL_GetTicks())
							{
							    selface=activeface;
							    clicksphase++;
							    selendx=activeface->t->cols;
							    selstartx=0;
							    selendy=ty;
							    selstarty=ty;
							}	
							else
							{
							    selface=0;
							    clicksphase=0;
							    selstartx=tx;
							    selstarty=ty;
							    lastclicktime=SDL_GetTicks();
							}
						    }
						}
						else if(event.button.button==SDL_BUTTON_MIDDLE&&(!activeface->t->docellmouse||k[SDLK_RSHIFT]||k[SDLK_LSHIFT]))
						{
							clipin(activeface, 0,0);
						}
						else
						{
						    rote_vt_mousedown(activeface->t,tx,ty);
						    logit("mousedown");
						}
						logit("%i %i\n", tx,ty);
					}
				    }
				    break;
				}
				case SDL_MOUSEBUTTONUP:
				{
				    if(activeface->t)
				    {
					int tx=-1+(event.button.x-cam.x-activeface->x)/activeface->scale/13;
					int ty=(event.button.y-cam.y-activeface->y)/activeface->scale/26;
					rote_vt_mouseup  (activeface->t,tx,ty);
					logit("mouseup");
				    }
				    break;
				}
				case SDL_VIDEOEXPOSE:
					dirty=1;
					break;
				case SDL_VIDEORESIZE:
				    {
					if(!s)exit(1);
					w=event.resize.w;h=event.resize.h;
                                        logit("videoresize %ix%i bpp %i\n", w,h,bpp);
					dirty=1;
					s=SDL_SetVideoMode( w,h, bpp, s->flags);
					wm();
					if(!justresized)
					    mustresize=1;
					justresized=0;
				    }
				    if(activeface->t)
					rote_vt_resize(activeface->t, h/26/activeface->scale,w/13/activeface->scale);
				    dirty=1;
				break;
				case SDL_USEREVENT:
					if(event.user.code==1)
					{
					    if(activeface->t==event.user.data1)
					    {
						activeface=activeface->next;
						if(!activeface)
						    activeface=face1;
					    }
					    face1=RemoveTerm(face1, event.user.data1);
					    dirty=1;
					}
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if (shrink||grow)
		    {
			resize(&w,&h,&bpp,&s->flags,&shrink,&grow);
			wm();
		    }
		    if (mustresize)
		    {
			mustresize=0;
			justresized=1;
		    }
		    if(gofullscreen)
		    {
			dirty=1;
			if(s->flags & SDL_FULLSCREEN )
			{

			    s=SDL_SetVideoMode( w,h, bpp, (s->flags & ~SDL_FULLSCREEN ));
			    logit("gooin !fuulin");
			}
			else
			{
			    SDL_Surface *ns;
			    ns=SDL_SetVideoMode( w,h, bpp, (s->flags | SDL_FULLSCREEN ));
			    if(ns)s=ns;
			}
			gofullscreen=0;
		    }
		    if(startup)
		    {
			startup=0;
//			if(!face1->t&&!face1->scripted)
//			{
//			    add_terminal(face1);
  //              lockterm(face1);
//			}
		    }
		    if(!done)
		    {
			unlockterms(face1);
			#ifndef threaded
				updateterminals(face1);
			#endif
		    }
		    else
		    {
			savemode(w,h,mdfl);
			savefaces(face1);
		    }
		}

	}
	freepython();
	freefaces(face1);
	freel2();
	SDL_Quit( );
	return(0);
}
#ifdef python
PyObject *pglBegin(PyObject *self, PyObject* args)
{
    int y;
    if(PyArg_ParseTuple(args, "i",&y))
    {
	glBegin(y);
	Py_INCREF(Py_None)     ;
	return Py_None;
    }
    return 0;
}
PyObject *pglEnd(PyObject *self, PyObject* args)
{
	glEnd();
	Py_INCREF(Py_None)    ;
	return Py_None;
}
PyObject *pglColor4f(PyObject *self, PyObject* args)
{
    float a,b,c,d;
    if(PyArg_ParseTuple(args, "ffff",&a, &b, &c, &d))
    {
	glColor4f(a,b,c,d);
	Py_INCREF(Py_None)   ;
	return Py_None;
    }
    return 0;
}
PyObject *pglVertex2f(PyObject *self, PyObject* args)
{
    float a,b;
    if(PyArg_ParseTuple(args, "ff",&a, &b))
    {
	glVertex2f(a,b);
	Py_INCREF(Py_None)  ;
	return Py_None;
    }
    return 0;
}
PyObject *pgetface(PyObject *self, PyObject* args)
{
	face*f=add_face();
	f->t=0;
	f->scripted=1;
	PyObject *ret=Py_BuildValue("i", f);
	Py_INCREF(ret) ;
	return ret;
}
PyObject *phookdraw(PyObject *self, PyObject* args)
{
    PyObject *func;
    face * f;
    if(PyArg_ParseTuple(args, "iO",&f, &func))
    {
	f->showfunc=func;
	Py_INCREF(Py_None);
	return Py_None;
    }
    return 0;
}
PyObject *pfork(PyObject *self, PyObject* args)
{
    face * f;
    char *s;
    if(PyArg_ParseTuple(args, "is",&f, &s))
    {
	f->scripted=0;
	add_terminal(f);
	type(f,s);
	Py_INCREF(Py_None);
	return Py_None;
    }
    return 0;
}
PyObject *ptype(PyObject *self, PyObject* args)
{
    face * f;
    char *s;
    if(PyArg_ParseTuple(args, "is",&f, &s))
    {
	type(f,s);
	Py_INCREF(Py_None);
	return Py_None;
    }
    return 0;
}
PyObject *psetlabel(PyObject *self, PyObject* args)
{
    face * f;
    char *s;
    if(PyArg_ParseTuple(args, "is",&f, &s))
    {
	if(f->label)free(f->label);
	f->label=strdup(s);
	Py_INCREF(Py_None);
	return Py_None;
    }
    return 0;
}
PyObject *pfreeface(PyObject *self, PyObject* args)
{
    face * f;
    if(PyArg_ParseTuple(args, "i",&f))
    {
	face1=removeface(face1, f);
	Py_INCREF(Py_None);
	return Py_None;
    }
    return 0;
}

PyMethodDef lemon_methods[] =
{
{"glBegin",pglBegin,METH_VARARGS, "Return the meaning of everything."},
{"glEnd",pglEnd,METH_NOARGS, "Return the meaning of everything."},
{"glColor4f",pglColor4f,METH_VARARGS, "Return the meaning of everything."},
{"glVertex2f",pglVertex2f,METH_VARARGS, "Return the meaning of everything."},
{"getface",pgetface,METH_VARARGS, "Return the meaning of everything."},
{"freeface",pfreeface,METH_VARARGS, "Return the meaning of everything."},
{"hookdraw",phookdraw,METH_VARARGS, "Return the meaning of everything."},
{"fork",pfork,METH_VARARGS, "Return the meaning of everything."},
{"type",ptype,METH_VARARGS, "Return the meaning of everything."},
{"setlabel",psetlabel,METH_VARARGS, "Return the meaning of everything."},
{NULL,NULL}/* sentinel */
};
#endif

void btnsfunc(char *path, char *justname)
{
	char *b=GetFileIntoCharPointer1(path);
	if(b)
	{
		logit("%s\n", justname);
		numbuttons++;
		buttons=realloc(buttons,sizeof(char*)* numbuttons);
		buttons[numbuttons-1]=b;
		buttonnames=realloc(buttonnames, sizeof(char*)*numbuttons);
		buttonnames[numbuttons-1]=strdup(justname);
	}
}

#ifdef python
void pythfunc(char *path, char *justname)
{
	char *b=GetFileIntoCharPointer1(path);
	if(b)
	{
		logit("%s\n", justname);
		PyRun_SimpleString(b);
	}
}
#endif

void listdir(char *path, void func(char *, char *))
{
	DIR *dir = opendir(path);
	if(dir)
	{	
		path=strdup(path);
		int maxsize = 50;
		path=realloc(path, strlen(path)+maxsize+1);
		char* n=strrchr(path, 0);
		struct dirent *ent;
		while((ent = readdir(dir)) != NULL)
		{
			if(strlen(ent->d_name) < maxsize)
			{
				strcat(path,ent->d_name);
				func(path,ent->d_name);
				*n=0;
			}
		}
		free(path);
	}
	else
	{
		logit("Error opening directory\n");
	}
}
                      
#include "makemessage.c"
void initpython( void)
{
#ifdef python
	Py_Initialize();
        PyImport_AddModule("lemon");
	Py_InitModule("lemon", lemon_methods);
	PyRun_SimpleString("from lemon import *");
	PyRun_SimpleString("afterstart=list()");
	PyRun_SimpleString("beforefinish=list()");
	char *t=make_message("path=\"%s\"",getexepath());
	PyRun_SimpleString(t);
	free(t);
	logit("pythons from %s:\n", pyth);
	listdir(pyth, &pythfunc);
	PyRun_SimpleString("for one in afterstart:\n	one()");
#endif
}

void initbuttons(void)
{
	logit("buttons:\n");
	listdir(btns, &btnsfunc);
}

void freebuttons(void)
{
	int x=numbuttons;
	while(x)
	{
	    free(buttons[--x]);
	    free(buttonnames[x]);
	}
	buttonnames=0;
	buttons=0;
	numbuttons=0;
}

void freepython(void)
{
    #ifdef python
    PyRun_SimpleString("for one in beforefinish:\n	one()");
    Py_Finalize();
    #endif
}

void reloadpythons(void)
{
    freepython();
    initpython();
}

void reloadbuttons(void)
{
    freebuttons();
    initbuttons();
}

int main(int argc, char *argv[])
{
        char *help=0;

	logit("hi\n");
	logit("outdated info:right Ctrl+ Home End PgDn Delete to resize, f12 to quit, f9 f10 scale terminal tab to tab thru terminals, \n");

	char *path;
	path=getexepath();
	if(path)
	{
		logit("path:%s\n", path);
		path=realloc(path, 1+strlen(path)+strlen("newtermmsg"));//newtermmsg is the longest string
		char* n=strrchr(path, 0);
		fnfl=strdup(strcat(path, "l2"));
		*n=0;
		clfl=strdup(strcat(path, "colors"));
		*n=0;
		ntfl=strdup(strcat(path, "newtermmsg"));
		*n=0;
		stng=strdup(strcat(path, "settings"));
		*n=0;
		mdfl=strdup(strcat(path, "mode"));
		*n=0;
		fcfl=strdup(strcat(path, "faces"));
		*n=0;
		help=strdup(strcat(path, "nohelp"));
		*n=0;
		btns=strdup(strcat(path, "buttons/"));
		*n=0;
		pyth=strdup(strcat(path, "python/"));
		free(path);
	}
	loadsettings();
	int i;
	for(i=1;i<argc;i++)
	    if(!strcmp(argv[i],"-f"))
		if(argc>i)
		    fnfl=argv[i+1];
	FILE* f;
	if((f=fopen(help, "r")))
	{
	    givehelp=0;
	    fclose(f);
	}

	clipout=rote_vt_create(10,10);
	clipout2=rote_vt_create(10,10);
	initbuttons();
	RunGLTest();
	freebuttons();
	savesettings();
	rote_vt_destroy(clipout);
	rote_vt_destroy(clipout2);
	logit("finished.bye.\n");
	return 0;
}






