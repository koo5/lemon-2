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


#include <iostream>
#include <vector>
#define _mutexV( d ) {if(SDL_mutexV( d )) {logit("SDL_mutexV!");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {logit("SDL_mutexP!");}}
#define CODE_TIMER 0
#define CODE_QUIT 1
#define as dynamic_cast<
#define is as
#define for_each_object for(int i=0;i<objects.size();i++) { obj*o=objects.at(i);
#define for_each_face for_each_object if (as face*>(o)){face*f=as face*>(o);
#ifdef GL
    inline void dooooot(float x,float y)
    {
        glVertex2f(x,y);
    }
#endif
char *tpl_settingz="S(iiis)";
struct Settingz
{
    int32_t line_antialiasing;
    int32_t showbuttons;
    int32_t givehelp;
    int32_t do_l2;
    double lv;//glLineWidth
}settingz={0,0,1,0,2};
#include "../gltext.c"
char *fnfl="l2";//font file
char *stng;//settingz
char *mdfl;//modes
char *fcfl;//faces
char *clfl;//colors
char *ntfl;//newtermmsg
char *newtermmsg;
char *btns;//buttons/
char **buttons;
char **buttonnames;
int numbuttons;
typedef 
struct 
{
    unsigned char r,g,b;
}
color;
color colors[16];
void loadcolors(void)
{
    int i,useless;
    for (i=0;i<16;i++)
    {
	colors[i].r=255/16*i;
	colors[i].g=255/16*i;
	colors[i].b=255/16*i;
    }
    if(!clfl)return;
    FILE * fp = fopen(clfl,"r");
    if (fp == NULL)
    {
        printf("cant load 'colors'\n");
        return;
    }
    useless=fread(&colors,3,16,fp);
    fclose(fp);
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int in(int a, int b, int c)
{
    return(a<=b&&c>=b);
}


void do_color(int attr)
{
    int c=ROTE_ATTR_XFG(attr);//0-15
    setcolor(colors[c].r/255.0,colors[c].g/255.0,colors[c].b/255.0,1);
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
using namespace std;

vector<obj *> objects;
obj * active;
struct obj{
    int dirty;
    double x,y,z;
    double a,b,c;
    double w,h,d;
    virtual void draw(){};
    virtual int getDirty(){return dirty;}
    virtual void setDirty(int d){dirty=d;}
    void switchpositions(obj*o){
	double t,tt,ttt,tttt,ttttt,tttttt;
	t-o->x;tt=o->y;ttt=o->z;tttt=o->a;ttttt=o->b;tttttt=o->c;
	o->a=a;o->b=b;o->c=c;o->x=x;o->y=y;o->z=z;
	x=t;y=tt;z=ttt;a=tttt;b=ttttt;c=tttttt;
    }
    void boundingbox()
    {
        glVertex3f(0,0,0);
        glVertex3f(w,0,0);
        glVertex3f(w,h,0);
        glVertex3f(0,h,0);
	if(d)
	{
    	    glVertex3f(0,0,d);
	    glVertex3f(w,0,d);
    	    glVertex3f(w,h,0);
    	    glVertex3f(0,h,0);
    	    glVertex3f(0,0,d);
    	    glVertex3f(w,0,d);
    	    glVertex3f(w,h,0);
    	    glVertex3f(0,h,0);
    	    glVertex3f(0,0,d);
    	    glVertex3f(w,0,d);
    	    glVertex3f(w,0,0);
    	    glVertex3f(w,0,d);
    	    glVertex3f(0,0,d);
    	    glVertex3f(w,0,0);
    	    glVertex3f(w,h,0);
    	    glVertex3f(0,h,0);
    	    glVertex3f(0,0,0);
    	    glVertex3f(w,0,0);
    	    glVertex3f(w,h,0);
    	    glVertex3f(0,h,0);
	}
    }
    
    virtual void pick()
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
	    glRotated(a,1,0,0);
	    glRotated(b,0,1,0);
	    glRotated(c,0,0,1);
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
	    glRotated(a,1,0,0);
	    glRotated(b,0,1,0);
	    glRotated(c,0,0,1);
	#endif
	pick();
	#ifdef GL
	    glPopMatrix();
	#endif
    }
    virtual void keyp(int key,int uni,int mod)=0;
    ~obj(){if(active==this)active=0;}
};
#ifdef nerve
    class nerverot:public obj
    {
        struct nerverotstate *nerv;
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
	int getDirty(){return 1;}
    };
#endif
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
#endif
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
	scale=1;
    }
    ~face()
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

    void add_term(int c,int r)
    {
	logit("adding terminal");
	t = rote_vt_create(c,r);
	rote_vt_forkpty((RoteTerm*) t, "bash");
	#ifdef threaded
	    upd_t_data.lock=SDL_CreateMutex();
	    upd_t_data.t=t;
	    logit("upd_t_data.lock=%i",upd_t_data.lock);
	    upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&upd_t_data);
	#endif
    }
    void add_terminal()
    {
	add_term(SDL_GetVideoSurface()->h/26/scale,SDL_GetVideoSurface()->w/13/scale);
    }

    void draw()
    {
    	draw_terminal();
    	
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
	    clipin(0,1);
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
    void draw_terminal()
    {
        xy lok;
        lok.x=0;
        lok.y=0;
        int j=0;
        int i;
        #ifdef GL
	    glBegin(GL_LINE_STRIP);
        #endif
        int scroll=min(scroll,t->logl);
	if(t->log)
	{
	    for (i=t->logl-scroll;i<t->logl;i++)
	    {
	        if(!t->log[i])break;
	        lok.y=(i-t->logl+scroll)*26*scale;
	        if(t->logstart) lok.y-=100;
		if(t->logstart) lok.x-=100;
		for(j=0;j<t->log[i][0].ch;j++)
	        {
		    lok.x=j*13*scale;
		    do_color(t->log[i][j+1].attr);
	    	    drawchar(lok,t->log[i][j+1].ch,scale,scale);
		}	
	    }
        }
	int isundercursor;
	for (i=0; i<t->rows; i++)
	{
	    lok.y=(scroll+i)*26*scale;
	    if(scale>1)lok.y+=(scale-1)/2*26;
	    for (j=0; j<t->cols; j++)
	    {
		lok.x=j*13*scale;
		if(scale>1)lok.x+=(scale-1)/2*13;
		{
		    if((j>0))
			if((ROTE_ATTR_BG(t->cells[i][j-1].attr))!=(ROTE_ATTR_BG(t->cells[i][j].attr)))
			    zspillit(lok,"aaaz",scale);
		    if((j<t->cols-1))
			if((ROTE_ATTR_BG(t->cells[i][j+1].attr))!=(ROTE_ATTR_BG(t->cells[i][j].attr)))
			    zspillit(lok,"zazz",scale);
		    if((i<t->rows-1))
			if((ROTE_ATTR_BG(t->cells[i+1][j].attr))!=(ROTE_ATTR_BG(t->cells[i][j].attr)))
			    zspillit(lok,"azzz",scale);
		    if((i>0))
			if((ROTE_ATTR_BG(t->cells[i-1][j].attr))!=(ROTE_ATTR_BG(t->cells[i][j].attr)))
			    zspillit(lok,"aaza",scale);
		}
		do_color(t->cells[i][j].attr);
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
			    glRotatef(rotor,0,0,1);
    	        	    glBegin(GL_LINE_LOOP);
        	    	    int w=13;
            		    glColor4f(1,0,0,1);
			    glVertex2f(-w*scale,-w*scale);
			    glVertex2f(+w*scale,-w*scale);
			    glVertex2f(+w*scale,+w*scale);
	    		    glVertex2f(-w*scale,+w*scale);
			    glEnd();
			    glPopMatrix();
			    glPushMatrix();
                            glTranslatef(lok.x+13,lok.y+13,0);
			    glRotatef(rotor/10, 0,0,1);
			    int i;
			    int steps=10;
			    for (i=0; i<360; i+=steps)
			    {
				glRotatef(steps, 0,0,1);
				glPushMatrix();
				glTranslatef(0,100,0);
				glBegin(GL_QUADS);
				glColor4f(1,1,0,0.2);
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
    			glRotatef(rotor+=((SDL_GetTicks()-lastrotor)/10),0,1,0);
			lastrotor=SDL_GetTicks();
			glBegin(GL_LINE_STRIP);
			xy molok;molok.x=-13;molok.y=-13;
			drawchar(molok,t->cells[i][j].ch,scale,scale);
			glEnd();
			glPopMatrix();
			glPopMatrix();
			glBegin(GL_LINE_STRIP);
		    }
		    else
		#else
		    if(isundercursor)
			// but still cursor square
			zspillit(lok,nums[0],1.2*scale);
	        #endif
		drawchar(lok,t->cells[i][j].ch,scale,scale);
	    }
	}
	#ifdef GL
	    oldcrow=t->crow;//4 cursor rotation
	    oldccol=t->ccol;
	    glEnd();
	#endif
    }

};
class facespawner:public obj
{
    int w;
    int h;
    facespawner(){w=h=100;}
    void pick()
    {
        if(newtermmsg)
        {
    	    glBegin(GL_QUADS);
    	    glVertex3f(0,0,0);
	    glVertex3f(w,0,0);
	    glVertex3f(w,h,0);
	    glVertex3f(0,h,0);
	    glEnd();
	}
    }
    void keyp(int key,int uni,int mod)
    {
	face*f=new face;
        objects.push_back(f);
        f->x=x;
        f->y=y;
        f->z=z;
    }
    void draw()
    {
        draw_text(newtermmsg);
    }
};                                                               

#ifdef GL
    class spectrum_analyzer:public obj
    {
	GLfloat heights[16][16], scale;
	int16_t buf[2][256];
	int getDirty(){return 1;}
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
	    glColor3f(red,green,blue);
	    draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
	    draw_rectangle(x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);
	
	    glColor3f(0.5 * red, 0.5 * green, 0.5 * blue);
	    draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
	    draw_rectangle(x_offset, 0.0, z_offset, x_offset + width, height, z_offset );

	    glColor3f(0.25 * red, 0.25 * green, 0.25 * blue);
	    draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);	
	    draw_rectangle(x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);
	}
	
	void oglspectrum_gen_heights(int16_t data[2][256])
	{
	    int i,c;
	    int y;
	    GLfloat val;
	    static GLfloat scale=0;
	    if(!scale)
	        scale = 1.0 / log(256.0);
	    int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};
    	    for(y = 15; y > 0; y--)
	    {
	        for(i = 0; i < 16; i++)
	        {
		    heights[y][i] = heights[y - 1][i];
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
		heights[0][i] = val;
	    }
	}
	
	void draw()
	{
	    int x,y;
	    GLfloat x_offset, z_offset, r_base, b_base;
    	    glBegin(GL_TRIANGLES);
	    for(y = 0; y < 16; y++)
	    {
		z_offset = -1.6 + ((15 - y) * 0.2);
		b_base = y * (1.0 / 15);
		r_base = 1.0 - b_base;
		for(x = 0; x < 16; x++)
		{
		    x_offset = -1.6 + (x * 0.2);			
		    draw_bar(x_offset, z_offset, heights[y][x], r_base - (x * (r_base / 15.0)), x * (1.0 / 15), b_base);
		}
	    }
	    glEnd();
	    FILE *f;
	    if(f=fopen("/tmp/somefunnyname", "r"))
	    {
		fread(buf,512,2,f);
    		fclose(f);
		remove("/tmp/somefunnyname");
		oglspectrum_gen_heights(buf);
	    }
	}


	
};
#endif

#include <string>
class mplayer:public obj
{
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
    void load()
    {
	
    }
};

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
		show_button(100,y+=100, buttonnames[--n],picking);
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
obj* mousefocus(int x, int y)
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
	    return objects.at(n);
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
    if(settingz.line_antialiasing)
        glEnable(GL_LINE_SMOOTH);
    else
        glDisable(GL_LINE_SMOOTH);
}
void updatelinewidth()
{
    glLineWidth(settingz.lv);
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
    }
    return 0;
}
int nothing_dirty()
{
    for_each_object
	o->setDirty(0);
    }
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
	objects.push_back(new face);
	objects.push_back(new face);
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
//	    if(k[SDLK_RCTRL])
//	        focusrect(activeface);
//	    focusline(activeface);
	    for_each_object
		o->translate_and_draw();
		#ifdef GL
		glBegin(GL_LINE_STRIP);
		o->boundingbox();
		glEnd();
		#endif
	    }
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
		if(settingz.showbuttons)
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
			if(active&&(escaped||k[SDLK_RCTRL])&&(SDL_BUTTON(1)&SDL_GetMouseState(0,0)))
			{
			    escaped=0;
			    active->move(event.motion.xrel,event.motion.yrel,0);
			}
			if(!SDL_GetMouseState(0,0))
			    active=mousefocus(event.motion.x,event.motion.y);
		        break;
		    case SDL_KEYDOWN:
			dirty=1;		    
			if(key==SDLK_RCTRL)
    			    escaped=1;
			if(escaped||(mod&&KMOD_RCTRL))
			    switch (key)
			    {
			        case SDLK_SLASH:
				    settingz.showbuttons=!settingz.showbuttons;
				    break;
				case SDLK_TAB:
				    for_each_object
					if(active==o)
				        {
					    if(++i==objects.size())
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
			    	    active->z+=1;
			    	    break;
				case SDLK_BACKSPACE:
			    	    active->z-=1;
		        	    break;
				case SDLK_t:
				{
			    	    int yy=0;
				    for_each_object
					o->x=yy;
					o->y=0;
					o->z=0;
					yy+=o->w;
				    }
				    break;
				}
				case SDLK_y:
				{
				    int yy=0;
				    for_each_object
					o->x=0;
				        o->y=yy;
				        o->z=0 ;
				        yy+=o->h;
				    }
				    break;
				}
				case SDLK_u:
				{
			    	    int yy=0;
				    for_each_object
					o->x=0;
				        o->y=0  ;
				        o->z=yy;
				        yy+=o->d;
				    }
				    break;
				}
				case SDLK_p:
				    #ifdef libpng
				        saveScreenshot();
				    #endif
				    break;
				case SDLK_l:
			    	    settingz.do_l2=!settingz.do_l2;
				case SDLK_n:
				    objects.push_back(new face);
				    active=objects.at(objects.size()-1);
				    break;
				#ifdef GL
				    case SDLK_PLUS:
				        settingz.line_antialiasing?settingz.lv+=0.1:settingz.lv++;
				        GLint max;
				        if(settingz.line_antialiasing)
					    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,&max);
					else
					    glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,&max); 
					if(settingz.lv>max)settingz.lv=max;
					updatelinewidth();
					break;
				    case SDLK_MINUS:
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
				    if(is face*>(active))as face*>(active)->resizooo(0,1,k);
				    break;
				case SDLK_HOME:
				    if(is face*>(active))as face*>(active)->resizooo(0,-1,k);
				    break;
				case SDLK_DELETE:
				    if(is face*>(active))as face*>(active)->resizooo(-1,0,k);
				    break;
				case SDLK_PAGEDOWN:
				    if(is face*>(active))as face*>(active)->resizooo(1,0,k);
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
						    objects.erase(objects.begin()+i);
					    }}
					    dirty=1;
					}
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if (shrink||grow)
		    {
			resize(&w,&h,&bpp,&s->flags,&shrink,&grow);
			resetviewport();
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
	obj * o=new face;
	printf("%i\n",o);
	face *ff=dynamic_cast<face*>(o);
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
	initbuttons();
	RunGLTest();
	freebuttons();
	savesettings();
	rote_vt_destroy(clipout);
	rote_vt_destroy(clipout2);
	logit("finished.bye.\n");
	return 0;
}






