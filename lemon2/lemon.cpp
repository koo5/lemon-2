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
#define as dynamic_cast<
#define is as
#define for_each_object for(unsigned int i=0;i<objects.size();i++) { obj*o=objects.at(i);
#define endfor }
#define endf }}
using namespace std;
using namespace YAML;

int w = 1280;
int h = 800;

float znear=1;
float zfar=20;

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

#include "v3d.cpp"
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
char *wrld;//world yaml

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
void resetmatrices(void)
{
    #ifdef GL
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
        glMatrixMode( GL_MODELVIEW );
        glLoadIdentity();
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
#include "lemon-pixel-city.c"
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


#include "terminal.cpp"
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
#include "spectrum_analyzer.cpp"
#ifdef nerve
#include "nerverot.cpp"
#endif
#endif
//#include <string>
//#include "mplayer.cpp"
#include "compositing.cpp"
#ifdef fontwatcher
#include <sys/inotify.h>
#include "fontwatcher.cpp"
#endif
//#include "listdir.c"
//#include "buttons.cpp"


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

SDL_Rect *SDLRect(Uint16 x,Uint16 y,Uint16 w,Uint16 h)
{
    static SDL_Rect xx ;
    xx.x=x;
    xx.y=y;
    xx.w=w;
    xx.h=h;
    return &xx;
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
	char *path = needexepath();
	char* world = "world";
	if(argc==2)
		world = argv[1];

	path=(char*)realloc(path, 100+strlen(path)+strlen(world));
	char* n=strrchr(path, 0);
	wrld=strdup(strcat(strcat(path, world),".yaml"));*n=0;
	fnfl=strdup(strcat(path, "l1"));		*n=0;
	clfl=strdup(strcat(path, "colors"));		*n=0;
	stng=strdup(strcat(path, "settings"));		*n=0;
	mdfl=strdup(strcat(path, "mode"));		*n=0;
	fcfl=strdup(strcat(path, "faces"));		*n=0;
	btns=strdup(strcat(path, "buttons/"));		*n=0;
	
	lemon();
	return 0;
}






