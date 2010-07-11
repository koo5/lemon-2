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
#include "../more-mess/rdfl.c"
#include "SDL_thread.h"
#include "SDL_mutex.h"
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
#include "../roteterm/demo/sdlkeys.c"
#include <iostream>
#include <vector>
#include "../yaml-cpp-0.2.5/include/yaml.h"
#include "serializable.h"
#define _mutexV( d ) {if(SDL_mutexV( d )) {logit("SDL_mutexV!");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {logit("SDL_mutexP!");}}
#define CODE_DATA 3
#define CODE_TIMER 0
#define CODE_QUIT 1
#define CODE_FNFLCHANGED 2
#define as dynamic_cast<
#define is as
#define for_each_object for(unsigned int i=0;i<objects.size();i++) { obj*o=objects.at(i);
#define for_each_face for_each_object if (as face*>(o)){face*f=as face*>(o);
#define for_each_composite_window for_each_object if (as composite_window*>(o)){composite_window*c=as composite_window*>(o);
#define endfor }
using namespace std;
using namespace YAML;



int w = 1280;
int h = 800;

float znear=1;
float zfar=20;
char * originalldpreload=0;
SDL_Surface* s;
#ifdef SDLD
    #include "../sdldlines.c"
#endif

#define SAVE(class) 	YAML_SERIALIZABLE_AUTO(class)\
			void emit_members(Emitter &out)const
#define LOAD	void load_members(const Node& doc)
#define save(x) YAML_EMIT_MEMBER(out, x);
#define load(x) try{YAML_LOAD_MEMBER(doc, x);}catch(...){}
#define vsave(x) YAML_EMIT(out, x);
#define vload(x) try{YAML_LOAD(doc, x);}catch(...){}


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


void slogit(const char * iFormat, ...);
void logit(const char * iFormat, ...);

#include "../gltext.c"
char *fnfl;//font file
char *stng;//settingz
char *mdfl;//modes
char *fcfl;//faces
char *clfl;//colors
char *btns;//buttons/
char *wrld;//world.yaml
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
int max(int a, int b)
{
    return a > b ? a : b;
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

void resetviewport(void)
{
    #ifdef GL
	glViewport( 0, 0, SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h );
    #endif
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
	    else if(gl_error==GL_STACK_OVERFLOW)
		logit("QUACK QUACK QUACK, OVERFLOVING STACK\n");
	    else if(gl_error==GL_INVALID_OPERATION)
		logit("INVALID OPERATION, PATIENT EXPLODED\n");
	    else if(gl_error==GL_INVALID_VALUE)
		logit("GL_INVALID_VALUE");
	    else
		logit("OpenGL error: 0x%X\n", gl_error );
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


struct obj;


void viewmatrix()
{
    #ifdef GL
	glFrustum(-1,1,-1,1,znear,zfar);
	gluLookAt(cam.x,cam.y,cam.z,look.x,look.y,look.z,0,1,0);
	glRotated(cr.x,1,0,0);
	glRotated(cr.y,0,1,0);
	glRotated(cr.z,0,0,1);
    #endif
}


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
    unsigned int runtime_id;
    static unsigned int idcounter;
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
	runtime_id=idcounter++;
    }
    virtual void activate(){
	active=this;
	logit("activating %u", this);
    }
    virtual void picked(int up, int b,vector<int>&v,int x,int y)
    {
	activate();
	logit("activating, up=%i, button=%i, v.size=%u, x=%i. y=%i",up,b,v.size(),x,y);
    }
    virtual void draw(int picking){logit("drawing nothing, %i",picking);};
    virtual int getDirty(){return dirty;}
    virtual void setDirty(int d){dirty=d;}
    void translate_and_draw(int picking)
    {
	#ifdef GL
	    glPushMatrix();
	    glTranslated(t.x,t.y,t.z);
	    glRotated(r.x,1,0,0);
	    glRotated(r.y,0,1,0);
	    glRotated(r.z,0,0,1);
	    glScalef(s.x,s.y,s.z);
	#endif
	if(picking==2)
	    glColor4f(0,0,1,0.04);
	else if(1==picking)
	    glColor4f(1,1,1,1);
	//else define it yourself
	gle();
	draw(picking);
	gle();
	/*picking == 2 == visualizing picking
	picking == 1 == picking*/
	#ifdef GL
	    glPopMatrix();
	#endif
    }
    virtual void keyp(int key,int uni,int mod)
    {                                      
	logit("default keyp handler, %i %i %i",key,uni,mod);
    }
    ~obj(){if(active==this)active=0;}
};
unsigned int obj::idcounter=0;

#ifdef has_pixel_city
#include "../fuzzyflakes-pixel-city-for-lemon/lemon-pixel-city.c"
#endif

struct Settingz: public Serializable
{
    SAVE(Settingz)
    {       
	save(line_antialiasing)
	save(givehelp)
	save(lv)
	vsave(w)
	vsave(h)
	vsave(znear)
	vsave(zfar)
	vsave(cam)
	vsave(look)
	vsave(cr)
#ifdef has_pixel_city
	vsave(pixel_city_ini)
#endif
    }
    LOAD
    {
	load(line_antialiasing)
	load(givehelp)
	load(lv)
	vload(w)
	vload(h)
	vload(znear)
	vload(zfar)
	vload(cam)
	vload(look)
	vload(cr)
#ifdef has_pixel_city
	vload(pixel_city_ini)
#endif
    }
    int32_t line_antialiasing;
    int32_t givehelp;
    double lv;//glLineWidth
    Settingz()
    {
	line_antialiasing=0;
	givehelp=1;
	lv=1;
    }
}settingz;




struct terminal:public obj
{
    char *status;
    RoteTerm *t;
    Uint32 last_resize;
    int lastxresize;
    int lastyresize;
    unsigned int scroll;
    int oldcrow, oldccol;
    Uint32 lastrotor;
    double rotor;
    unsigned int selstartx,selstarty,selendx,selendy;
    int getDirty(){return dirty||t->dirty;}
    void setDirty(int d){t->dirty=d;obj::setDirty(d);}
    SAVE(terminal)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
	int cols, rows;
	cols=t->cols;
	rows=t->rows;
	vsave(cols)
	vsave(rows)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    	int cols=100;
    	int rows=100;
    	vload(cols)
    	vload(rows)
        cout << cols<<" "<<rows<<endl;
	rote_vt_resize(t,rows,cols);
	rote_vt_clear(t);
    }
    void init()
    {
	status=0;
	last_resize=lastxresize=lastyresize=0;
	scroll=0;
	oldcrow=oldccol=-1;
	lastrotor=rotor=0;
	selstartx=selstarty=selendx=selendy=-1;
	s.x=0.002;
	s.y=-0.005;
	s.z=0.002;
	obj::t.x=obj::t.y=obj::t.z=0;
	r.x=r.y=r.z=0;
    }
    terminal()
    {
	init();
    }
    void type(const char * x)
    {
	while(*x)
	{
	    rote_vt_keypress(t,*x);
	    x++;
	}
    }
    ~terminal()
    {
	rote_vt_destroy(t);
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
	    printstatus("%i x %i", t->rows, t->cols);
	}
	lastxresize=xx;
	lastyresize=yy;
	
    }

    void printstatus(const char * iFormat, ...)
    {
	char *s = (char*)malloc(99);
	va_list argp;
	va_start(argp, iFormat);
	vsnprintf(s,99,iFormat, argp);
	s[98]=0;
	va_end(argp);
	updatestatus(s);
    }


    void updatestatus(char *s)
    {
	if(status)free(status);
	status=s;
	dirty=1;
    }

    //FAIL
    void ghost()
    {
	glPushMatrix();
	glTranslated(obj::t.x,obj::t.y,obj::t.z);
	glRotated(r.x,1,0,0);
	glRotated(r.y,0,1,0);
	glRotated(r.z,0,0,1);
	glScalef(s.x,s.y,s.z);
        glTranslatef((t->cols-t->ccol)*26/s.x,(t->rows-t->crow)*26/s.y,0);
	glScalef(30,30,1);
	draw_terminal(1,0.2*alpha);
	glPopMatrix();
    }
	

    void draw(int picking)
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
	{
	    if((active==this)&&!t->cursorhidden)ghost();
    	    draw_terminal();
    	}
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
    	if((mod&KMOD_RSHIFT)&&(key==SDLK_INSERT))
    	{
    	    if(mod&KMOD_RCTRL)
    		clipin(0,0);  //clipboard
    	    else
		clipin(0,1);  //selection buffer
	}
	if(mod&KMOD_RCTRL)
	{	
	    Uint8*k = SDL_GetKeyState(0);
	    switch(key)
	    {
		case SDLK_END:
		    resizooo(0,1,k);
		    break;
		case SDLK_HOME:
		    resizooo(0,-1,k);
		    break;
		case SDLK_DELETE:
		    resizooo(-1,0,k);
		    break;
		case SDLK_PAGEDOWN:
		    resizooo(1,0,k);
	    }
	    return;
	}
	else if(mod&KMOD_RSHIFT&&(key==SDLK_HOME||key==SDLK_END||key==SDLK_PAGEUP||key==SDLK_PAGEDOWN))
	{
	    if(key==SDLK_PAGEUP)
		scroll+=9;
	    if(key==SDLK_PAGEDOWN)
	    	scroll=max(scroll-9,0);
	    if(key==SDLK_END)
		scroll=0;
	    if(key==SDLK_HOME)
		scroll=t->logl;
	}
	else
	{
	    if((key!=SDLK_LALT)&&(key!=SDLK_RSHIFT))
	    {
	        scroll=0;
	        t->stoppedscrollback=0;
	    }
    	    sdlkeys(t,key,uni,mod);
	}
    }
    void draw_terminal(int theme=0, double alpha=1)
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
	    for (i=t->logl-s;i<(int)t->logl;i++)
	    {
	        if(!t->log[i])break;
	        lok.y=((i-t->rows/2.0)-t->logl+s)*26;
	        if(t->logstart) lok.y-=100;
		if(t->logstart) lok.x-=100;
		for(j=0;j<(int)t->log[i][0].ch;j++)//len
	        {
		    lok.x=(j-t->cols/2.0)*26;
		    do_color(theme,ROTE_ATTR_XFG(t->log[i][j+1].attr),alpha);
	    	    drawmonospace(lok,t->log[i][j+1].ch);
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
		isundercursor=(!t->cursorhidden)&&((t->ccol==(int)j)&&(t->crow==(int)i));
		#ifdef GL
		    if(isundercursor||((int)selstartx<=j&&(int)selstarty<=i&&(int)selendx>=j&&(int)selendy>=i))
		    {
			if((oldcrow!=(int)t->crow)||(oldccol!=(int)t->ccol))
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
			drawmonospace(molok,t->cells[i][j].ch);
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
		    drawmonospace(lok,t->cells[i][j].ch);
	    }
	}
	#ifdef GL
	    oldcrow=t->crow;//4 cursor rotation
	    oldccol=t->ccol;
	    glEnd();

	    if(status)
	    {
		glPushMatrix();
		glTranslatef(-t->cols/2.0*26,(s+((1+t->rows)/2.0))*26,0);
		glColor4f(0,1,0,alpha);
		draw_text(status);
		glPopMatrix();
	    }

	#endif
    }
};
#include "face.cpp"
#include "logger.cpp"
logger * loggerface;
void slogit(const char * iFormat, ...)
{
    char* s=(char*)malloc(101);
    va_list argp;
    va_start(argp, iFormat);
    vsnprintf(s,101,iFormat, argp);
    s[100]=0;
    va_end(argp);
    if(loggerface)
	loggerface->slogit(s);
}

void logit(const char * iFormat, ...)
{
    char* s=(char*)malloc(10101010);
    va_list argp;
    va_start(argp, iFormat);
    vsnprintf(s,10101010,iFormat, argp);
    s[10101009]=0;
    va_end(argp);
    if(loggerface)
	loggerface->logit(s);
    else
	printf("%s\n",s);
    free(s);
}

float maxvol;


#include "../toys/atlantis/atlantis.c"
#include "../toys/flipflop.c"



#ifdef GL
    struct spectrum_analyzer:public obj
    {
	SAVE(spectrum_analyzer)
	{
	    YAML_EMIT_PARENT_MEMBERS(out,obj)
	    save(rotx)
	    save(roty)
	    save(rotz)
	    save(rx)
	    save(ry)
	    save(rz)
	}
	LOAD
	{
	    YAML_LOAD_PARENT_MEMBERS(doc,obj)
	    try{
	    load(rotx)
	    load(roty)
	    load(rotz)
	    load(rx)
	    load(ry)
	    load(rz)
	    }catch(Exception){}
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
	void picked(int up, int  b,vector<int>&v,int x, int y)
	{
	    if(!up)
	    {
		if(b==SDL_BUTTON_LEFT)
		    ry-=2;
		else if (b==SDL_BUTTON_RIGHT)
		    ry+=2;
		else if(b==SDL_BUTTON_MIDDLE)
		    rx=0;
		else
		    obj::picked(up,b,v,x,y);
	    }
	}
	void keyp(int key,int uni,int mod)
	{
	    switch(key)
	    {
		case SDLK_UP:
		    rx-=0.2;
		    break;
		case SDLK_DOWN:
		    rx+=0.2;
		    break;
		case SDLK_LEFT:
		    rz-=0.2;
		    break;
		case SDLK_RIGHT:
		    rz+=0.2;
		    break;
		default:
		    obj::keyp(key,uni,mod);
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
	
	void draw(int picking)
	{
	
	    FILE *f;
	    if((f=fopen("/tmp/somefunnyname", "r")))
	    {
	    	int16_t buf[2][256];
		fread(buf,256,2,f);
    		fclose(f);
		remove("/tmp/somefunnyname");
		oglspectrum_gen_heights(buf);
	    }

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
    	    float oldalpha=alpha;
    	    if(picking==1)
    		alpha=1;
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
	    alpha=oldalpha;
	    glEnd();
	    glPopMatrix();
	    
	    float sx=fmax(fmax(fmax(fmax(spectrum_analyzer::heights[0][0],spectrum_analyzer::heights[0][1]),spectrum_analyzer::heights[0][2]),spectrum_analyzer::heights[0][3]),spectrum_analyzer::heights[0][4]);
	    float sy=fmax(sx,fmax(fmax(fmax(fmax(spectrum_analyzer::heights[0][5],spectrum_analyzer::heights[0][6]),spectrum_analyzer::heights[0][7]),spectrum_analyzer::heights[0][8]),spectrum_analyzer::heights[0][9]));
	    maxvol=fmax(sy,fmax(fmax(fmax(fmax(fmax(spectrum_analyzer::heights[0][10],spectrum_analyzer::heights[0][11]),spectrum_analyzer::heights[0][12]),spectrum_analyzer::heights[0][13]),spectrum_analyzer::heights[0][14]),spectrum_analyzer::heights[0][15]));
//	    logit("maxvol:%f",maxvol);

	}
    };
    GLfloat spectrum_analyzer::heights[16][16];

#ifdef nerve
    GLfloat fmin(GLfloat a, GLfloat b)
    {
	return a>b?b:a;
    }
    GLfloat fmax(GLfloat a, GLfloat b)
    {
	return a<b?b:a;
    }
    class nerverot:public obj
    {
        public:
        int band;
	SAVE(nerverot)
	{
	    YAML_EMIT_PARENT_MEMBERS(out,obj)
	    save(nerv->please_num)
	    save(band)
	}
	LOAD
	{
	    YAML_LOAD_PARENT_MEMBERS(doc,obj)
	    try
	    {
		load(nerv->please_num)
		load(band)
	    }catch(Exception){}
	}
        void draw(int picking)
        {
	    glPushMatrix();
	    if(band==-2)
	    {
	    
	    }
	    else if(band==-1)
	    {
		glScalef(1+maxvol,1+maxvol,1);
	    }
	    else
		glScalef(1+spectrum_analyzer::heights[0][band],1+spectrum_analyzer::heights[0][band],1+spectrum_analyzer::heights[0][band]);
	    
    	    if(picking)
    		gluSphere(gluNewQuadric(),1,10,10);
    	    else
    	    {
        	nerverot_update(nerv);
        	nerverot_draw(3,nerv,alpha);
    	    }
    	    glPopMatrix();
        }
	nerverot(float x=0,float y=0,float z=0,int b = -1)
	{
	    nerv=nerverot_init(SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h);
	    t.x=x;t.y=y;t.z=z;
	    alpha=0.5;
	    band=b;
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
		default:
		    obj::keyp(key,uni,mod);
	    }
	    
	}
	void picked(int up, int b,vector<int>&v,int x, int y)
	{
	    if(!up)
		switch(b)
		{
		    case SDL_BUTTON_LEFT:
			nerverot_cycleup(nerv);
			break;
		    case SDL_BUTTON_RIGHT:
		        nerverot_cycledown(nerv);
		        break;
		    case SDL_BUTTON_MIDDLE:
		    default:
		        obj::picked(up,b,v,x,y);
		}
	}
	int getDirty(){return 1;}
	protected:
        struct nerverotstate *nerv;

    };
#endif
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
	    default:
	    obj::keyp(key,uni,mod);
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

static int _XPrintDefaultError(
    Display *dpy,
    XErrorEvent *event)
{
    char buffer[BUFSIZ];
    char mesg[BUFSIZ];
    char number[32];
    const char *mtype = "XlibMessage";
    XGetErrorText(dpy, event->error_code, buffer, BUFSIZ);
    XGetErrorDatabaseText(dpy, mtype, "XError", "X Error", mesg, BUFSIZ);
    slogit("%s:  %s\n  ", mesg, buffer);
    XGetErrorDatabaseText(dpy, mtype, "MajorCode", "Request Major code %d",
	mesg, BUFSIZ);
    slogit(mesg, event->request_code);
    if (event->request_code < 128) {
	sprintf(number, "%d", event->request_code);
	XGetErrorDatabaseText(dpy, "XRequest", number, "", buffer, BUFSIZ);
    } 
    slogit( " (%s)\n", buffer);
    if (event->request_code >= 128) {
	XGetErrorDatabaseText(dpy, mtype, "MinorCode", "Request Minor code %d",
			      mesg, BUFSIZ);
	slogit( mesg, event->minor_code);
    }
    if ((event->error_code == BadWindow) ||
	       (event->error_code == BadPixmap) ||
	       (event->error_code == BadCursor) ||
	       (event->error_code == BadFont) ||
	       (event->error_code == BadDrawable) ||
	       (event->error_code == BadColor) ||
	       (event->error_code == BadGC) ||
	       (event->error_code == BadIDChoice) ||
	       (event->error_code == BadValue) ||
	       (event->error_code == BadAtom)) {
	if (event->error_code == BadValue)
	    XGetErrorDatabaseText(dpy, mtype, "Value", "Value 0x%x",
				  mesg, BUFSIZ);
	else if (event->error_code == BadAtom)
	    XGetErrorDatabaseText(dpy, mtype, "AtomID", "AtomID 0x%x",
				  mesg, BUFSIZ);
	else
	    XGetErrorDatabaseText(dpy, mtype, "ResourceID", "ResourceID 0x%x",
				  mesg, BUFSIZ);
	slogit( mesg, event->resourceid);
    }
    XGetErrorDatabaseText(dpy, mtype, "ErrorSerial", "Error Serial #%d",
			  mesg, BUFSIZ);
    slogit( mesg, event->serial);
    if (event->error_code == BadImplementation) return 0;
    return 1;
}
unsigned int rw,rh;
float cellx,celly;
GLuint dlist;
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
    int zerotoone(double x)
    {
	return ((x>=0)&&(x<=1));
    }
    int _isvisible(double x, double y, double z)
    {
	GLint viewport[4]={0,0,1,1};
	GLdouble winx, winy, winz;// Space For Returned Projected Coords
        GLdouble mvmatrix[16], projmatrix[16];// Space For Transform Matrix
        glGetDoublev (GL_MODELVIEW_MATRIX, (GLdouble*)&mvmatrix);// Get Actual Model View Matrix
        glGetDoublev (GL_PROJECTION_MATRIX,  (GLdouble*)&projmatrix);// Get Actual Projection Matrix
	gluProject(x,y,z,mvmatrix,projmatrix,viewport,&winx,&winy,&winz);
	return(zerotoone(winx)&&zerotoone(winy)&&zerotoone(winz));
    }
    int isvisible()
    {
	return 	 _isvisible(-(float)width/(float)rw,(float)height/(float)rh,0)||
		 _isvisible(-(float)width/(float)rw,-(float)height/(float)rh,0)||
	     	 _isvisible((float)width/(float)rw,-(float)height/(float)rh,0)||
	      	 _isvisible(-(float)width/(float)rw,(float)height/(float)rh,0)||
	       	 _isvisible(-(float)width/(float)rw,0,0)||
	       	 _isvisible((float)width/(float)rw,0,0)||
	       	 _isvisible(0,(float)height/(float)rh,0)||
	       	 _isvisible(0,-(float)height/(float)rh,0)||
	       	 _isvisible(0,0,0);
	       	 
    }
    void picked(int up, int  b,vector<int>&v,int x, int y)
    {
	obj::picked(up,b,v,x,y);
	logit("picking...");
	GLuint fuf[500];
	GLint viewport[4];
	glGetIntegerv (GL_VIEWPORT, viewport);
	glSelectBuffer(500, fuf);
        glRenderMode (GL_SELECT);
        glPushMatrix ();
        glLoadIdentity ();
        gluPickMatrix (x,y,1,1, viewport);
        viewmatrix();
	glInitNames();
	glCallList(dlist);
	glPopMatrix();
	int numhits;
	gle();
	logit("%i hits",numhits=glRenderMode(GL_RENDER));
	if(!numhits)return;
	if(fuf[0]<2)return;
	xx=fuf[3];
	yy=fuf[4];

        glSelectBuffer(500, fuf);
        glRenderMode (GL_SELECT);
        glPushMatrix ();
        glLoadIdentity ();
        gluPickMatrix (x,y,3,3, viewport);
        viewmatrix();
        glTranslatef(2/cellx*xx-1,2/celly*yy-1,0);
        glScalef(2/cellx, 2/celly,1);
	glCallList(dlist);
	glPopMatrix();
	logit("%i hits",numhits=glRenderMode(GL_RENDER));
	if(!numhits)return;
	if(fuf[0]>2)return;
	int xxx=fuf[3];
	int yyy=fuf[4];
	mousex=(float)xx*cellx+xxx;
	mousey=-(yy*celly+yyy);
	logit("xx:%i yy:%i xxx:%i yyy:%i mousex:%i mousey:%i",xx,yy,xxx,yyy,mousex,mousey);
    }
    int xx,yy;
    int mousex,mousey;
    GLuint texture;
    Display *dpy;
    Window window;
    int X,Y;
    int needsreconf;
    XShmSegmentInfo shminfo;
    int border_width;
    int pictaken;
    void reconfigure()
    {
	unsigned int Z;
	Window programming;
	//real
	//BAD
	XGetGeometry(dpy, window, &programming,&X,&Y,&width,&height,&Z,&Z);
//	cout << width << "x" << height << "at"<<X<<","<<Y<<endl;
	XWindowAttributes attr;
	XGetWindowAttributes(dpy,window,&attr);
	border_width=attr.border_width;
	needsreconf=0;
    }
    composite_window(double x,double sc, Display *dpy,Window id,int scr)
    {
	gc=0;
	t.x=x;
	s.x=sc;
	xim=0;
	window=id;
	pictaken=0;
	this->dpy=dpy;
	mousex=mousey=50;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
	damage=XDamageCreate(dpy,window,XDamageReportRawRectangles);
	reconfigure();
	Visual* visual = DefaultVisual(dpy, scr);

	xim=XShmCreateImage(dpy, visual, DefaultDepth(dpy,scr), ZPixmap, NULL, &shminfo, width, height);
	shminfo.shmid = shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height, IPC_CREAT | 0777);
	shminfo.shmaddr = xim->data = (char*)shmat(shminfo.shmid, NULL, 0);
	shmctl(shminfo.shmid, IPC_RMID, NULL);
	shminfo.readOnly = False;
	if (!XShmAttach(dpy, &shminfo))
	    logit("cannot use the shared memory segment .. :( \n");
	else
	    logit("can use share segment :D\n");
	XSync(dpy, False);
    }
    XImage *xim;
    Damage damage;
    int damaged;
    GC gc;
    Pixmap pix;
    unsigned int width, height;
    ~composite_window()
    {
	XDamageDestroy(dpy, damage);
    }
    void draw(int picking)
    {
//        if(needsreconf)
    	    reconfigure();
//	cout << window << endl;
	if(!picking)
	{
	    if(!isvisible())
	        return;

	    glEnable(GL_TEXTURE_2D);
	    glBindTexture(GL_TEXTURE_2D, texture);
    	    if(damaged)
	    {
		if(Success==XShmGetImage(dpy, window, xim, 0,0, AllPlanes))
		    pictaken=1;
		glTexImage2D(GL_TEXTURE_2D,0,4,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,xim->data);
	    }
	    damaged=0;
	    glColor4f(1,1,1,alpha);
	}
	
	glBegin(GL_QUADS);
	if(!picking)glTexCoord2f(0,0);	glVertex2f(-(float)width/(float)rw,(float)height/(float)rh);
	if(!picking)glTexCoord2f(1,0);	glVertex2f((float)width/(float)rw,(float)height/(float)rh);
	if(!picking)glTexCoord2f(1,1);	glVertex2f((float)width/(float)rw,-(float)height/(float)rh);
	if(!picking)glTexCoord2f(0,1);	glVertex2f(-(float)width/(float)rw,-(float)height/(float)rh);
	glEnd();
	
	
	{
	glColor4f(0,1,0,0.3);
        glPushMatrix ();
        glLoadIdentity ();
        viewmatrix();
	glCallList(dlist);
	glPopMatrix();
        glPushMatrix ();
        glLoadIdentity ();
        viewmatrix();
        glTranslatef(2/cellx*xx-1,2/celly*yy-1,0);
        glScalef(2/cellx, 2/celly,1);
	glColor4f(1,0,0,0.3);
	glCallList(dlist);
	glPopMatrix();
        }
	
        if(picking)
    	    return;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	{
	    glColor3f(0,0,0.2);
	    glVertex2f(-(float)width/(float)rw,((float)mousey/(float)rh*2.0-1.0));
	    glVertex2f((float)width/(float)rw,((float)mousey/(float)rh*2.0-1.0));
	    glVertex2f((float)mousex/(float)rw*2.0-1.0,(float)height/(float)rh);
	    glVertex2f((float)mousex/(float)rw*2.0-1.0,-(float)height/(float)rh);
	    glVertex3f((float)mousex/(float)rw*2.0-1.0,((float)mousey/(float)rh*2.0-1.0),-1);
	    glVertex3f((float)mousex/(float)rw*2.0-1.0,((float)mousey/(float)rh*2.0-1.0),+1);
	}
	glEnd();

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

    int scr;
    char * data;
    Window root;
    Pixmap windowPix;
//    XRenderPictureAttributes<pa;
    Display *dpy;
    XShmSegmentInfo shm;
    Atom a;
    void genlists()
    {
	cellx=sqrt(rw);
	celly=sqrt(rh);
    	dlist = glGenLists(1);
	glNewList(dlist,GL_COMPILE);
	glPushName(0);
	int nx=0;
	int ny=0;
	for(float x=-1;x<1;x+=cellx)
	{
	    glLoadName(nx++);
	    glPushName(0);
	    for(float y=-1;y<1;y+=celly)
	    {
		glLoadName(ny++);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x+2/cellx, y);
		glVertex2f(x+2/cellx, y+2/celly);
		glVertex2f(x, y+2/celly);
		glEnd();
	    }
	    ny=0;
	    glPopName();
	}
	glPopName();
	glEndList();
    }
    void regetwindows()
    {
    	unsigned long nitems=0;
	unsigned long bytes_after;
	unsigned int *prop;
	Atom aa;
	int af;
	int max_len = 10000;
	double x=-12.5;
//	objects.push_back(new composite_window(x+=2.5,0.8, dpy, root,scr));	
	if(XGetWindowProperty(dpy, root, a, 0, (max_len+3)/4,0,AnyPropertyType, &aa, &af, &nitems, &bytes_after, (unsigned char**)&prop)==Success)
	{
            for(unsigned int j=0;j<nitems;j++)
	    {
		//cout <<prop[j]<<"::"<<endl;
		if(prop[j])
		{
		    int found = 0;
		    for_each_composite_window
			if(prop[j]==c->window)
			    found=1;
		    endfor endfor
		    if(!found)
		    {
			XWindowAttributes attr;
	    		if(XGetWindowAttributes(dpy,prop[j],&attr))
	    		{
	    		    if((attr.c_class==InputOutput)&&(attr.map_state==IsViewable))
	            		objects.push_back(new composite_window(x+=2.5,0.8, dpy, prop[j],scr));
			}
		    }
		}
	    }
	}
    }
    composite()
    {
        SDL_SysWMinfo i;
        SDL_VERSION(&i.version)
        if(SDL_GetWMInfo(&i))
        {
    	    logit("d:%u",i.info.x11.display);
    	    logit("gfxd:%u",i.info.x11.gfxdisplay);
    	    dpy=i.info.x11.display;
    	    SDL_EventState(SDL_SYSWMEVENT,SDL_ENABLE);
    	    scr=DefaultScreen(dpy);
    	    root=RootWindow(dpy,scr);
    	    rw=DisplayWidth(dpy,scr);
    	    rh=DisplayHeight(dpy,scr);
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
				XGrabServer (dpy);
				XCompositeRedirectSubwindows (dpy, root, CompositeRedirectAutomatic);
				XUngrabServer (dpy);
				logit("HAPPY");
				XWindowAttributes attr;
				XGetWindowAttributes( dpy, root, &attr );
				if(XShmQueryExtension(dpy))
				{
				    int shm_pixmaps;
				    int shm_major;
				    int shm_minor;
				    if(XShmQueryVersion(dpy,&shm_major,&shm_minor,&shm_pixmaps))
				    {
				        logit("HAPPY");
//					if(shm_pixmaps)
//					{
//					    logit("HAPPY");
//					    if((shm.shmid = shmget (IPC_PRIVATE, SHM_SIZE, IPC_CREAT | 0600))>=0)
//					    {
//						logit("HAPPY");
//						shm.shmaddr = (char *) shmat (shm.shmid, 0, 0);
//					    }
//					}
				    }
				} 
				//XFlush(dpy);
				//windowPix = XCompositeNameWindowPixmap(dpy, root);
				
					
  //				XShmGetImage(dpy, root,xim,0,0,AllPlanes);
//				XQueryTree(dpy, window, &root_win, &parent_win, &child_list,
//				<------><------>  &num_children))
				//if(hasNamePixmap)
				  //  windowPix = XCompositeNameWindowPixmap( dpy, root );
				//objects.push_back(new composite_window(dpy, root));
				
				
				XSetErrorHandler(_XPrintDefaultError);
				
				
				XSelectInput(dpy, root, SubstructureNotifyMask);
				a = XInternAtom(dpy,"_NET_CLIENT_LIST",1);
				regetwindows();
				/*unsigned int nchildren;
				Window*children;
				Window parent;
				Window rootret;
				int numprop;
				
				
				
				
				if(XQueryTree(dpy, root, &rootret, &parent, &children, &nchildren))
				{
				    for(int i=0;i<nchildren;i++)
				    {
					Window *children2;
					unsigned int nchildren2;                                                   
					if(XQueryTree(dpy, *children,&rootret, &parent, &children2, &nchildren2))
					{
					    if(parent==root)
					    {
						cout<<*children<<":";
						//objects.push_back(new composite_window(dpy, *children));
	unsigned int X;
	Window programming;
	int sucks;
	//real
	//BAD
	unsigned int width,height;
	XGetGeometry(dpy, *children, &programming,&sucks,&sucks,&width,&height,&X,&X);
	cout << width << "x" << height << endl;

					    }
					}
					children++;
				    }
				} */
				genlists();

			    }
			}
		    }
		}
	    }
    	}
    }
};

composite *comp;

#ifdef fontwatcher
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
    void draw( int picking)
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
    void picked(int b,vector<int>&v, int x, int y)
    {
	loadfont(fnfl);
    }
};
int fontwatcherthread(void *data)
{
    while(1)
    {
	char buf[1000];
//	cout << reinterpret_cast<fontwatcher*>(data)->i << endl;
	read(reinterpret_cast<fontwatcher*>(data)->i,&buf,1000);
	//todo:check it wasnt "ignore"
	reinterpret_cast<fontwatcher*>(data)->grow=0.1;
        SDL_Event e;
        e.type=SDL_USEREVENT;
        e.user.code=CODE_FNFLCHANGED;
        SDL_PushEvent(&e);
        cout << "reloading " << endl;
    }
}
#endif
void add_button(char *path, char *justname, void *data);


void listdir(char *path, void func(char *, char *, void* ), void *data)
{
	DIR *dir = opendir(path);
	if(dir)
	{	
		path=strdup(path);
		unsigned int maxsize = 50;
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
    for(unsigned int i=0;i<objects.size();i++)
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
    void show_button(int x, int y, button *b, int picking)
    {
//	cout << b->content << endl;
	glPushMatrix();
	glTranslatef(x,y,0);
	if(picking)
	    glBegin(GL_QUADS);
	else
	{
	    glBegin(GL_LINE_LOOP);
	    glColor4f(1,1,0,alpha);
	}
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
    void draw(int picking)
    {
	int n=buttonz.size();
	int y=0;
///	cout <<"+++"<<endl;
	while(n)
	{
		if(picking)glLoadName(n-1);
		show_button(-100,y+=100, &buttonz.at(--n),picking);
	}
//	glLoadName(-1);
    }
    void picked(int up, int b, vector<int> &v, int x, int y)
    {
	if(v.size()&&!up&&b&&is face*>(active))
	{
	    logit("%i %i",x,y);
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
	for_each_face
	    f->lock();
	endfor endfor
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

#define loado(y,x) if(!strcmp(n , #x )) {\
if(online&&hasid) {\
 for_each_object\
  if(o->runtime_id==runtimeid)\
   if(is x*>(o))\
    o->load_members(doc[i]); \
 endfor }\
else\
 {  obj*o;y=as x*>(o=new x); o->load_members(doc[i]); objects.push_back(o); }}

void loadobjects(int online=0)
{
    std::ifstream fin(wrld);
    YAML::Parser parser(fin);
    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        for(unsigned int i=0;i<doc.size();i++) {
            string nn;
            doc[i]["Class"]>>nn;
            const char *n=nn.c_str();
            unsigned int runtimeid=0;
            int hasid=0;
            if(online)
        	try
        	{
	    	    runtimeid = doc[i]["runtime_id"];
	    	    hasid=1;
	    	}
	    	catch(Exception){}
            obj * d;//dummy
	    loado(d,face)
	    #ifdef has_atlantis
            loado(d,atlantis)
	    #endif
	    #ifdef has_pixel_city
	    loado(d,pixel_city)
	    #endif
	    loado(comp,composite)
	    loado(loggerface,logger)
	    loado(d,flipflop)
	    loado(d,nerverot)
	    loado(d,spectrum_analyzer)
        }
    }
}
void loadsettingz()
{
    std::ifstream fin(wrld);
    YAML::Parser parser(fin);
    YAML::Node doc;
    while(parser.GetNextDocument(doc)) {
        for(unsigned int i=0;i<doc.size();i++) {
            string nn;
            doc[i]["Class"]>>nn;
            const char *n=nn.c_str();
	    if(!strcmp(n, "Settingz"))
		doc[i]>>settingz;
        }
    }
}

void saveobjects(int online=0)
{
    ofstream fout(wrld);
    Emitter out;
    out << BeginSeq;
    for_each_object
	if(!is composite_window*>(o))
	{
	    if(online)
	    {
		out << YAML::BeginMap;
	        std::string my_class_name = o->class_name();
	        if (my_class_name.size()>0) {
	          out << YAML::Key << CLASS_TAG << YAML::Value << my_class_name;
	        }
	        out<<Key<<"runtime_id"<<Value<<o->runtime_id;
	        o->emit_members(out);
		out << YAML::EndMap;
	    }
	    else
		out << *o;
	}
    endfor
    out << settingz;
    out << EndSeq;
    fout << out.c_str();
    logit("%s",out.c_str());
}


#ifdef GL
#include <limits>

void pick(int up, int button, int x, int y)
{         
    logit("picking objects...");
    y=h-y;
    GLuint fuf[500];
    GLint viewport[4];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glSelectBuffer(500, fuf);
    glRenderMode (GL_SELECT);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    gluPickMatrix (x,y,10,10, viewport);
    viewmatrix();
    glInitNames();
    glPushName(-1);
    for_each_object
        glLoadName(i);
	o->translate_and_draw(1);
    endfor
    glPopMatrix();
    unsigned int i,j, k;
    GLuint minz = std::numeric_limits<unsigned int>::max() ;
    int nearest=-1;
    unsigned int numhits = glRenderMode(GL_RENDER);
    logit("%i hits", numhits);
    vector<vector<int> > hits;
    for(i=0,k=0;i<numhits;i++)
    {
	GLuint numnames=fuf[k++];
	logit("%i names", numnames);
	logit("%d minz", fuf[k]);
	logit("%d maxz", fuf[k+1]);
	if(fuf[k]<minz)
	{
	    logit("%u < %u, nearest = %i", fuf[k],minz,i);
	    nearest = i;
	    minz = fuf[k];
	}
	k+=2;
	vector<int> v;
	hits.push_back(v);
	for(j=0;j<numnames;j++)
	{
	    hits[hits.size()-1].push_back(fuf[k]);
	    k++;
	}
    }
    if(nearest != -1)
    {
        if(hits.at(nearest).size())
        {
	    obj *o=objects.at(hits.at(nearest)[0]);
	    hits.at(nearest).erase(hits.at(nearest).begin());
	    //lets send the name stack, without the first hit, which identifies object
	    o->picked(up, button,hits.at(nearest),x,y);
	}
    }

}
void vispick()
{
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();
    viewmatrix();

    for_each_object
	o->translate_and_draw(2);
    endfor
    glPopMatrix();
}
                          
#endif

Uint32 TimerCallback(Uint32 interval, void *param)
{
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=CODE_TIMER;
	e.user.data1=param;
	SDL_PushEvent(&e);
	return interval;
}

void updatelinesmooth()
{
    #ifdef GL
	if(settingz.line_antialiasing){
	    logit("antialiasing");
	    glEnable(GL_LINE_SMOOTH);
	}
	else
	{
	    logit("not antialiasing");
    	    glDisable(GL_LINE_SMOOTH);
    	}
    #endif
}
void updatelinewidth()
{
    #ifdef GL
	glLineWidth(settingz.lv);
    #endif
}


int anything_dirty()
{
    for_each_object
	if(o->getDirty())
	    return 1;
    }
    return 0;
}
void  nothing_dirty()
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
//	    glScalef(active->s.x,active->s.y,active->s.z);
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
    #endif
}


void moveit(Uint8*k)
{
	if(k[SDLK_F1])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.x-=10;
	    else
		cam.x-=0.1;
	}
	if(k[SDLK_F2])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.x-=0.1;
	    else
	        look.x-=0.1;
	}
	if(k[SDLK_F3])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.x+=0.1;
	    else
		look.x+=0.1;
	}
	if(k[SDLK_F4])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.x+=10;
	    else
		cam.x+=0.1;
	}
	if(k[SDLK_F5])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.y-=10;
	    else
	        cam.y-=0.1;
	}
	if(k[SDLK_F6])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.y-=0.1;
	    else
		look.y-=0.1;
	}
	if(k[SDLK_F7])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.y+=0.1;
	    else
	        look.y+=0.1;
	}
	if(k[SDLK_F8])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.y+=10;
	    else
		cam.y+=0.1;
	}
	if(k[SDLK_F9])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.z-=10;
	    else
		cam.z-=0.1;
	}
	if(k[SDLK_F10])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.z-=0.1;
	    else
		look.z-=0.1;
	}
	if(k[SDLK_F11])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->t.z+=0.1;
	    else
	        look.z+=0.1;
	}
	if(k[SDLK_F12])
	{
	    if(active&&k[SDLK_RSHIFT])
		active->r.z+=10;
	    else
		cam.z+=0.1;
	}
		
//	int x,y;
	//pick(0,SDL_GetMouseState(&x,&y),x,y);
}
void lemon (void)
{
    int bpp=8;
    int done = 0;
    int gofullscreen=0;
    int escaped = 0;
    cam.z=10;
    cam.x=cam.y=0;
    loadsettingz();
    #ifdef GL
	s=initsdl(w,h,&bpp,SDL_OPENGL
    #else
	s=initsdl(w,h,&bpp,0
    #endif
    );
    SDL_EnableUNICODE(1);
    SDL_InitSubSystem( SDL_INIT_TIMER);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2, SDL_DEFAULT_REPEAT_INTERVAL*2);
    loadfont(fnfl);
    loadcolors();
    #ifdef GL
	gle();
	resetviewport();
        resetmatrices();
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glClearColor( 0.0, 0.0, 0.0, 0.0 );
        updatelinewidth();
	updatelinesmooth();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    #endif
    loadobjects();
//    objects.push_back(new nerverot);
    if(!objects.size())
    {
    	objects.push_back(loggerface=new logger(-8,0,0,0,70,0));
	#ifdef GL
//	    objects.push_back(new spectrum_analyzer);
//	    for(int i=0;i<16;i++)
//		objects.push_back(new nerverot(-10.0f+20.0f/16.0f*(float)i,0,0,i));
//	    objects.push_back(new flipflop);
	objects.push_back(new spectrum_analyzer);

	    objects.push_back(comp = new composite);
	#endif
	objects.push_back(active=new face("bash"));
#ifdef has_atlantis
        objects.push_back(new atlantis);
#endif

//	objects.push_back(active=new face("bash",1.0,0.0,3.0,0.0,90.0,0.0));
//	objects.push_back(active=new face("bash",0.0,0.0,6.0,0,180.0,0.0));
//	objects.push_back(active=new face("bash",-1.0,0.0,3.0,0.0,270.0,0.0));
//	objects.push_back(new fontwatcher);
	
    }
    maxvol=0;
    int norm=0;
    int mousemoved=1;
    int mousejustmoved=0;
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
                gle();
        	glLoadIdentity();
        	gle();
	        viewmatrix();
	        gle();
	        if(SDL_GetMouseState(0,0)||mousemoved)
	    	    vispick();
	    	gle();
	    #else
		SDL_FillRect    ( s, NULL, SDL_MapRGB( s->format, 0,0,0) );
	    #endif
	    for_each_object
		if(!o->overlay)o->translate_and_draw(0);}
	    for_each_object
		if( o->overlay)o->translate_and_draw(0);}
	    gle();
	    if((escaped||k[SDLK_RCTRL]))
	    	showfocus();

	    #ifdef GL
	    gle();
		if(settingz.givehelp)
		{	
		    glPushMatrix();
		    glLoadIdentity();
	    	    glOrtho(0,1200,1200,0,0,1);
		    if(!(escaped||k[SDLK_RCTRL]))
		    
			draw_text("\npress right ctrl for more fun...");
		    else
			draw_text("\nnow press f12 to zoom out,\nf9 to zoom in\ndel end home and pgdn to resize terminal...\nh to hide help");
		    glPopMatrix();
		}
		SDL_GL_SwapBuffers( );
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
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
		x= SDL_AddTimer(5, TimerCallback, 0);
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
	    do
	    {
		if(event.type!=SDL_MOUSEMOTION||!SDL_GetMouseState(0,0))
		    dirty=1;
		switch( event.type )
		{
		    case SDL_SYSWMEVENT:

			if(comp)
			{
			
			    if(event.syswm.msg->event.xevent.type==comp->damage_event)
			    {
				
				XEvent ee = event.syswm.msg->event.xevent;
				XDamageNotifyEvent* eee = (XDamageNotifyEvent*)&ee;
				for(unsigned int i=0;i<objects.size();i++)
				{
//				cout <<"damaged"<<endl;
				    if(is composite_window*>(objects[i])&&(as composite_window*>(objects[i])->window==(eee->drawable)))
				    {
					as composite_window*>(objects[i])->damaged=1;

					break;
				    }

				}
			    }
			    if(event.syswm.msg->event.xevent.type==ConfigureNotify)
			    {
				
				XEvent ee = event.syswm.msg->event.xevent;
//				XConfigureEvent* eee = (XConfigureEvent*)&ee;
				for(unsigned int i=0;i<objects.size();i++)
				{
//				cout <<"damaged"<<endl;
				    if(is composite_window*>(objects[i]))//&&(as composite_window*>(objects[i])->window==(eee->window)))
					as composite_window*>(objects[i])->needsreconf=1;
				}
			    }
			    if(event.syswm.msg->event.xevent.type==DestroyNotify)
			    {
				
				XEvent ee = event.syswm.msg->event.xevent;
				XDestroyWindowEvent* eee = (XDestroyWindowEvent*)&ee;
				for(unsigned int i=0;i<objects.size();i++)
				{

				    if(is composite_window*>(objects[i])&&(as composite_window*>(objects[i])->window==(eee->window)))
				    {
				    	cout <<"destroyyyed"<<endl;
					objects.erase(objects.begin()+i);
					break;
				    }
				}
			    }
			    if(event.syswm.msg->event.xevent.type==CreateNotify)
			    {
				cout << "created" << endl;
				comp->regetwindows();
			    }
			    
			}
			break;
			
		    case SDL_MOUSEMOTION:
			mousemoved=1;
			lastmousemoved=SDL_GetTicks();
			mousejustmoved=1;
		        break;
		    case SDL_MOUSEBUTTONDOWN:
		    {
			pick(0,event.button.button,event.button.x,event.button.y);
			break;
		    }
		    case SDL_MOUSEBUTTONUP:
		    {
			pick(1,event.button.button,event.button.x,event.button.y);
			break;
		    }
		    
/*
d(WINDOWS) && !defined(OSX)                                                                                              
   framebuffer.h       72                  case SDL_SYSWMEVENT:                                                                                                
      frustum.c           73                                                                                                                                      
         gamewin.c           74                          if(event->syswm.msg->event.xevent.type == SelectionNotify)                                                  
            gamewin.h           75                                  finishpaste(event->syswm.msg->event.xevent.xselection);                                             
               gl_init.c           76                          break;   
    */
		    case SDL_KEYDOWN:
		    {
			dirty=1;
			int key=event.key.keysym.sym;
			int uni=event.key.keysym.unicode;
			int mod=event.key.keysym.mod;

			if(escaped||(mod&KMOD_RCTRL)||(key==SDLK_RCTRL))
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
				    saveobjects(1);
				    break;
				case SDLK_l:
				    loadobjects(1);
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
				        /*GLint max=10;
				        if(settingz.line_antialiasing)
					    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,&max);
					else
					    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,&max); 
					if(settingz.lv>max)settingz.lv=max;
					cout << "max:" << max <<endl;*/
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
				case SDLK_h:
				    settingz.givehelp=!settingz.givehelp;
				    break;
				case SDLK_RCTRL:
				    escaped=1;
				    break;
				default:
				    if(active)
					active->keyp(key,uni,mod|KMOD_RCTRL);//escaped
			    }
			}
			else if(active)
			    active->keyp(key,uni,mod);
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
					w=event.resize.w;h=event.resize.h;
                                        logit("videoresize %ix%i bpp %i", w,h,bpp);
					dirty=1;
					s=SDL_SetVideoMode( w,h, bpp, s->flags);
					resetviewport();
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
					else if(event.user.code==CODE_DATA)
                                            if(reinterpret_cast<RoteTerm*>(event.user.data1)->stoppedscrollback)
                                            	for_each_face
                                            	    if(f->t==reinterpret_cast<RoteTerm*>(event.user.data1))
							f->scroll=f->t->logl;
						endfor endfor

				
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if(mousejustmoved)
		    {
			int x,y;
			SDL_GetMouseState(&x,&y);
			pick(0,0,x,y);
			mousejustmoved=0;
		    }
		    if(gofullscreen)
		    {
			if(s->flags & SDL_FULLSCREEN )
			{
			    s=SDL_SetVideoMode( w,h, bpp, (s->flags & ~SDL_FULLSCREEN ));
			    #ifdef LINUX
			    if(norm&1)
				system("wmctrl -r :ACTIVE: -b remove,maximized_horz");
			    if(norm&2)
				system("wmctrl -r :ACTIVE: -b remove,maximized_vert");
			    #endif
			}
			else
			{
			    #ifdef LINUX
			    SDL_SysWMinfo i;
			    SDL_VERSION(&i.version)
			    if(SDL_GetWMInfo(&i))
			    {
				char *c=make_message("xprop -id %u |grep _NET_WM_STATE_MAXIMIZED_VERT", i.info.x11.window);
				norm=!system(c);
				free(c);
				c=make_message("xprop -id %u |grep _NET_WM_STATE_MAXIMIZED_HORZ", i.info.x11.window);
				norm&=(!system(c))<<1;
				free(c);
			    }
			    //system("wmctrl -r :ACTIVE: -b add,maximized_vert,maximized_horz");
			    #endif
			    SDL_Surface *ns;
			    ns=SDL_SetVideoMode( w,h, bpp, (s->flags | SDL_FULLSCREEN ));
			    if(ns)s=ns;
			}
			gofullscreen=0;
		    }
		    if(!done)
		    {
			unlockterms();
			updateterminals();
		    }
		    else
		    {
			saveobjects();
		    }
		}
	}
	objects.clear();
	font.clear();
	SDL_Quit( );
}                      

#include "getexecname.c"
int main(int argc, char *argv[])
{
	logit("hi");
	
	if((argc==3)||(argc==2))
	    if(!strcmp(argv[1],"-originalldpreload"))
	    
	    {
		originalldpreload=(argc==2)?strdup(""):argv[2];
		//cout << "LD_PRELOAD:"<<originalldpreload<<endl;
	    }
	    
	char *path;
	path=getexepath();
	if(path)
	{
		logit("path:%s", path);
		path=(char*)realloc(path, 100+strlen(path));
		char* n=strrchr(path, 0);
		wrld=strdup(strcat(path, "world.yaml"));	*n=0;
		fnfl=strdup(strcat(path, "l1"));		*n=0;
		clfl=strdup(strcat(path, "colors"));		*n=0;
		stng=strdup(strcat(path, "settings"));		*n=0;
		mdfl=strdup(strcat(path, "mode"));		*n=0;
		fcfl=strdup(strcat(path, "faces"));		*n=0;
		btns=strdup(strcat(path, "buttons/"));		*n=0;
	}
	clipout=rote_vt_create(10,10);
	clipout2=rote_vt_create(10,10);
	lemon();
	rote_vt_destroy(clipout);
	rote_vt_destroy(clipout2);
	logit("finished.bye.\n");
	return 0;
}






