/*
atlantis
dbus freezescrollback
todo
editor
draw everywhere
client server copying
s3d










*/
/*******************************************************************
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
#include "SDL_thread.h"
#include "SDL_mutex.h"
#include "../tpl/tpl.h"
#include "SDL_syswm.h"
#include "makemessage.c"
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
#include <iostream>
#include <vector>
#include "../yaml-cpp-0.2.5/include/yaml.h"
#include "serializable.h"
#define _mutexV( d ) {if(SDL_mutexV( d )) {logit("SDL_mutexV!");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {logit("SDL_mutexP!");}}
#define CODE_DATA 0
#define CODE_TIMER 0
#define CODE_QUIT 1
#define CODE_FNFLCHANGED 2
#define as dynamic_cast<
#define is as
#define for_each_object for(int i=0;i<objects.size();i++) { obj*o=objects.at(i);
#define for_each_face for_each_object if (as face*>(o)){face*f=as face*>(o);
float znear=1;
float zfar=20;

#ifdef GL
    inline void dooooot(float x,float y)
    {
        glVertex2f(x,y);
    }
#endif
SDL_Surface* s;
#ifdef SDLD
    #include "../sdldlines.c"
#endif
char *tpl_settingz="S(iii)";
struct Settingz
{
    int32_t line_antialiasing;
    int32_t givehelp;
    double lv;//glLineWidth
}settingz={1,1,1};
using namespace std;
using namespace YAML;
#include "../gltext.c"
char *fnfl;//font file
char *stng;//settingz
char *mdfl;//modes
char *fcfl;//faces
char *clfl;//colors
char *ntfl;//newtermmsg
char *newtermmsg;
char *btns;//buttons/
typedef 
struct 
{
    unsigned char r,g,b;
}
color;
color colors[2][16];
void loadcolors(void)
{
    int i,useless;
    if(!clfl)return;
    FILE * fp = fopen(clfl,"r");
    if (fp == NULL)
    {
        printf("cant load 'colors'\n");
        return;
    }
    useless=fread(&colors,3,16,fp);
    fclose(fp);
    for (i=0;i<16;i++)
    {
	colors[1][i].r=0;
	colors[1][i].g=0;
	colors[1][i].b=colors[0][i].r+colors[0][i].b+colors[0][i].g/3;
    }

}

int min(int a, int b)
{
    return a < b ? a : b;
}

int in(int a, int b, int c)
{
    return(a<=b&&c>=b);
}


void do_color(int theme,int attr,float a)
{
    int c=(attr);//0-15
    setcolor(colors[theme][c].r/255.0,colors[theme][c].g/255.0,colors[theme][c].b/255.0,a);
}

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

#define SAVE(class) 	YAML_SERIALIZABLE_AUTO(class)\
			void emit_members(Emitter &out)const

#define LOAD	void load_members(const Node& doc)
#define save(x) YAML_EMIT_MEMBER(out, x);
#define load(x) YAML_LOAD_MEMBER(doc, x);
struct v3d:public Serializable
{
    SAVE(v3d)
    {
	save(x)
	save(y)
	save(z)
    }
    LOAD
    {
	load(x)
	load(y)
	load(z)
    }
    double x,y,z;
    v3d()
    {
    
    }
};

v3d cam;
v3d look;
v3d cr;

vector<obj *> objects;
obj * active;
struct obj:public Serializable
{
    SAVE(obj)
    {
	save(t)
	save(r)
	save(s)
    }
    LOAD
    {
	load(t)
	load(r)
	load(s)
    }
    int dirty;
    int overlay;
    double alpha;
    v3d t,r,s;
    obj()
    {
	t.x=t.y=t.z=r.x=r.y=r.z=dirty=0;
	s.x=s.y=s.z=1;
	alpha=1;
	overlay=0;
    }
    virtual void activate(){
	active=this;
    }
    virtual void picked(int b,vector<int>&v)
    {
	activate();
    }
    virtual void draw(int picking,double alpha){};
    virtual int getDirty(){return dirty;}
    virtual void setDirty(int d){dirty=d;}
    
    void move(double xx,double yy,double zz)
    {
	t.x+=xx;
	t.y+=yy;
	t.z+=zz;
	dirty=1;
    }
    void translate_and_draw(int picking, double aa,int ghost=0)
    {
	#ifdef GL
	    glPushMatrix();
	    glTranslated(t.x,t.y,t.z);
	    glRotated(r.x,1,0,0);
	    glRotated(r.y,0,1,0);
	    glRotated(r.z,0,0,1);
	    glScalef(s.x,s.y,s.z);
	#endif
	glColor4f(0,0,1,alpha*aa);
	draw(picking,alpha);
	#ifdef GL
	    glPopMatrix();
	#endif
    }
    virtual void keyp(int key,int uni,int mod)
    {
    }
    ~obj(){if(active==this)active=0;}

};

#ifdef nerve
    class nerverot:public obj
    {
        public:
	SAVE(nerverot)
	{
	    YAML_EMIT_PARENT_MEMBERS(out,obj)
	    save(nerv->please_num)
	}
	LOAD
	{
	    YAML_LOAD_PARENT_MEMBERS(doc,obj)
	    load(nerv->please_num)
	}
        void draw(int picking,double alpha)
        {
	    
    	    if(picking)
    		gluSphere(gluNewQuadric(),1,10,10);
    	    else
    	    {
        	nerverot_update(nerv);
        	nerverot_draw(3,nerv,alpha);
    	    }
        }
	nerverot()
	{
	    nerv=nerverot_init(SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h);
	    alpha=0.5;
	}
	~nerverot()
	{
	    nerverot_free(nerv);
	}
	void keyp(int key,int uni,int mod)
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
	void picked(int b,vector<int>&v)
	{
	    switch(b)
	    {
		case SDL_BUTTON_LEFT:
		    nerverot_cycleup(nerv);
		    break;
		case SDL_BUTTON_RIGHT:
		    nerverot_cycledown(nerv);
		    break;
		case SDL_BUTTON_MIDDLE:
		    activate();
	    }
	}
	int getDirty(){return 1;}
	protected:
        struct nerverotstate *nerv;

    };
#endif
#ifdef threaded
    int update_terminal(void *data)
    {
	while(1)
    	{
	    moomoo * d = (moomoo *)data;
	    char buf[512512];
	    int br=-1;
	    //logit("UNLOCKED SELECT\n");
	    rote_vt_update_thready(buf, 512512, &br, d->t);
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
	        e.user.code=CODE_DATA;
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
#endif
struct face:public obj
{
    RoteTerm *t;
    Uint32 last_resize;
    int lastxresize;
    int lastyresize;
    moomoo upd_t_data;
    int scroll;
    int oldcrow, oldccol;
    Uint32 lastrotor;
    double rotor;
    int selstartx,selstarty,selendx,selendy;
    int getDirty(){return dirty||t->dirty;}
    void setDirty(int d){t->dirty=d;obj::setDirty(d);}
    SAVE(face)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    }
    void init()
    {
	last_resize=lastxresize=lastyresize=0;
	scroll=0;
	oldcrow=oldccol=-1;
	lastrotor=rotor=0;
	selstartx=selstarty=selendx=selendy=-1;
	s.x=0.001;
	s.y=-0.005;
	s.z=0.002;
    }
    face(double x=0, double y=0, int w=80, int h=28)
    {
	init();
	obj::t.x=x;obj::t.y=y;
	add_term(w,h,"bash");
    }
    face(char* run, double x, double y, double z, double a, double b, double c)
    {
	init();
	obj::t.x=x;obj::t.y=y;obj::t.z=z;
	r.x=a;r.y=b;r.z=c;
	add_terminal(run);
    }
        
    face(const char*run)
    {
	init();
	add_terminal(run);
    }
    void type(const char * x)
    {
	while(*x)
	{
	    rote_vt_keypress(t,*x);
	    x++;
	}
    }
    ~face()
    {
	rote_vt_destroy(t);
	#ifdef threaded
	    SDL_DestroyMutex(upd_t_data.lock);
	    SDL_KillThread(upd_t_data.thr);
	#endif
	cout << "terminal destroyed"<<endl;
    }
    void resizooo(int xx, int yy, Uint8* duck)
    {
        int up=duck[SDLK_HOME]||(yy==-1);
        int dw=duck[SDLK_END]||(yy==1);
        int lf=duck[SDLK_DELETE]||(xx==-1);
        int ri=duck[SDLK_PAGEDOWN]||(xx==1);
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

    #ifndef threaded
	void updateterminal()
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
    #endif

    void add_term(int c,int r,const char *run)
    {
	logit("adding terminal");
	t = rote_vt_create(r,c);
	rote_vt_forkpty((RoteTerm*) t, run);
	#ifdef threaded
	    upd_t_data.lock=SDL_CreateMutex();
	    upd_t_data.t=t;
	    logit("upd_t_data.lock=%i",upd_t_data.lock);
	    upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&upd_t_data);
	#endif
    }
    void add_terminal(const char *run)
    {
	add_term(SDL_GetVideoSurface()->w/26*3,SDL_GetVideoSurface()->h/26,run);
    }
    
    void ghost()
    {
	glPushMatrix();
	glTranslated(obj::t.x,obj::t.y,obj::t.z);
	glRotated(r.x,1,0,0);
	glRotated(r.y,0,1,0);
	glRotated(r.z,0,0,1);
	glScalef(s.x,s.y,s.z);
        glTranslatef((t->cols-t->ccol)*26,(t->rows-t->crow)*26,0);
	glScalef(30,30,1);
	draw_terminal(1,0.2);
	glPopMatrix();
    }
	

    void draw(int picking,double alpha)
    {
	if(picking)
	{
	    glBegin(GL_QUADS);
	    glVertex2f(-t->cols/2*26,-t->rows/2*26);
	    glVertex2f(-t->cols/2*26,+t->rows/2*26);
	    glVertex2f(+t->cols/2*26,+t->rows/2*26);
	    glVertex2f(+t->cols/2*26,-t->rows/2*26);
	    glEnd();
	}
	else
    	    draw_terminal(0,alpha);
    }
    void clipin(int noes, int sel)
    {
	char * r=rotoclipin(sel);
	char *s=r;
	if(s)
	{
	    while(*r)
	    {
		if((noes && (*r)!=10 && (*r)!=13 ) || !noes) rote_vt_keypress(t,*r);
		r++;
	    }
	    free(s);
	}
    }
    void keyp(int key,int uni,int mod)
    {
    	if(mod&KMOD_RSHIFT&&(key==SDLK_INSERT))
    	    if(mod&KMOD_RCTRL)
    		clipin(0,0);
    	    else
		clipin(0,1);
	if(mod&KMOD_RCTRL)return;
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
	    cout<<scroll<<endl;
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
    void draw_terminal(int theme=0,double alpha=1)
    {
        xy lok;
        lok.x=0;
        lok.y=0;
        int j=0;
        int i;
        #ifdef GL
	    glBegin(GL_LINE_STRIP);
        #endif
        int s=min(scroll,t->logl);
	if(t->log)
	{
	    for (i=t->logl-s;i<t->logl;i++)
	    {
	        if(!t->log[i])break;
	        lok.y=((i-t->rows/2.0)-t->logl+s)*26;
	        if(t->logstart) lok.y-=100;
		if(t->logstart) lok.x-=100;
		for(j=0;j<t->log[i][0].ch;j++)
	        {
		    lok.x=(j-t->cols/2.0)*26;
		    do_color(theme,ROTE_ATTR_XFG(t->log[i][j+1].attr),alpha);
	    	    drawchar(lok,t->log[i][j+1].ch);
		}	
	    }
        }
	int isundercursor;
	for (i=0; i<t->rows; i++)
	{
	    lok.y=(s+(i-t->rows/2.0))*26;
	    for (j=0; j<t->cols; j++)
	    {
		lok.x=(j-t->cols/2.0)*26;
		if((j>0)&&((ROTE_ATTR_XBG(t->cells[i][j-1].attr))!=(ROTE_ATTR_XBG(t->cells[i][j].attr))))
		{
			do_color(theme,ROTE_ATTR_XBG(t->cells[i][j].attr),0.2*alpha);
		        _spillit(lok,"aaa{",-0.5);
		}
		if((j<t->cols-1)&&((ROTE_ATTR_XBG(t->cells[i][j+1].attr))!=(ROTE_ATTR_XBG(t->cells[i][j].attr))))
		{
			do_color(theme,ROTE_ATTR_XBG(t->cells[i][j].attr),0.2*alpha);
		        _spillit(lok,"{a{{",-0.5);
		}
		if((i<t->rows-1)&&((ROTE_ATTR_XBG(t->cells[i+1][j].attr))!=(ROTE_ATTR_XBG(t->cells[i][j].attr))))
		{
			do_color(theme,ROTE_ATTR_XBG(t->cells[i][j].attr),0.2*alpha);
		        _spillit(lok,"a{{{",-0.5);
		}
		if((i>0)&&((ROTE_ATTR_XBG(t->cells[i-1][j].attr))!=(ROTE_ATTR_XBG(t->cells[i][j].attr))))
		{
			do_color(theme,ROTE_ATTR_XBG(t->cells[i][j].attr),0.2*alpha);
		        _spillit(lok,"aa{a",-0.5);
		}
		if(t->cells[i][j].ch!=32)
		    do_color(theme,ROTE_ATTR_XFG(t->cells[i][j].attr),alpha);
		isundercursor=(!t->cursorhidden)&&((t->ccol==j)&&(t->crow==i));
		#ifdef GL
		    if(isundercursor||(selstartx<=j&&selstarty<=i&&selendx>=j&&selendy>=i))
		    {
			if((oldcrow!=t->crow)||(oldccol!=t->ccol))
			    rotor=0;//if cursor moved, reset letter rotor
			//zspillit(lok,nums[0],2.4*f->scale);//draw cursor
			glEnd();
			if(isundercursor)
			{
			    glPushMatrix();
			    glTranslatef(lok.x+13,lok.y+13,0);
			    glRotatef(SDL_GetTicks()/100,0,0,1);
    	        	    glBegin(GL_LINE_LOOP);
        	    	    int w=13;
            		    glColor4f(1,0,0,alpha);
			    glVertex2f(-w,-w);
			    glVertex2f(+w,-w);
			    glVertex2f(+w,+w);
	    		    glVertex2f(-w,+w);
			    glEnd();
			    glPopMatrix();
			    glPushMatrix();
                            glTranslatef(lok.x+13,lok.y+13,0);
			    glRotatef(SDL_GetTicks()/42,0,1,0);
			    int i;
			    int steps=10;
			    for (i=0; i<360; i+=steps)
			    {
				glRotatef(steps, 0,0,1);
				glPushMatrix();
				glTranslatef(0,100,0);
				glBegin(GL_QUADS);
				glColor4f(1,1,0,0.2*alpha);
				glVertex2f(-1,0);
				glVertex2f(1,0);
				glVertex2f(2,10);
				glVertex2f(-2,10);
				glEnd();
				glPopMatrix();
			    }
			    glPopMatrix();
			}
		    	glPushMatrix();
			glTranslatef(lok.x+13,lok.y+13,0);
			glPushMatrix();
    			glRotatef(SDL_GetTicks()/10,0,1,0);
			glBegin(GL_LINE_STRIP);
			xy molok;molok.x=-13;molok.y=-13;
			drawchar(molok,t->cells[i][j].ch);
			glEnd();
			glPopMatrix();
			glPopMatrix();
			glBegin(GL_LINE_STRIP);
		    }
		    else
		#else
		    if(isundercursor)
			// but still cursor square
			spillit(lok, "aaazzazz");
	        #endif
	        if(ROTE_ATTR_XFG(t->cells[i][j].attr)!=ROTE_ATTR_XBG(t->cells[i][j].attr))
		    drawchar(lok,t->cells[i][j].ch);
	    }
	}
	#ifdef GL
	    oldcrow=t->crow;//4 cursor rotation
	    oldccol=t->ccol;
	    glEnd();
	#endif
    }

};
#ifdef GL
    struct spectrum_analyzer:public obj
    {
	SAVE(spectrum_analyzer)
	{
	    YAML_EMIT_PARENT_MEMBERS(out,obj)
	    save(roty)
	}
	LOAD
	{
	    YAML_LOAD_PARENT_MEMBERS(doc,obj)
	    load(roty)
	}
	static GLfloat heights[16][16];
	GLfloat  scale;
	float rx,ry,rz;
	float rotx,roty,rotz;
	int getDirty(){return 1;}
	spectrum_analyzer()
	{
	    scale = 1.0 / log(256.0);
	    alpha=0.05;
	    rz=ry=rz=rotx=roty=rotz=0;
	}
	void picked(int  b,vector<int>&v)
	{
	    if(b==SDL_BUTTON_LEFT)
		ry-=2;
	    else if (b==SDL_BUTTON_RIGHT)
		ry+=2;
	    else if(b==SDL_BUTTON_MIDDLE)
	    {
		rx=ry=rz=0;
		rotx=roty=rotz=0;
	    }
	}
	void keyp(int key,int uni,int mod)
	{
	    switch(key)
	    {
		case SDLK_LEFT:
		    rx-=0.2;
		    break;
		case SDLK_RIGHT:
		    rx+=0.2;
		    break;
		case SDLK_UP:
		    ry-=0.2;
		    break;
		case SDLK_DOWN:
		    ry+=0.2;
		    break;
		case SDLK_a:
		    rz-=0.2;
		    break;
		case SDLK_z:
		    rz+=0.2;
		    break;
	    }
        }
	void draw_rectangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
	{
	    if(y1 == y2)
	    {
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y1, z1);
		glVertex3f(x2, y2, z2);
		glVertex3f(x2, y2, z2);
		glVertex3f(x1, y2, z2);
		glVertex3f(x1, y1, z1);
	    }
	    else
	    {
		glVertex3f(x1, y1, z1);
		glVertex3f(x2, y1, z2);
		glVertex3f(x2, y2, z2);
		glVertex3f(x2, y2, z2);
		glVertex3f(x1, y2, z1);
		glVertex3f(x1, y1, z1);
	    }
	}
	void draw_bar(GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue )
	{
	    GLfloat width = 0.1;
	    glColor4f(red,green,blue,alpha);
	    draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
	    draw_rectangle(x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);
	
	    glColor4f(0.5 * red, 0.5 * green, 0.5 * blue,alpha);
	    draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
	    draw_rectangle(x_offset, 0.0, z_offset, x_offset + width, height, z_offset );

	    glColor4f(0.25 * red, 0.25 * green, 0.25 * blue,alpha);
	    draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);	
	    draw_rectangle(x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);
	}
	
	void oglspectrum_gen_heights(int16_t data[2][256])
	{
	    int i,c;
	    int y;
	    GLfloat val;

	    int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};
    	    for(y = 15; y > 0; y--)
	    {
	        for(i = 0; i < 16; i++)
	        {
		    spectrum_analyzer::heights[y][i] = heights[y - 1][i];
		}
	    }
	    for(i = 0; i < 16; i++)
	    {
		for(c = xscale[i], y = 0; c < xscale[i + 1]; c++)
		{
		    if(data[0][c] > y)
		    y = data[0][c];
		}
		y >>= 7;
		if(y > 0)
		    val = (log(y) * scale);
		else
		    val = 0;
		spectrum_analyzer::heights[0][i] = val;
	    }
	}
	
	void draw(int picking,double alpha)
	{
	    int x,y;
	    GLfloat x_offset, z_offset, r_base, b_base;
	    glPushMatrix();
	    rotx+=rx;
	    roty+=ry;
	    rotz+=rz;
	    glRotatef(rotx,1,0,0);
	    glRotatef(roty,0,1,0);
	    glRotatef(rotz,0,0,1);
    	    glBegin(GL_TRIANGLES);
	    for(y = 0; y < 16; y++)
	    {
		z_offset = -1.6 + ((15 - y) * 0.2);
		b_base = y * (1.0 / 15);
		r_base = 1.0 - b_base;
		for(x = 0; x < 16; x++)
		{
		    x_offset = -1.6 + (x * 0.2);			
		    draw_bar(x_offset, z_offset, spectrum_analyzer::heights[y][x], r_base - (x * (r_base / 15.0)), x * (1.0 / 15), b_base);
		}
	    }
	    glEnd();
	    glPopMatrix();
	    FILE *f;
	    if(f=fopen("/tmp/somefunnyname", "r"))
	    {
	    	int16_t buf[2][256];
		fread(buf,256,2,f);
    		fclose(f);
		remove("/tmp/somefunnyname");
		oglspectrum_gen_heights(buf);
	    }
	}
};
GLfloat spectrum_analyzer::heights[16][16];
#endif

#include <string>
class mplayer:public obj
{
    public:
    SAVE(mplayer){
    	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD{
    	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    }
    int pos;
    int twist;
    vector<string>list;    
    void draw()
    {
	
    }
    void keyp(int key,int uni,int mod)
    {
	switch(key)
	{
	    case SDLK_t:
	    twist=!twist;
	    break;
	    case SDLK_UP:
	    pos--;
	    break;
	    case SDLK_DOWN:
	    pos++;
	    break;
	}
    }
    void loadlist()
    {
	
    }
};
#include "X11/extensions/Xcomposite.h"
#include "X11/extensions/Xdamage.h"
#include "X11/extensions/Xrender.h"
#include <X11/Xlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
class composite_window:public obj
{
    public:
    SAVE(composite_window)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD
    {
	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    }
    composite_window(Window id)
    {
	
    }
};
#define MAX_SUB_TEX 2048
#define SHM_SIZE MAX_SUB_TEX * MAX_SUB_TEX * 4
class composite:public obj
{
    public:
    SAVE(composite)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD
    {
	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    }
    int event_base, error_base;
    int render_event, render_error;
    int damage_event, damage_error;
    int fixes_event, fixes_error;
    int composite_opcode, composite_event, composite_error;
    bool hasNamePixmap;
    int root_width, root_height;
    int scr;
    char * data;
    Window root;
    Pixmap windowPix;
//    XRenderPictureAttributes<pa;
    Display *dpy;
    XShmSegmentInfo shm;	
    XImage *xim;
    Damage damage;
    void rootdamaged()
    {
	if(xim)XDestroyImage(xim);
	xim = XGetImage(dpy, root, 0,0,root_width, root_height,AllPlanes,ZPixmap);
	glTexImage2D(GL_TEXTURE_2D,0,4,root_width,root_height,0,GL_RGBA,GL_UNSIGNED_BYTE,xim->data);
    }
    composite()
    {
	xim=0;
        SDL_SysWMinfo i;
        SDL_VERSION(&i.version)
        if(SDL_GetWMInfo(&i))
        {
    	    cout << "d:"<<i.info.x11.display<<endl;
    	    cout << "gfxd:"<<i.info.x11.gfxdisplay<<endl;
    	    dpy=i.info.x11.display;
    	    SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);
    	    scr=DefaultScreen(dpy);
    	    root=RootWindow(dpy,scr);
    	    if(XRenderQueryExtension(dpy,&render_event,&render_error))
    	    {
    		if(XQueryExtension(dpy, COMPOSITE_NAME, &composite_opcode, &composite_event,&composite_error))
    		{
    		    if ( XCompositeQueryExtension( dpy, &event_base, &error_base ) )
    		    {
		        int major = 0, minor = 2; // The highest version we support
		        XCompositeQueryVersion( dpy, &major, &minor );
    	    	        // major and minor will now contain the highest version the server supports.
		        // The protocol specifies that the returned version will never be higher
    		        // then the one requested. Version 0.2 is the first version to have the
		        // XCompositeNameWindowPixmap() request.
	    	        hasNamePixmap = ( major > 0 || minor >= 2 );
			if(XDamageQueryExtension(dpy,&damage_event,&damage_error))
			{
			    if(XFixesQueryExtension(dpy, &fixes_event, &fixes_error))
			    {
  //                      	pa.subwindow_mode=IncludeInferiors;
                        	root_width = DisplayWidth(dpy,scr);
				root_height= DisplayHeight(dpy,scr);
				XGrabServer (dpy);
				XCompositeRedirectSubwindows (dpy, root, CompositeRedirectAutomatic);
				XUngrabServer (dpy);
				logit("HAPPY");
				XWindowAttributes attr;
				XGetWindowAttributes( dpy, root, &attr );
				GLuint texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
				damage=XDamageCreate(dpy,root,XDamageReportRawRectangles);
				/*if(XShmQueryExtension(dpy))
				{
				    int shm_pixmaps;
				    int shm_major;
				    int shm_minor;
				    if(XShmQueryVersion(dpy,&shm_major,&shm_minor,&shm_pixmaps))
				    {
				        logit("HAPPY");
					if(shm_pixmaps)
					{
					    logit("HAPPY");
					    if((shm.shmid = shmget (IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0600))>=0)
					    {
						logit("HAPPY");
						shm.shmaddr = (char *) shmat (shm.shmid, 0, 0);
					    }
					}
				    }
				} 
				XFlush(dpy);
				windowPix = XCompositeNameWindowPixmap(dpy, root);
				*/
					
				rootdamaged();
  //				XShmGetImage(dpy, root,xim,0,0,AllPlanes);
//				XQueryTree(dpy, window, &root_win, &parent_win, &child_list,
//				<------><------>  &num_children))
				//if(hasNamePixmap)
				  //  windowPix = XCompositeNameWindowPixmap( dpy, root );
					
			    }
			}
		    }
		}
	    }
    	}
    }
    ~composite()
    {
	XDamageDestroy(dpy, damage);
    }
    void draw(int picking,double alpha)
    {
//	XImage *xim;
//	xim = XGetImage(dpy, root, 0,0,root_width, root_height,AllPlanes,ZPixmap);

	glEnable(GL_TEXTURE_2D);
//	XFree(xim);
	glBegin(GL_QUADS);
	glColor3f(1,1,1);
	glTexCoord2f(0,0);	glVertex2f(-1,1);
	glTexCoord2f(1,0);	glVertex2f(1,1);
	glTexCoord2f(1,1);	glVertex2f(1,-1);
	glTexCoord2f(0,1);	glVertex2f(-1,-1);
	glEnd();
	glDisable(GL_TEXTURE_2D);
    }
	

};
composite *comp;

#include <sys/inotify.h>
int fontwatcherthread(void *data);

class fontwatcher:public obj
{
    public:
    SAVE(fontwatcher)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD
    {
	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    }
    SDL_Thread* t;
    int i,f;
    float osize;
    float grow,size;
    void activate()
    {
	grow=0.1;
	obj::activate();
    }
    fontwatcher()
    {
	size=osize=0;
	obj::t.x=0.3;
	obj::t.y=-0.3;
	grow=0;
	alpha=0.1;
	if((i=inotify_init())==-1) cout << "no tengo descriptor\n";
	if((f=inotify_add_watch(i,fnfl,IN_MODIFY))==-1) cout << "no tengo file\n";;
	cout << f <<endl;
	t=SDL_CreateThread(&fontwatcherthread, (void*)this);
    }
    ~fontwatcher()
    {
        SDL_KillThread(t);
        close(i);
    }
    void draw( int picking,double alpha)
    {
	#ifdef GL
	    size+=grow;
	    if(size>osize)
		grow-=0.003;
	    else 
		size=osize;
	    glColor4f(1,0,0,alpha);
	    if (size)gluSphere(gluNewQuadric(),size,10,10);
	#endif
    }
    void picked(int b,vector<int>&v)
    {
	loadfont(fnfl);
    }
};
int fontwatcherthread(void *data)
{
    while(1)
    {
	char buf[1000];
	cout << reinterpret_cast<fontwatcher*>(data)->i << endl;
	read(reinterpret_cast<fontwatcher*>(data)->i,&buf,1000);
	reinterpret_cast<fontwatcher*>(data)->grow=0.1;
        SDL_Event e;
        e.type=SDL_USEREVENT;
        e.user.code=CODE_FNFLCHANGED;
        SDL_PushEvent(&e);
        cout << "reloading " << endl;
    }
}
void add_button(char *path, char *justname, void *data);


void listdir(char *path, void func(char *, char *, void* ), void *data)
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
				func(path,ent->d_name,data);
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

void erase(obj * o)
{
    if(active==o)active=0;
    for(int i=0;i<objects.size();i++)
	if(objects.at(i)==o)
	    objects.erase(objects.begin()+i);
}
void erase(int i)
{
    if(active==objects.at(i))active=0;
    objects.erase(objects.begin()+i);
}

#ifdef GL
struct button
{
    string name,content;
    button(string n, string c)
    {
	name=n;content=c;
    }
};
class buttons:public obj
{
    public:
    SAVE(buttons)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
    }
    LOAD{YAML_LOAD_PARENT_MEMBERS(doc,obj)}
    vector<button> buttonz;
    buttons()
    {
	logit("buttons:");
	listdir(btns, &add_button,this);
	overlay=1;
	s.x=s.y=s.z=1/300;
    }
    void show_button(int x, int y, button *b, int picking,double alpha)
    {
//	cout << b->content << endl;
	glPushMatrix();
	glTranslatef(x,y,0);
	if(picking)
		glBegin(GL_QUADS);
	else
		glBegin(GL_LINE_LOOP);
	glColor4f(1,1,0,alpha);
	int w=b->content.length()*13+13;
	glVertex2f(0,0);
	glVertex2f(w,0);
	glVertex2f(w,26);
	glVertex2f(0,26);
	glEnd();
	if(!picking)
	{
	    setcolor(1,1,0,alpha);
	    draw_text(b->content.c_str());
	}
	glPopMatrix();
    }
    void draw(int picking,double alpha)
    {
	int n=buttonz.size();
	int y=0;
///	cout <<"+++"<<endl;
	while(n)
	{
		if(picking)glLoadName(n-1);
		show_button(-100,y+=100, &buttonz.at(--n),picking,alpha);
	}
//	glLoadName(-1);
    }
    void picked(int b, vector<int> &v)
    {
	if(is face*>(active))
	{
	    cout << v.at(0)<<endl;
	    as face*>(active)->type(buttonz.at(v.at(0)).content.c_str());
	    erase(this);
	}
    }
};
void add_button(char *path, char *justname, void *data)
{
	char *b=GetFileIntoCharPointer1(path);
	if(b)
	{
		logit(justname);
		reinterpret_cast<buttons*>(data)->buttonz.push_back(button(justname, b));
	}
}

#endif 

RoteTerm *clipout, *clipout2;

void updateterminals()
{
    #ifndef threaded
	for(int i=0;i<objects.size();i++) 
	{ 
	    obj*o=objects.at(i);
	    if (as face*>(o))
	    {
		face*f=as face*>(o);
		updateterminal(f->t);
	    }
	}
    #endif
}

void resetmatrices(void)
{
    #ifdef GL
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
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

void lockterms(void)
{
    #ifdef threaded
	//logit("locking terms");
	for(int i=0;i<objects.size();i++) 
	{ 
	    obj*o=objects.at(i);
	    if (dynamic_cast< face*>(o))
	    {
		face*f=as face*>(o);
		f->lock();
	    }
	}
	//logit("done");
    #endif
}
void unlockterms(void)
{
    #ifdef threaded
	//logit("unlocking terms");
	for_each_face
	    f->unlock();
	}}
	//logit("done");
    #endif
}
void loadsettings(void)
{
    tpl_node *tn;
    tn=tpl_map(tpl_settingz, &settingz);
    if(!tpl_load(tn, TPL_FILE, stng))
	tpl_unpack(tn,0);
    tpl_free(tn);
}
void savesettings(void)
{
    tpl_node *tn;
    tn=tpl_map(tpl_settingz, &settingz);
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
void perspmatrix()
{
    #ifdef GL
	glFrustum(-1,1,-1,1,znear,zfar);
	gluLookAt(cam.x,cam.y,cam.z,look.x,look.y,look.z,0,1,0);
	glRotated(cr.x,1,0,0);
	glRotated(cr.y,0,1,0);
	glRotated(cr.z,0,0,1);
    #endif
}


#ifdef GL

obj* pick(int button, int x, int y)
{
    GLuint fuf[500];
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glSelectBuffer(500, fuf);
    glRenderMode (GL_SELECT);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (x,y,10,10, viewport);
    perspmatrix();
    glInitNames();
    glPushName(-1);
    for_each_object
        glLoadName(i);
	o->translate_and_draw(1,1);
    }
    glPopMatrix();
    int i,j, k;
    int numhits = glRenderMode(GL_RENDER);
//    logit("%i\n", numhits);
    for(i=0,k=0;i<numhits;i++)
    {
	GLuint numnames=fuf[k++];
	k++;k++;
	int obj=fuf[k];
	int j = 1;
	k++;
	vector<int> v;
	for(;j<numnames;j++)
	{
	    v.push_back(fuf[k]);
	    //logit("%i\n", n);
	    k++;
	}
	objects.at(obj)->picked(button,v);
	v.clear();
    }
    return active;
}
void vispick()
{
    for_each_object
	o->translate_and_draw(1,0.04);
    }
}
                          
#endif

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
    #ifdef GL
	if(settingz.line_antialiasing)
	    glEnable(GL_LINE_SMOOTH);
	else
    	    glDisable(GL_LINE_SMOOTH);
    #endif
}
void updatelinewidth()
{
    #ifdef GL
	glLineWidth(settingz.lv);
    #endif
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
	

int anything_dirty()
{
    for_each_object
	if(o->getDirty())
	    return 1;
    }
    return 0;
}
int nothing_dirty()
{
    for_each_object
	o->setDirty(0);
    }
}


void showfocus()
{
    #ifdef GL
	if(active)
	{
	    glPushMatrix();
	    glTranslated(active->t.x,active->t.y,active->t.z);
	    glRotated(active->r.x,1,0,0);
	    glRotated(active->r.y,0,1,0);
	    glRotated(active->r.z,0,0,1);
	    glScalef(active->s.x,active->s.y,active->s.z);
	    glColor3f(1,1,1);
	    glBegin(GL_LINE_STRIP);
	    glVertex3f(-1,1,0.2);
	    glVertex3f(-1,1,1);
	    glVertex3f(-1,-1,1);
	    glVertex3f(-1,-1,0.2);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    glVertex3f(1,1,0.2);
	    glVertex3f(1,1,1);
	    glVertex3f(1,-1,1);
	    glVertex3f(1,-1,0.2);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    glVertex3f(-1,1,-0.2);
	    glVertex3f(-1,1,-1);
	    glVertex3f(-1,-1,-1);
	    glVertex3f(-1,-1,-0.2);
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	    glVertex3f(1,1,-0.2);
	    glVertex3f(1,1,-1);
	    glVertex3f(1,-1,-1);
	    glVertex3f(1,-1,-0.2);
	    glEnd();
	    glPopMatrix();
	}
    }
#endif

void moveit(Uint8*k)
{
	if(k[SDLK_F1])
	    if(active&&k[SDLK_RSHIFT])
		active->r.x-=10;
	    else
		cam.x-=0.1;
	if(k[SDLK_F2])
	    if(active&&k[SDLK_RSHIFT])
		active->t.x-=0.1;
	    else
	        look.x-=0.1;
	if(k[SDLK_F3])
	    if(active&&k[SDLK_RSHIFT])
		active->t.x+=0.1;
	    else
		look.x+=0.1;
	if(k[SDLK_F4])
	    if(active&&k[SDLK_RSHIFT])
		active->r.x+=10;
	    else
		cam.x+=0.1;
	if(k[SDLK_F5])
	    if(active&&k[SDLK_RSHIFT])
		active->r.y-=10;
	    else
	        cam.y-=0.1;
	if(k[SDLK_F6])
	    if(active&&k[SDLK_RSHIFT])
		active->t.y-=0.1;
	    else
		look.y-=0.1;
	if(k[SDLK_F7])
	    if(active&&k[SDLK_RSHIFT])
		active->t.y+=0.1;
	    else
	        look.y+=0.1;
	if(k[SDLK_F8])
	    if(active&&k[SDLK_RSHIFT])
		active->r.y+=10;
	    else
		cam.y+=0.1;
	if(k[SDLK_F9])
	    if(active&&k[SDLK_RSHIFT])
		active->r.z-=10;
	    else
		cam.z-=0.1;
	if(k[SDLK_F10])
	    if(active&&k[SDLK_RSHIFT])
		active->t.z-=0.1;
	    else
		look.z-=0.1;
	if(k[SDLK_F11])
	    if(active&&k[SDLK_RSHIFT])
		active->t.z+=0.1;
	    else
	        look.z+=0.1;
	if(k[SDLK_F12])
	    if(active&&k[SDLK_RSHIFT])
		active->r.z+=10;
	    else
		cam.z+=0.1;
}
int RunGLTest (void)
{
    cam.z=2;
    cam.x=cam.y=0;
    int normalize;
    xy ss;
    ss.x=-1;
    {
	char *x=GetFileIntoCharPointer1("mode");
	if(x)
	    ss = parsemodeline(x);
    }
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
    loadfont(fnfl);
    loadcolors();
    #ifdef GL
	resetviewport();
        resetmatrices();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        updatelinewidth();
	updatelinesmooth();
    #endif
//    loadobjects();
    if(!objects.size())
    {
	#ifdef GL
	    objects.push_back(new nerverot);
	    objects.push_back(new spectrum_analyzer);
	    objects.push_back(comp = new composite);
	#endif
	objects.push_back(active=new face("bash"));
	objects.push_back(active=new face("bash",1.0,0.0,3.0,0.0,90.0,0.0));
	objects.push_back(active=new face("bash",0.0,0.0,6.0,0,180.0,0.0));
	objects.push_back(active=new face("bash",-1.0,0.0,3.0,0.0,270.0,0.0));
	objects.push_back(new fontwatcher);
	
    }
    int norm=0;
    int mousemoved=1;
    int lastmousemoved=SDL_GetTicks();
    while( !done )
    {
	int dirty=1;

	lockterms();
	Uint8 * k=SDL_GetKeyState(NULL);
	if(k[SDLK_RCTRL])moveit(k);
	if(dirty|=anything_dirty())
	{

	    nothing_dirty();
	    #ifdef GL
		glClear(GL_COLOR_BUFFER_BIT);
        	glLoadIdentity();
	        perspmatrix();
	        if(SDL_GetMouseState(0,0)||mousemoved)
	    	vispick();
	    #else
		SDL_FillRect    ( s, NULL, SDL_MapRGB( s->format, 0,0,0) );
	    #endif
	    for_each_object
		if(!o->overlay)o->translate_and_draw(0,1);}
	    for_each_object
		if( o->overlay)o->translate_and_draw(0,1);}
	    for_each_face
		if((active==f)&&(!f->t->cursorhidden))
		    f->ghost();}}
	    if((escaped||k[SDLK_RCTRL]))
	    	showfocus();

	    #ifdef GL
		if(settingz.givehelp)
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
        dirty=0;
	unlockterms();
	//logit("---------unlocked wating\n");
	if(SDL_WaitEvent( &event ))
	{
	    if(SDL_GetTicks()-lastmousemoved>300)mousemoved=0;
	    lockterms();
	    //logit("---------locked goooin %i\n", event.type);
	    if(x)SDL_RemoveTimer(x);
	    x=0;
	    int rootdamaged=1;
	    do
	    {
		if(event.type!=SDL_MOUSEMOTION||!SDL_GetMouseState(0,0))
		    dirty=1;
		int key=event.key.keysym.sym;
		int uni=event.key.keysym.unicode;
		int mod=event.key.keysym.mod;
		switch( event.type )
		{
		    case SDL_SYSWMEVENT:
			if(comp)
			    if(event.syswm.msg->event.xevent.type==comp->damage_event)
			    {
				XEvent ee = event.syswm.msg->event.xevent;
				XDamageNotifyEvent* eee = (XDamageNotifyEvent*)&ee;
				if((!eee->more)&&(eee->drawable==comp->root))
				{
				    rootdamaged=1;
				}
			    }
			break;
			
		    case SDL_MOUSEMOTION:
			if(active&&(escaped||k[SDLK_RCTRL])&&(SDL_BUTTON(1)&SDL_GetMouseState(0,0)))
			{
			    escaped=0;
			    active->move(event.motion.xrel/10,event.motion.yrel/10,0);
			}
			mousemoved=1;
		        break;
		    case SDL_MOUSEBUTTONDOWN:
		    {
			pick(event.button.button,event.button.x,event.button.y);
			break;
		    }
		    case SDL_MOUSEBUTTONUP:
		    {
			pick(event.button.button,event.button.x,event.button.y);
			break;
		    }
		    
/*todo:
draw vs show
dbus freezescrollback;

d(WINDOWS) && !defined(OSX)                                                                                              
   framebuffer.h       72                  case SDL_SYSWMEVENT:                                                                                                
      frustum.c           73                                                                                                                                      
         gamewin.c           74                          if(event->syswm.msg->event.xevent.type == SelectionNotify)                                                  
            gamewin.h           75                                  finishpaste(event->syswm.msg->event.xevent.xselection);                                             
               gl_init.c           76                          break;   
    */
		    case SDL_KEYDOWN:
			dirty=1;
			if(escaped||(mod&KMOD_RCTRL))
			{
			    escaped=0;
			    switch (key)
			    {
			        case SDLK_SLASH:
			        {
			    	    buttons *bb;
			    	    objects.push_back( bb=new buttons);
			    	    if(active)
				    {
			    		bb->t.x=active->t.x;
			    		bb->t.y=active->t.y;
			    		bb->t.z=active->t.z;
			    		bb->r.x=active->r.x;
			    		bb->r.y=active->r.y;
			    		bb->r.z=active->r.z;
			    	    }
				    break;
				}
				case SDLK_TAB:
				    for_each_object
					if(active==o)
				        {
					    if(++i==objects.size())
					        i=0;
					    objects.at(i)->activate();
					    break;
					}
				    }
				    if(!active&&objects.size())objects.at(0)->activate();
				    break;
				case SDLK_f:
				    if(mod&KMOD_RSHIFT)
				    {
				        if(as face*>(active))
				        {
					    face *f = as face*>(active);
					    rote_vt_resize(f->t,28,160);
					}
				    }
				    else
					gofullscreen=1;
				    break;
				case SDLK_s:
				    break;
				case SDLK_l:
				    break;
				case SDLK_r:
			    	    loadfont(fnfl);
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
					*/
				case SDLK_p:
				    #ifdef libpng
				        saveScreenshot();
				    #endif
				    break;
				case SDLK_n:
				    objects.push_back(new face);
				    active=objects.at(objects.size()-1);
				    break;
				#ifdef GL
				    case SDLK_t:
					if(is face*>(active))
					{
					    face *f = as face*>(active);
					    int step=1;
					    int c=70;
					    f->obj::t.x=-step;
					    rote_vt_resize(f->t,28,c);
					    objects.push_back(new face(step,0,c,28));
					}
					break;
				    case SDLK_v:
					cr.y+=3.4;
					break;
				    case SDLK_c:
					cr.y-=3.4;
					break;
				    case SDLK_EQUALS:
					{
					if(mod&KMOD_RSHIFT)
					    if(active)
					    {
						active->alpha+=0.05;
						if(active->alpha>1) active->alpha=1;
						break;
					    }
				        settingz.line_antialiasing?settingz.lv+=0.1:settingz.lv++;
				        GLint max=10;
				        if(settingz.line_antialiasing)
					    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,&max);
					else
					    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,&max); 
					if(settingz.lv>max)settingz.lv=max;
					cout << "max:" << max <<endl;
					updatelinewidth();
					break;
					}
				    case SDLK_MINUS:
					if(mod&KMOD_RSHIFT)
					    if(active)
					    {
						active->alpha-=0.05;
						if(active->alpha<0) active->alpha=0;
						break;
					    }
				        settingz.line_antialiasing?settingz.lv-=0.1:settingz.lv--;
				        if(settingz.lv<=0)settingz.lv=settingz.line_antialiasing?0.1:1;
					updatelinewidth();
					break;
				    case SDLK_a:
				        settingz.line_antialiasing=!settingz.line_antialiasing;
				        updatelinesmooth();
				        break;
				#endif
				case SDLK_END:
				    if(active&&is face*>(active))as face*>(active)->resizooo(0,1,k);
				    break;
				case SDLK_HOME:
				    if(active&&is face*>(active))as face*>(active)->resizooo(0,-1,k);
				    break;
				case SDLK_DELETE:
				    if(active&&is face*>(active))as face*>(active)->resizooo(-1,0,k);
				    break;
				case SDLK_PAGEDOWN:
				    if(active&&is face*>(active))as face*>(active)->resizooo(1,0,k);
				    break;
				case 44:
				    znear-=0.2;
				    break;
				case 107:
				    znear+=0.2;
				    break;
				case 105:
				    zfar-=2;
				    break;
				case 56:
				    zfar+=2;
				    break;
			
				default:
				    if(active)
					active->keyp(key,uni,mod);
			    }
			}
			else
			    if(active)
				active->keyp(key,uni,mod);

			if(key==SDLK_RCTRL)
			    escaped=1;
			break;
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
					resetviewport();
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
						    erase(i);
					    }}
					    dirty=1;
					}
					else if(event.user.code==CODE_FNFLCHANGED)
						loadfont(fnfl);
				
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if(rootdamaged)
		    {
			comp->rootdamaged();
		    }
		    if(mousemoved)
		    {
			lastmousemoved=SDL_GetTicks();
			int x,y;
			SDL_GetMouseState(&x,&y);
			pick(0,x,y);
		    }
		    if (mustresize)
		    {
			mustresize=0;
			justresized=1;
		    }
		    if(gofullscreen)
		    {
			if(s->flags & SDL_FULLSCREEN )
			{
			    s=SDL_SetVideoMode( w,h, bpp, (s->flags & ~SDL_FULLSCREEN ));
			    #ifdef linux
			    if(norm&1)
				system("wmctrl -r :ACTIVE: -b remove,maximized_horz");
			    if(norm&2)
				system("wmctrl -r :ACTIVE: -b remove,maximized_vert");
			    #endif
			}
			else
			{
			    #ifdef linux
			    SDL_SysWMinfo i;
			    SDL_VERSION(&i.version)
			    if(SDL_GetWMInfo(&i))
			    {
				char *c=make_message("xprop -id %u |grep _NET_WM_STATE_MAXIMIZED_VERT", i.info.x11.window);
				norm=!system(c);
				free(c);
				c==make_message("xprop -id %u |grep _NET_WM_STATE_MAXIMIZED_HORZ", i.info.x11.window);
				norm&=(!system(c))<1;
				free(c);
			    }
			    system("wmctrl -r :ACTIVE: -b add,maximized_vert,maximized_horz");
			    #endif
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
			updateterminals();
		    }
		    else
		    {
			savemode(w,h,mdfl);
			//savefaces();
		    }
		}

	}
	objects.clear();
	font.clear();
	SDL_Quit( );
	return(0);
}                      

int main(int argc, char *argv[])
{
	logit("hi\n");
	logit("outdated info:right Ctrl+ Home End PgDn Delete to resize, f12 to quit, f9 f10 scale terminal tab to tab thru terminals, \n");
	cout << argv[0] << endl;
	char *path;
	path=getexepath(argv[0]);
	if(path)
	{
		logit("path:%s\n", path);
		path=(char*)realloc(path, 1+strlen(path)+strlen("newtermmsg"));//newtermmsg is the longest string
		char* n=strrchr(path, 0);
		fnfl=strdup(strcat(path, "l1"));		*n=0;
		clfl=strdup(strcat(path, "colors"));		*n=0;
		ntfl=strdup(strcat(path, "newtermmsg"));	*n=0;
		stng=strdup(strcat(path, "settings"));		*n=0;
		mdfl=strdup(strcat(path, "mode"));		*n=0;
		fcfl=strdup(strcat(path, "faces"));		*n=0;
		btns=strdup(strcat(path, "buttons/"));		*n=0;
	}
	loadsettings();
	int i;
	for(i=1;i<argc;i++)
	    if(!strcmp(argv[i],"-f"))
		if(argc>i)
		    fnfl=argv[i+1];
	FILE* f;

	clipout=rote_vt_create(10,10);
	clipout2=rote_vt_create(10,10);
	RunGLTest();
	savesettings();
	rote_vt_destroy(clipout);
	rote_vt_destroy(clipout2);
	logit("finished.bye.\n");
	return 0;
}






