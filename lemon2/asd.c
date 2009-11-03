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
#ifdef GL
#include "SDL_opengl.h"

#ifdef nerve
#include "../toys/nerverot/stolenfromglu"
#include "../toys/nerverot/nerverot.c"
#endif

#else
#include "SDL_draw.h"
#endif

#include "initsdl.c"
#include "screenshot.c"


#ifdef GL
inline void dooooot(float x,float y)
{
    glVertex2f(x,y);
}
#endif

#ifdef SDLD
#include "../sdldlines.c"
#endif




#include "../gltext.c"

double rastio=1;


typedef struct
{
    RoteTerm* t;
    SDL_mutex *lock;
}
moomoo;

typedef struct
{
    Uint32 last_resize;
    RoteTerm *t;
    int lastxresize;
    int lastyresize;
    float x,y;
    limits lim;
    void * next;
    moomoo upd_t_data;
} roteface;


#include "glterm.c"

char *newtermmsg;//
xy cam;//




float floabs(float x)
{
    return x>0 ? x : -x;
}


float r1x=0;
float r1y=0;
float sx=0.01;
float sy=-0.01;
float lv=1;

void keyp(roteface* f, char ey)
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




#define _mutexV( d ) {if(SDL_mutexV( d )) {printf("SDL_mutexV!\n");}}
#define _mutexP( d ) {if(SDL_mutexP( d )) {printf("SDL_mutexP!\n");}}

int update_terminal(void *data)
{
    while(1)
    {
      moomoo * d = (moomoo *)data;
      char buf[512];
      int br=-1;
//     printf("UNLOCKED SELECT\n");
      rote_vt_update_thready(buf, 512, &br, d->t);
//     printf("*end SELECT, locking %i*\n", d->lock);
	_mutexP(d->lock);
//        printf("LOCKED\n");

      if (br>0)
      {
	/* inject the data into the terminal */
//        printf("*locked injecting\n");
	rote_vt_inject(d->t, buf, br);
//        printf("*locked injected\n");
	SDL_Event e;
	e.type=SDL_USEREVENT;
	e.user.code=0;
	SDL_PushEvent(&e);
      }
      
      if(!d->t->childpid)
      {
	printf("Segmentation Fault\n");
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






void wm(int w,int h)
{
#ifdef GL

	glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	glFrustum(-1, 1, -1, 1, 1.5, 10);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glTranslatef(0,0,-4);
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


#ifdef Limits
// adjust s and r1 to keep terminal on screen
int zoomize(roteface *f)
{
	GLdouble model_view[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model_view);
	GLdouble projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	GLint viewport[4];
	viewport[0]=0;	viewport[1]=0; viewport[2]=1;	viewport[3]=1;
	double a,b,c,d,e;	
	gluProject(f->lim.x, f->lim.y, 0,model_view, projection, viewport,&a,&b,&c);
	gluProject(f->lim.x2,f->lim.y2,0,model_view, projection, viewport,&d,&e,&c);
	if(isnan(a))a=0.1;
	if(isnan(b))b=0.1;
	if(isnan(d))d=0.8;
	if(isnan(e))e=0.8;
	int eee=0;
	if(d-a>0.97)
	{eee++;
	    sx*=0.9;}
	if(d-a<0.9)
	{eee++;
	    sx*=1.01;}
	if(floabs(b-e)<0.9)
	{eee++;
	    sy*=1.01;}
	if(floabs(b-e)>0.91)
	{eee++;
	    sy*=0.9;}
	if(a<f->x1)
	{eee++;r1x+=0.01;}
	if(a>f->x1+0.01)
	{eee++;r1x-=0.1;}
	if(b>f->y1)
	{eee++;r1y-=0.1;}
	if(b<f->y1-0.1)
	{eee++;r1y+=0.1;}
	//printf("%i",eee);
	return eee;
}
//e.x1=0
//e.y1=1;
#endif

	
void resizooo(roteface *f, int x, int y, Uint8* duck)
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
	SDL_PushEvent(&e);
	return interval;
}    

	


roteface * add_face(void)
{
    roteface * f=(roteface*)malloc(sizeof(roteface));
    f->next=0;
    f->x=0;
    f->y=1;
    f->t=0;
}

void add_terminal(roteface * f)
{
    printf("adding terminal|");
    RoteTerm* t;
    t= rote_vt_create(25,90);
    rote_vt_forkpty((RoteTerm*) t, "bash");
    f->t=t;
    f->upd_t_data.lock=SDL_CreateMutex();
    f->upd_t_data.t=t;
//    printf("upd_t_data.lock=%i", f->upd_t_data.lock);
    SDL_CreateThread(update_terminal, (void *)&f->upd_t_data);
    printf("|added.\n");
}

void cycle(roteface *face1, roteface **g)
{
    *g=(*g)->next;
    if(*g==0)
	*g=face1;
}

int term_dirty(RoteTerm *t)
{
    return t->curpos_dirty || lines_r_dirty(t);
}
int faces_dirty(roteface * f)
{
    while(f&&f->t)
    {
	if(term_dirty(f->t))
	    return 1;
	f=f->next;
    }
    return 0;
}

void lockterms(roteface * f)
{
//    printf("locking terms\n");
    while(f&&f->t)
    {
	_mutexP(f->upd_t_data.lock);
	f=f->next;
//      printf(".\n");
    }
//    printf("done\n");
}
void unlockterms(roteface * f)
{
    while(f&&f->t)
    {
	_mutexV(f->upd_t_data.lock);
	f=f->next;
    }
}

void faceclean(roteface *f)
{
	f->t->curpos_dirty=false;
	lines_r_clean(f->t);
}

void facesclean(roteface * f)
{
    while(f&&f->t)
    {
	faceclean(f);
	f=f->next;
    }
}

void showface(roteface *g)
{
    if(g->t)
	draw_terminal(g);
    else
	draw_text(newtermmsg);
}


int countfaces(roteface* f)
{
    int i=0;
    while(f)
    {
	i++;
	f=f->next;
    }
}

void focusline(roteface * activeface)
{
    static double angel=0;
    float csize=30;
#ifdef GL

    glBegin(GL_LINES);
    glColor3f(1,1,0);
    glVertex2i(0,0);
//    glVertex2f(activeface->x, activeface->y);
    glVertex2f(csize*sin(angel)+activeface->x,csize*cos(angel)+activeface->y);
    angel+=0.1;
    if(angel>2*3.14159265358979323846264)angel=0;
    glEnd();
#else
    Draw_Line(gltextsdlsurface,activeface->x,activeface->y,activeface->x+100,activeface->y,barvicka);
    Draw_Line(gltextsdlsurface,activeface->x,activeface->y+100,activeface->x,activeface->y,barvicka);
#endif

}

roteface *getprevface(roteface* f1,roteface* f)
{
    roteface *prev, *cyc;
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
    


void removeface(roteface **face1, roteface *f)
{
    if(*face1==f)
	*face1=f->next;
    else
    {
	roteface * prev=getprevface(*face1, f);
	if(prev)prev->next=f->next;
    }
    rote_vt_destroy(f->t);
    free(f);
}

void RemoveTerm(roteface** activeface,roteface** f1, RoteTerm * Term)
{
    while(f1)
    {
	if((*f1)->t==Term)
	{
	    removeface(f1, *f1);
	    if(*activeface==*f1)
		*activeface=(*f1)->next;
	    return;
	}
	(*f1)=(*f1)->next;
    }
}
void  showfaces(roteface * g)
{
	while(g)
	{
#ifdef GL
	    glPushMatrix();
	    glTranslatef(g->x,g->y,0);
#endif
#ifdef SDLD
	    gltx=g->x;
	    glty=g->y;
#endif
	    showface(g);
	    g=g->next;
#ifdef GL
	    glPopMatrix();
#endif
	}
}

#ifdef nerve
void shownerv(struct state *nerv)
{
    nerverot_update(nerv);
    glPushMatrix();
    //glTranslatef(0,0,-5.5);
    nerverot_draw(3,nerv);
    glPopMatrix();
}
#endif

#ifdef GL
void krychlus(roteface *g)
{
	int nf;
//      glScalef(0.2,0.2,1);
	nf=countfaces(g);
	printf("%i\n " ,nf / 6);
	while(g)
	{
	    showface(g);
	    glTranslatef(200,200,0);
	    g=g->next;
	}
}
#endif




int RunGLTest (void)
{

#ifdef GL
	int mm=1;
#else
	int mm = 0;
#endif
	int mode=0;
	cam.x=0;
	cam.y=0;
	int bpp;
	int w = 1280;
	int h = 800;
	int done = 0;
	int shrink=0;
	int grow=0;
	int gofullscreen=1;
	int escaped = 0;
	int mustresize = 1;
	int justresized = 0;
	xy  ss = parsemodes(w,h,"mode",1,0,0);
	printf("wtf\n");
	if (ss.x!=-1){w=ss.x;h=ss.y;};
	SDL_Surface* s;
#ifdef GL
	s=initsdl(w,h,&bpp,SDL_OPENGL

#else
	gltextsdlsurface=s=initsdl(w,h,&bpp,
#endif

	+0);printf("inito\n");
	SDL_InitSubSystem( SDL_INIT_TIMER);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2, SDL_DEFAULT_REPEAT_INTERVAL*2);
#ifdef GL
	newtermmsg=GetFileIntoCharPointer1("newtermmsg");
	printf("pretty far\n");
	wm(w,h);
	int down=0;
	glEnable(GL_BLEND);
	glShadeModel(GL_FLAT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glClearColor( 0.0, 0.0, 0.04, 0.0 );
	glLineWidth(lv);
#else
	gltextsdlsurface=s;
#endif
	roteface *face1;
	roteface *activeface;
	face1=add_face();
	activeface =face1 ;
	face1->next=add_face();
	printf("still?\n");
	add_terminal(face1);
	printf("2threaad\n");
	loadl2("l2");
	struct state *nerv=0;
#ifdef nerve
	nerv=nerverot_init(w,h);
#endif

	int dirty=1;
	printf("mainloop descent commencing\n");
	while( !done )
	{       
		lockterms(face1);


		if(dirty||faces_dirty(face1))
		{
			dirty=0;
			facesclean(face1);
#ifdef GL
			glClear(GL_COLOR_BUFFER_BIT);
#else
			SDL_FillRect    ( s, NULL, 0 );

#endif

#ifndef GL

#else

				if(nerv)
				{
				    shownerv(nerv);
				    dirty=1;
				}
				glPushMatrix();
				glScalef(sx,sy,0.004);
				glTranslatef(cam.x,cam.y,0);
				

#endif
				Uint8 * k;
				int integer;

				k=SDL_GetKeyState(&integer);
				if(k[SDLK_RCTRL])
					focusline(activeface);
				int nf;
				switch(mode)
				{
				    case 0:
					showfaces(face1);
				    break;
#ifdef GL
				    case 1:
					krychlus(face1);
				    break;
#endif
				}
#ifdef GL
				glPopMatrix();
#endif


#ifndef GL
			SDL_UpdateRect(s,0,0,0,0);
#else
			SDL_GL_SwapBuffers( );
#endif
			facesclean(face1);
			
		}
#ifdef GL
		GLenum gl_error;
		gl_error = glGetError( );
		if( gl_error != GL_NO_ERROR )
		{
			if(gl_error==GL_STACK_OVERFLOW)
				printf("QUACK QUACK QUACK, OVERFLOVING STACK\n");
			else if(gl_error==GL_INVALID_OPERATION)
				printf("INVALID OPERATION, PATIENT EXPLODED\n");
			else    fprintf( stderr, "testgl: OpenGL error: %d\n", gl_error );
			
		}
#endif
		char* sdl_error;
		sdl_error = SDL_GetError( );
		if( sdl_error[0] != '\0' )
		{
		    fprintf(stderr, "testgl: SDL error '%s'\n", sdl_error);
		    SDL_ClearError();
		}

		SDL_TimerID x=0;
		if(dirty)
		    x= SDL_AddTimer(55, NewTimerCallback, 0);
				     
		unlockterms(face1);
//              printf("---------unlocked wating\n");
		SDL_Event event;
		if(SDL_WaitEvent( &event ))
		{
		    lockterms(face1);
//                  printf("---------locked goooin %i\n", event.type);
		    if(x)SDL_RemoveTimer(x);x=0;                    
		    do {
			int mod=event.key.keysym.mod;
			int key=event.key.keysym.sym;
			Uint8 *keystate = SDL_GetKeyState(NULL);

			switch( event.type )
			{
			
#ifdef GL
				case SDL_MOUSEMOTION:
				if((SDL_BUTTON(1)|SDL_BUTTON(2))&SDL_GetMouseState(0,0))
				{
					activeface->x+=event.motion.xrel;
					activeface->y+=event.motion.yrel;
				}
				if((SDL_BUTTON(3)|SDL_BUTTON(2))&SDL_GetMouseState(0,0))
				{
					cam.x-=event.motion.xrel;
					cam.y-=event.motion.yrel;
				}
				
				break;
#endif
				case SDL_KEYUP:
				{
					if ( (key == SDLK_RCTRL) )
					{
						dirty=1;
					}
				}
				break;
				case SDL_KEYDOWN:
					if(!activeface->t)
						add_terminal(activeface);

					if(mod&KMOD_RSHIFT&&(key==SDLK_PAGEUP||key==SDLK_PAGEDOWN))
					{
						dirty=1;
						if(key==SDLK_PAGEUP)
							tscroll+=9;
						if(key==SDLK_PAGEDOWN)
							tscroll-=9;
						if(tscroll<0)tscroll=0;
//                                              printf("scroll:%i,logl:%i, log&%i, t:%i ,b:%i\n", tscroll,activeface->t->logl, activeface->t->log,activeface->t->scrolltop,activeface->t->scrollbottom);
					}
					else
					if(key==SDLK_RCTRL||mod&KMOD_RCTRL||escaped)
					{
						dirty=1;
						escaped=0;
						if(key==SDLK_RCTRL) escaped=1;
						switch (key)
						{
							case SDLK_TAB:
							    cycle(face1, &activeface);
							break;
							case SDLK_F2:
							    gofullscreen=1;
							break;
							case SDLK_F3:
							    rastio+=0.05;
							    mustresize=1;
							    dirty=1;
							break;
							case SDLK_F4:
							    rastio-=0.05;
							    mustresize=1;
							    dirty=1;
							break;
							case SDLK_F5:
							    sx-=0.001;
							    sy+=0.001;
							break;
							case SDLK_F6:
							    sx+=0.001;
							    sy-=0.001;
							break;

							case SDLK_F7:
							    savemode(w,h);
							break;
							case SDLK_F8:
							    loadl2("l2");
							break;
							case SDLK_p:
							    saveScreenshot();
							break;
#ifdef GL
							case SDLK_F9:
							    lv-=1;      glLineWidth(lv);
							break;
							case SDLK_F10:
							    lv+=1;      glLineWidth(lv);
							break;
#endif
							case SDLK_F11:
							    shrink=1;
							break;
							case SDLK_F12:
							    //grow=1;
							    done=1;
							    
							break;
							case SDLK_PAGEUP:
							     mode++;
							     if(mode>mm)mode= mm;
							break;
							case SDLK_INSERT:
							     mode--;
							     if(mode<0)mode= 0;
							break;
							case SDLK_END:
							    resizooo(activeface, 0,1,keystate);
							break;
							case SDLK_HOME:
							    resizooo(activeface, 0,-1,keystate);
							break;
							case SDLK_DELETE:
							    resizooo(activeface, -1,0,keystate);
							break;
							case SDLK_PAGEDOWN:
							    resizooo(activeface, 1,0,keystate);
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
#endif
						}
					}
					else
					{
					    if(activeface->t==0)
					    {
						printf("debug messages r fun\n");
						add_terminal(activeface);
						activeface->next=add_face();
					    }
					    if ( (key >= SDLK_F1) && (key <= SDLK_F15) )
					    {
						char *k;
						if(asprintf(&k ,"kf%i", key-SDLK_F1+1)!=-1)
						{
						    rote_vt_terminfo(activeface->t, k);
						    free(k);
						}
					    }
					    else
					    if ( (key == SDLK_SPACE) )
						keyp(activeface,32);
					    else
					    #define magic(x) rote_vt_terminfo(activeface->t, x )
					    if ( (key == SDLK_BACKSPACE) )
						magic( "kbs");
					    else
					    if ( (key == SDLK_ESCAPE) )
						keyp(activeface,27);
					    else
					    if ( (key == SDLK_LEFT) )
						magic("kcub1");
					    else
					    if ( (key == SDLK_RIGHT) )
						magic( "kcuf1");
					    else
					    if ( (key == SDLK_UP) )
						magic( "kcuu1");
					    else
					    if ( (key == SDLK_DOWN) )
						magic( "kcud1");
					    else
					    if ( (key == SDLK_END) )
						magic( "kend");
					    else
					    if ( (key == SDLK_HOME) )
						magic("khome");
					    else
					    if ( (key == SDLK_DELETE) )
						magic( "kdch1");
					    else
					    if ( (key == SDLK_PAGEDOWN) )
						magic( "knp");
					    else
					    if ( (key == SDLK_INSERT) )
						magic( "kich1");
					    else
					    if ( (key == SDLK_PAGEUP) )
						magic ( "kpp" );
					    else
					    if ( (key == SDLK_RETURN) )
						keyp(activeface,13);
					    else
					    if( event.key.keysym.unicode && ( (event.key.keysym.unicode & 0xFF80) == 0 ) )
						keyp(activeface, event.key.keysym.unicode);
					}
				break;
				case SDL_QUIT:
					done = 1;
					break;
#ifndef GL
				case SDL_MOUSEBUTTONDOWN:
					rote_vt_mousedown(activeface->t,event.button.x/13, event.button.y/26);
					break;
				case SDL_MOUSEBUTTONUP:
					rote_vt_mouseup  (activeface->t,event.button.x/13, event.button.y/26);
					break;
				case SDL_MOUSEMOTION:
					rote_vt_mousemove(activeface->t,event.button.x/13, event.button.y/26);
					break;
#endif
				case SDL_VIDEORESIZE:
				    {
					w=event.resize.w;h=event.resize.h;
//                                      printf("videoresize %i %i\n", w,h);
					dirty=1;
					if (s=SDL_SetVideoMode( w,h, bpp, s->flags ) ) 
//                                              printf("hmm\n");
					wm(w,h);
				    if(!justresized)

					mustresize=1;
					justresized=0;
					
				    }

					break;
				case SDL_USEREVENT:
					if(event.user.code==1)
					    RemoveTerm(&activeface,&face1, event.user.data1);
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if (shrink||grow)
		    { 
			resize(&w,&h,&bpp,&s->flags,&shrink,&grow);
			wm(w,h);
		    }
		    if (mustresize)
		    {
			mustresize=0;
			justresized=1;
//                      if(activeface->t->cols!=event.resize.w/13/rastio||
//                          activeface->t->rows!=event.resize.h/26/rastio)
				//rote_vt_resize(activeface->t,event.resize.h/26/rastio ,event.resize.w/13/rastio);
		    }
		    if(gofullscreen)
		    {
			dirty=1;
			if(s->flags & SDL_FULLSCREEN )
			{
			
			    s=SDL_SetVideoMode( w,h, bpp, (s->flags & ~SDL_FULLSCREEN ));
			    printf("gooin !fuulin");
			}
			else
			    s=SDL_SetVideoMode( w,h, bpp, (s->flags | SDL_FULLSCREEN ));
		    }
		    gofullscreen=0;
		    unlockterms(face1);
		}

	}
	SDL_Quit( );
	return(0);
}

int main(int argc, char *argv[])
{
	printf("hi\n");
	printf("Ctrl+ Home End PgDn Delete to resize , f11 f12 to resize window, f9 f10 line width, \n");
	RunGLTest();
	printf("finished.bye.\n");
	return 0;
}
