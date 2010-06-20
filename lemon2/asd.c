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
#include <dirent.h>
#include "getexecname.c"
#include "../roteterm/demo/sdlkeys.c"
#ifdef SDLD
    #include "../sdldlines.c"
#endif
#include "../gltext.c"
#include "glterm.c"
#include <iostream>
#include <vector>
#define _mutexV( d ) {if(SDL_mutexV( d )) {logit("SDL_mutexV!");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {logit("SDL_mutexP!");}}
#define as dynamic_cast<
#define is as
#define for_each_object for(int i=0;i<objects.size();i++) { obj*o=objects.at(i);
#define for_each_face for_each_object face*f=as face>o;
#ifdef GL
    inline void dooooot(float x,float y)
    {
        glVertex2f(x,y);
    }
#endif
char *tpl_settings="S(iiis)";
struct settings
{
    int32_t line_antialiasing=0;
    int32_t showbuttons=0;
    int32_t givehelp=1;
    int32_t do_l2=0;
    double lv=2;//glLineWidth
}

char *fnfl="l2";//font file

char *stng;//settings
char *mdfl;//modes
char *fcfl;//faces
char *ntfl;//newtermmsg
char *newtermmsg;
char *btns;//buttons/
char **buttons;
char **buttonnames;
int numbuttons;
float floabs(float x)
{
    return x>0 ? x : -x;
}
void logit(const char * iFormat, ...)
{
    va_list argp;
    va_start(argp, iFormat);
    vprintf(iFormat, argp);
    va_end(argp);
    printf("\n");
}
typedef struct
{
    RoteTerm* t;
    SDL_mutex *lock;
    SDL_Thread *thr;
}
moomoo;
struct obj;
obj * active;
struct obj{
    int dirty;
    double x,y,z;
    double a,b,c;
    double w,h,d;
    virtual void draw();
    virtual void pick();
    virtual int getDirty(){return dirty;}
    virtual void setDirty(int d){dirty=d}
    void switchpositions(obj*o){
	double t,tt,ttt,tttt,ttttt,tttttt;
	t-o->x;tt=o->y;ttt=o->z;tttt=o->a;ttttt=o->b;tttttt=o->c;
	o->a=a;o->b=b;o->c=c;o->x=x;o->y=y;o->z=z;
	x=t;y=tt;z=ttt;a=tttt;b=ttttt;c=tttttt;
    }
    void boundingbox()
    {
        glVertex(0,0,0);
        glVertex(w,0,0);
        glVertex(w,h,0);
        glVertex(0,h,0);
	if(d)
	{
    	    glVertex(0,0,d);
	    glVertex(w,0,d);
    	    glVertex(w,h,0);
    	    glVertex(0,h,0);
    	    glVertex(0,0,d);
    	    glVertex(w,0,d);
    	    glVertex(w,h,0);
    	    glVertex(0,h,0);
    	    glVertex(0,0,d);
    	    glVertex(w,0,d);
    	    glVertex(w,0,0);
    	    glVertex(w,0,d);
    	    glVertex(0,0,d);
    	    glVertex(w,0,0);
    	    glVertex(w,h,0);
    	    glVertex(0,h,0);
    	    glVertex(0,0,0);
    	    glVertex(w,0,0);
    	    glVertex(w,h,0);
    	    glVertex(0,h,0);
	}
    }
    
    void pick()
    {
        glBegin(GL_QUADS);
	boundingbox();
	glEnd();
    }

    void move(double xx,double yy,double zz)
    {
	x+=xx;
	y+=yy;
	z+=zz;
	dirty=1;
    }
    void translate_and_draw()
    {
	#ifdef GL
	    glPushMatrix();
	    glTranslated(x,y,z);
	    glRotated(a,b,c);
	#endif
	draw();
	#ifdef GL
	    glPopMatrix();
	#endif
    }
    void translate_and_pick()
    {
	#ifdef GL
	    glPushMatrix();
	    glTranslated(x,y,z);
	    glRotated(a,b,c);
	#endif
	pick();
	#ifdef GL
	    glPopMatrix();
	#endif
    }
    virtual void keyp(int key,uni,mod)=0;
    ~obj(){if(active==this)active=0;}
};
#ifdef nerve
    class nerverot:public obj
    {
        struct nerverotstate *nerv
        public:
	int w,h;
        void draw()
        {
            nerverot_update(nerv);
            nerverot_draw(3,nerv);
        }
	nerverot()
	{
	    nerv=nerverot_init(w=SDL_GetVideoSurface()->w,h=SDL_GetVideoSurface()->h);
	}
	~nerverot()
	{
	    nerverot_free(nerv);
	}
	void keyp(int key,uni,mod)
	{
	    switch(key)
	    {
		case SDLK_LEFT:
		    nerverot_cycledown(nerv);
		    break;
		case SDLK_RIGHT:
		    nerverot_cycleup(nerv);
		    break;
	    }
	}
	int getDirty(){return 1;}
    }
#endif
class facespawner:obj
{
    int w=100;
    int h=100;
    void pick()
    {
        if(newtermmsg)
        {
    	    glBegin(GL_QUADS);
    	    glVertex(0,0,0);
	    glVertex(w,0,0);
	    glVertex(w,h,0);
	    glVertex(0,h,0);
	    glEnd();
	}
    }
    void keyp(int key,uni,mod)
    {
        faces.push_back(face*f=new face);
        f->x=x;
        f->y=y;
        f->z=z;
    }
    void draw()
    {
        draw_text(newtermmsg);
    }
}
struct face:public obj
{
    RoteTerm *t;
    Uint32 last_resize;
    int lastxresize;
    int lastyresize;
    moomoo upd_t_data;
    double scale;
    int scroll;
    int oldcrow, oldccol;
    Uint32 lastrotor;
    double rotor;
    int selstartx,selstarty,selendx,selendy;
    int getDirty(){return dirty||t->dirty;}
    void setDirty(int d){t->dirty=d;obj::setDirty(d);}
    face()
    {
	memset(this,0,sizeof(face));
	f->scale=1;
	return f;
    }
    void ~face()
    {
	rote_vt_destroy(t);
	#ifdef threaded
	    SDL_DestroyMutex(upd_t_data.lock);
	    SDL_KillThread(upd_t_data.thr);
	#endif
    }
    void resizooo(int xx, int yy, Uint8* duck)
    {
        int up=duck[SDLK_HOME]||(yy==-1);
        int dw=duck[SDLK_END]||(yy==1);
        int lf=duck[SDLK_DELETE]||(yx==-1);
        int ri=duck[SDLK_PAGEDOWN]||(yx==1);
        if(up)
	    yy=-1;
	if(dw)
	    yy=1;
        if(lf)
	    xx=-1;
        if(ri)
	    xx=1;
        if((!xx+lastxresize||!yy+lastyresize)||(SDL_GetTicks()-last_resize>100)||!last_resize)
        {
    	    rote_vt_resize(t, t->rows+yy,t->cols+xx);
	    last_resize=SDL_GetTicks();
	}
	lastxresize=xx;
	lastyresize=yy;
    }
    void add_term(int c,int r)
    {
	logit("adding terminal");
	t = rote_vt_create(c,r);
	rote_vt_forkpty((RoteTerm*) t, "bash");
	#ifdef threaded
	    upd_t_data.lock=SDL_CreateMutex();
	    upd_t_data.t=t;
	    logit("upd_t_data.lock=%i", f->upd_t_data.lock);
	    upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&upd_t_data);
	#endif
    }
    void add_terminal()
    {
	add_term(SDL_GetVideoSurface()->h/26/f->scale,SDL_GetVideoSurface()->w/13/f->scale);
    }
    void draw()
    {
    	draw_terminal(this);
    	
    }
    void keyp(int key,uni,mod)
    {
    	if(mod&KMOD_RSHIFT&&(key==SDLK_INSERT))
	    clipin(activeface,0,1);
	else if(mod&KMOD_RSHIFT&&(key==SDLK_HOME||key==SDLK_END||key==SDLK_PAGEUP||key==SDLK_PAGEDOWN))
	{
	    if(key==SDLK_PAGEUP)
		scroll+=9;
	    if(key==SDLK_PAGEDOWN)
	    	scroll-=9;
	    if(key==SDLK_END)
		scroll=0;
	    if(key==SDLK_HOME)
		scroll=t->logl;
	    if(scroll<0)scroll=0;
	}
	else
	{
	    if(key!=SDLK_RSHIFT)
	        scroll=0;
    	    sdlkeys(t,key,uni,mod);
	}
	}
	void lock()
	{
	    #ifdef threaded
		if(t)_mutexP(upd_t_data.lock);
	    #endif
	}
	void unlock()
	{
	    #ifdef threaded
		if(t)_mutexV(upd_t_data.lock);
	    #endif
	}

    }
}
class mplayer
{
    int twist;
    vector<String>list;    
    void draw()
    {
	
    }
    void keyp(int key,unicode,mod)
    {
	switch(key){
	    case SDLK_t:
	    twist=!twist;
	    break;
	    case SDLK_UP:
	    pos--;
	    break;
	    case SDLk_DOWN:
	    pos++;
	    break;
	}
    }
    void load()
    {
	
    }
}
using namespace std;
vector<obj *> objects;
obj *active=0;
xy cam;
RoteTerm *clipout, *clipout2;
#define CODE_TIMER 0
#define CODE_QUIT 1
#ifdef threaded
    int update_terminal(void *data)
    {	
        while(1)
        {
	    moomoo * d = (moomoo *)data;
	    char buf[512];
	    int br=-1;
	    //logit("UNLOCKED SELECT\n");
	    rote_vt_update_thready(buf, 512, &br, d->t);
	    //logit("*end SELECT, locking %i*\n", d->lock);
	    _mutexP(d->lock);
	    //logit("LOCKED\n");
	    if (br>0)
	    {
		//logit("*locked injecting\n");
		rote_vt_inject(d->t, buf, br);
		//logit("*locked injected\n");
		SDL_Event e;
		e.type=SDL_USEREVENT;
		e.user.code=0;
		SDL_PushEvent(&e);
	    }
	    if(!d->t->childpid)
	    {
		SDL_Event e;
		e.type=SDL_USEREVENT;
		e.user.code=CODE_QUIT;
		e.user.data1=d->t;
		SDL_PushEvent(&e);
		_mutexV(d->lock);
		return 1;
	    }
	    _mutexV(d->lock);
	}
	return 1;
    }
#else
    void updateterminal(RoteTerm *t)
    {
        rote_vt_update(t);
        if(!t->childpid)
        {
	    SDL_Event e;
	    e.type=SDL_USEREVENT;
	    e.user.code=CODE_QUIT;
	    e.user.data1=t;
	    SDL_PushEvent(&e);
	}
    }
    void updateterminals()
    {
        for(i=0;i<faces.size();i++)
            if(faces.at(i)->t)
		updateterminal(faces.at(i)->t);
    }
#endif
void setmatrix()
{
    glFrustum(0,SDL_GetVideoSurface()->w,0,SDL_GetVideoSurface()->h,1,100);
}
void resetmatrices(void)
{
    #ifdef GL
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	setmatrix();
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
    #endif
}
void resetviewport(void)
{
    #ifdef GL
	glViewport( 0, 0, SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h );
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

int clean_faces(void)
{
    int c=1;
    for_each_face
	if(!clean_term(as face>objects.at(i)->t))
	    c=0;
    return c;
}
void lockterms(void)
{
    #ifdef threaded
	//logit("locking terms");
	for_each_face
	    f->lock();
	//logit("done");
    #endif
}
void unlockterms(void)
{
    #ifdef threaded
	//logit("unlocking terms");
	for_each_face
	    f->unlock();
	//logit("done");
    #endif
}
/*
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
#ifdef SDLD
    void DrawLine(SDL_Surface *s, int x, int y, int x2, int y2, int c)
    {
        if (y<1)y=1;    if (x<1)x=1;
        if(y>gltextsdlsurface->h-2)y=gltextsdlsurface->h-2;
        if(x>gltextsdlsurface->w-2)x=gltextsdlsurface->w-2;
        if (y2<1)y2=1;    if (x2<1)x2=1;
        if(y2>gltextsdlsurface->h-2)y2=gltextsdlsurface->h-2;
        if(x2>gltextsdlsurface->w-2)x2=gltextsdlsurface->w-2;
        Draw_Line(s,x,y,x2,y2,c);
    }
#endif
void focusline(face * activeface)
{
    static double angel=0;
    float csize=30;
    #ifdef GL
        glBegin(GL_POINTS);
        glColor3f(1,0,0);
        glVertex2f(csize*sin(angel)+activeface->x,csize*cos(angel)+activeface->y);
        glVertex2f(csize*sin(angel+3.14159265358979323846264/2)+activeface->x,csize*cos(angel+3.14159265358979323846264/2)+activeface->y);
        glVertex2f(csize*sin(angel+3.14159265358979323846264)+activeface->x,csize*cos(angel+3.14159265358979323846264)+activeface->y);
        glVertex2f(csize*sin(angel+1.5*3.14159265358979323846264)+activeface->x,csize*cos(angel+1.5*3.14159265358979323846264)+activeface->y);
	angel+=0.1;
        if(angel>2*3.14159265358979323846264)angel=0;
        glEnd();
    #else
	DrawLine(s,cam.x+activeface->x,cam.y+activeface->y,    cam.x+activeface->x+100,cam.y+activeface->y,barvicka);
	DrawLine(s,cam.x+activeface->x,cam.y+activeface->y+100,cam.x+activeface->x,    cam.y+activeface->y,barvicka);
    #endif
}
*/

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

/*void clipoutlastline(face *f)
{
    if(f->t->crow<1)return;
    int i;
    char *s=(char*)malloc(f->t->cols+1);
    s[f->t->cols]=0;
    for (i=0;i<f->t->cols;i++)
	s[i]=f->t->cells[f->t->crow-1][i].ch;
    rote_vt_forsake_child(clipout);
    rotoclipout(s,clipout, 1);
    logit("%s\n",s);
    free(s);
}*/
void loadsettings(void)
{
    tpl_node *tn;
    tn=tpl_map(tpl_settings, &settings);
    if(!tpl_load(tn, TPL_FILE, stng))
	tpl_unpack(tn,0);
    tpl_free(tn);
}
void savesettings(void)
{
    tpl_node *tn;
    tn=tpl_map(tpl_settings, &settings);
    tpl_pack(tn,0);
    tpl_dump(tn, TPL_FILE, stng);
    tpl_free(tn);
}

/*
void loadobjects(void)
{
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
	faces.push_back(g);
    }
    tpl_free(tn);
}

void saveobjects(void)
{
    face *f1;
    tpl_node *tn;
    double a,b,c;
    int32_t t,cols,rows;
    tn = tpl_map("A(fffiii)",&a,&b,&c,&t,&cols,&rows);
    for(int i=0;i<faces.size(); i++)
    {
        f1=faces.at(i);
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
    }
    tpl_dump(tn, TPL_FILE, fcfl);
    tpl_free(tn);
}
*/
face *mousefocus(void)
{
    int mx;
    int my;
    SDL_GetMouseState(&mx,&my);
    face * result=activeface;
    for(int i=0;i<faces.size();i++)
    {
	face *f1=faces.at(i);
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
    }
    return result;
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
void sdle(void)
{
	char* sdl_error;
	sdl_error = SDL_GetError( );
	if( sdl_error[0] != '\0' )
	{
		logit("testgl: SDL error '%s'\n", sdl_error);
		SDL_ClearError();
	}
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
	int n=numbuttons;
	int y=0;
	glInitNames();
	glPushName(-1);
	while(n)
	{
		glLoadName(n-1);
		show_button(-cam.x+100,-cam.y+(y+=100), buttonnames[--n],picking);
	}
//	glLoadName(-1);
}


int testbuttonpress(int x, int y)
{
    GLuint fuf[500];
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glSelectBuffer(500, fuf);
    glMatrixMode (GL_PROJECTION);
    glRenderMode (GL_SELECT);
    glPushMatrix ();
    glLoadIdentity ();

    gluPickMatrix (x,y,10,10, viewport);
    glOrtho(0,SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h,0,100,-100);
    glMatrixMode (GL_MODELVIEW);
    glTranslatef(cam.x,cam.y,0);
    glMatrixMode (GL_PROJECTION);
    
    show_buttons(1);
    glPopMatrix();
    resetmatrices();
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
    return -1;
}
int mousefocus(int x, int y)
{
    GLuint fuf[500];
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glSelectBuffer(500, fuf);
    glMatrixMode (GL_PROJECTION);
    glRenderMode (GL_SELECT);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (x,y,10,10, viewport);
    setmatrix();
    glMatrixMode (GL_MODELVIEW);
    glInitNames();
    glPushName(-1);
    for_each_object
        glLoadName(i);
	o->translate_and_pick();
    }
    glMatrixMode (GL_PROJECTION);
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
    return active;
}

#endif

void reloadbuttons(void);

Uint32 TimerCallback(Uint32 interval, void *param)
{
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=CODE_TIMER;
	SDL_PushEvent(&e);
	return interval;
}

void updatelinesmooth()
{
    if(settings.line_antialiasing)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
}
void updatelinewidth()
{
    glLineWidth(settings.lv);
}							    

//int RunGLTest (void)
	int startup=1;
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
int anything_dirty()
{
    for_each_object
	if(o->getDirty())
	    return 1;
    return 0;
}
int nothing_dirty()
{
    for_each_object
	o->setDirty(0);
}


int RunGLTest (void)
{
    xy ss = parsemodes(w,h,mdfl,1,0,0);
    if (ss.x!=-1){w=ss.x;h=ss.y;};
    #ifdef GL
	s=initsdl(w,h,&bpp,SDL_OPENGL
    #else
	s=initsdl(w,h,&bpp,0
    #endif
    );
    SDL_EnableUNICODE(1);
    SDL_InitSubSystem( SDL_INIT_TIMER);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2, SDL_DEFAULT_REPEAT_INTERVAL*2);
    if(ntfl)newtermmsg=GetFileIntoCharPointer1(ntfl);
    loadl2(fnfl);
    loadcolors();
    #ifdef GL
	resetviewport();
        resetmatrices();
        glEnable(GL_BLEND);
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        updatelinewidth();
	updatelinesmooth();
    #endif
//    loadobjects();
    if(!objects.size())
    {
	objects.push_back(new_face());
	objects.push_back(new_face());
    }
    while( !done )
    {
	int dirty=1;
	lockterms();
	Uint8 * k=SDL_GetKeyState(NULL);
	if(dirty||anything_dirty())
	{
	    dirty=0;
	    nothing_dirty();
	    #ifdef GL
		glClear(GL_COLOR_BUFFER_BIT);
	    #else
		SDL_FillRect    ( s, NULL, SDL_MapRGB( s->format, 0,0,0) );
	    #endif
	    if(active)
	    if(k[SDLK_RCTRL])
	        focusrect(activeface);
	    focusline(activeface);
	    for_each_object
		o->translate_and_draw();
		#ifdef GL
		glBegin(GL_LINE_STRIP);
		o->boundingbox();
		glEnd();
		#endf
	    }
	    #ifdef GL
		if(settings.givehelp)
		{	
		    glPushMatrix();
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
		SDL_GL_SwapBuffers( );
	    #else
		SDL_UpdateRect(s,0,0,0,0);
	    #endif
	    gle();
	}
	sdle();
	SDL_Event event;
	SDL_TimerID x=0;
	#ifdef threaded
	    if(dirty)
	#endif
		x= SDL_AddTimer(55, TimerCallback, 0);

	unlockterms();
	//logit("---------unlocked wating\n");
	if(SDL_WaitEvent( &event ))
	{
	    lockterms();
	    //logit("---------locked goooin %i\n", event.type);
	    if(x)SDL_RemoveTimer(x);
	    x=0;
	    do
	    {
		int key=event.key.keysym.sym;
		int uni=event.key.keysym.unicode;
		int mod=event.key.keysym.mod;
		switch( event.type )
		{
		    case SDL_MOUSEMOTION:
			if(active&&(escaped||k[SDLK_RCTRL])&&(SDL_BUTTON(1)&SDL_GetMouseState(0,0))
			{
			    escaped=0;
			    active->move(event.motion.xrel,event.motion.yrel,0);
			}
			if(!SDL_GetMouseState(0,0))
			    active=mousefocus();
		        break;
		    case SDL_KEYDOWN:
			dirty=1;		    
			if(key==SDLK_RCTRL)
    			    escaped=1;
			if(escaped||(mod&&KMOD_RCTRL))
			    switch (key)
			    {
			        case SDLK_SLASH:
				    showbuttons=!showbuttons;
				    break;
				case SDLK_TAB:
				    for_each_object
					if(active==o)
				        {
					    if(++i==faces.size())
					        i=0;
					    obj*newactive=objects.at(i);
					    newactive->switchpositions(active);
					    active=newactive;
					}
				    }
				    break;
				case SDLK_F2:
				    gofullscreen=1;
				    break;
				case SDLK_F8:
			    	    loadl2(fnfl);
			    	    break;
/*			    	case SDLK_F9:
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
*/			    	case SDLK_RETURN:
			    	    active.z+=1;
			    	    break;
				case SDLK_BACKSPACE:
			    	    active.z-=1;
		        	    break;
				case SDLK_t:
				{
			    	    int yy=0;
				    for_each_object
					o->x=yy;
					o->y=0;
					o->z=0
					yy+=->w;
				    }
				    break;
				}
				case SDLK_y:
				{
				    int yy=0;
				    for_each_object
					o->x=0;
				        o->y=yy;
				        o->z=0
				        yy+=->h;
				    }
				    break;
				}
				case SDLK_u:
				{
			    	    int yy=0;
				    for_each_object
					o->x=0;
				        o->y=0
				        o->z=yy;
				        yy+=->d;
				    }
				    break;
				}
				case SDLK_p:
				    #ifdef libpng
				        saveScreenshot();
				    #endif
				    break;
				case SDLK_l:
			    	    do_l2=!do_l2;
				case SDLK_n:
				    objects.push_back(new face);
				    active=objects.at(objects.size()-1);
				    break;
				#ifdef GL
				    case SDLK_PLUS:
				        settings.line_antialiasing?settings.lv+=0.1:settings.lv++;
				        GLint max;
				        if(settings.line_antialiasing)
					    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,&max);
					else
					    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,&max); 
					if(settings.lv>max)settings.lv=max;
					updatelinewidth();
					break;
				    case SDLK_MINUS:
				        settings.line_antialiasing?settings.lv-=0.1:settings.lv--;
				        if(settings.lv<=0)settings.lv=settings.line_antialiasing?0.1:1;
					updatelinewidth();
					break;
				    case SDLK_a:
				        settings.line_antialiasing=!settings.line_antialiasing;
				        updatelinesmooth();
				        break;
				#endif
				case SDLK_END:
				    if(is face>active)as face>active->resizooo(0,1,k);
				    break;
				case SDLK_HOME:
				    if(is face>active)as face>active->resizooo(0,-1,k);
				    break;
				case SDLK_DELETE:
				    if(is face>active)as face>active->resizooo(-1,0,k);
				    break;
				case SDLK_PAGEDOWN:
				    if(is face>active)as face>active->resizooo(1,0,k);
				    break;
			    }
			case SDL_QUIT:
			    done = 1;
			    break;
			    /*
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
					dirty=1;
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
				}*/
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
				    //if(is face>active)
				//	rote_vt_resize(as face>active, h/26/activeface->scale,w/13/activeface->scale);
				    dirty=1;
				break;
				case SDL_USEREVENT:
					if(event.user.code==CODE_QUIT)
					{
					    for_each_face
						if (f->t == event.user.data1)
						    objects.erase(i);
					    }
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
			unlockterms();
			#ifndef threaded
			updateterminals();
			#endif
		    }
		    else
		    {
			savemode(w,h,mdfl);
			//savefaces();
		    }
		}

	}
	freepython();
	freefaces();
	freel2();
	SDL_Quit( );
	return(0);
}
void add_button(char *path, char *justname)
{
	char *b=GetFileIntoCharPointer1(path);
	if(b)
	{
		logit("%s\n", justname);
		numbuttons++;
		buttons=(char**)realloc(buttons,sizeof(char*)* numbuttons);
		buttons[numbuttons-1]=b;
		buttonnames=(char**)realloc(buttonnames, sizeof(char*)*numbuttons);
		buttonnames[numbuttons-1]=strdup(justname);
	}
}

void listdir(char *path, void func(char *, char *))
{
	DIR *dir = opendir(path);
	if(dir)
	{	
		path=strdup(path);
		int maxsize = 50;
		path=(char*)realloc(path, strlen(path)+maxsize+1);
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
                      
void initbuttons(void)
{
	logit("buttons:\n");
	listdir(btns, &add_button);
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
		path=(char*)realloc(path, 1+strlen(path)+strlen("newtermmsg"));//newtermmsg is the longest string
		char* n=strrchr(path, 0);
		fnfl=strdup(strcat(path, "l2"));		*n=0;
		clfl=strdup(strcat(path, "colors"));		*n=0;
		ntfl=strdup(strcat(path, "newtermmsg"));	*n=0;
		stng=strdup(strcat(path, "settings"));		*n=0;
		mdfl=strdup(strcat(path, "mode"));		*n=0;
		fcfl=strdup(strcat(path, "faces"));		*n=0;
		help=strdup(strcat(path, "nohelp"));		*n=0;
		btns=strdup(strcat(path, "buttons/"));		*n=0;
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






