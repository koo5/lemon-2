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
#include "sys/time.h"
#include <stdio.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <roteterm/rote.h>
#include "modes.c"
#include "more-mess/rdfl.c"
#include "more-mess/resize.c"
#include "SDL_thread.h"
#include "SDL_mutex.h"
#ifdef GL
#include "SDL_opengl.h"
#else
#include "SDL_draw.h"
#endif
#include "initsdl.c"
#include "toys/nerverot/stolenfromglu"
#include "toys/nerverot/nerverot.c"

typedef struct 
{
    int x,y,x2,y2;
}	
limits;


typedef struct
{
    Uint32 last_resize;
    RoteTerm *t;
    int lastxresize;
    int lastyresize;
    float x1,y1;
    limits lim;
} roteface;

#include "gltext.c"

float floabs(float x)
{
    return x>0 ? x : -x;
}


float r1x=0;
float r1y=0;
float sx=1;
float sy=-1;
float lv=1;

void keyp(roteface* f, char ey)
{
    if (f->t)
        rote_vt_keypress(f->t,ey);
}

#ifdef GL
// adjust s and r1 to keep terminal on screen on its resize/move
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
	if(floabs(b-e)>0.97)
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

RoteTerm* add_terminal(void)
{
    RoteTerm* t;
    t= rote_vt_create(25,80);
    rote_vt_forkpty((RoteTerm*) t, "bash");
    return t;
}

typedef struct
{
    RoteTerm* rt;
    SDL_mutex *lock;
}
moomoo;


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
      rote_vt_update_thready(buf, 512, &br, d->rt);
//     printf("*end SELECT*\n");
        _mutexP(d->lock);

      if (br>0)
      {
        /* inject the data into the terminal */
//        printf("*locked injecting\n");
	rote_vt_inject(d->rt, buf, br);
//        printf("*locked injected\n");
	SDL_Event e;
	e.type=SDL_USEREVENT;
	SDL_PushEvent(&e);
      }
      
      if(!d->rt->childpid)
      {
        printf("Segmentation Fault\n");
	SDL_Event e;
	e.type=SDL_QUIT;
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
	
void resizooo(roteface *f, int x, int y, Uint8* duck)
{

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

	
int RunGLTest (void)
{
	int bpp;
	int w = 200;
	int h = 200;
	int done = 0;
	int shrink=0;
	int grow=0;
	xy  ss = parsemodes(w,h,"mode",1,0,0);
	printf("wtf\n");
	if (ss.x!=-1){w=ss.x;h=ss.y;};
	SDL_Surface* s;
	s=initsdl(w,h,&bpp,
#ifdef GL
	SDL_OPENGL
#endif
	+0);printf("inito\n");
	SDL_InitSubSystem( SDL_INIT_TIMER);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY/2, SDL_DEFAULT_REPEAT_INTERVAL*2);
#ifdef GL
	printf("pretty far\n");
	wm(w,h);
	glEnable(GL_BLEND);
	glShadeModel(GL_FLAT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glClearColor( 0.0, 0.0, 0.0, 0.0 );
	glLineWidth(lv);
#else
	gltextsdlsurface=s;
#endif
	roteface f;
	RoteTerm* t=f.t=add_terminal();
	printf("still?\n");
	moomoo upd_t_data;
	upd_t_data.rt=f.t;
	upd_t_data.lock = SDL_CreateMutex();
	SDL_CreateThread(update_terminal, (void *)&upd_t_data);
	f.x1=0;
	f.y1=1;
	printf("2threaad\n");
	
	loadl2();

	struct state *nerv=0;
	nerv=nerverot_init(w,h);
	while( !done )
	{	
	        _mutexP(upd_t_data.lock);
		int dirty=1;
		int showhex;

		if(dirty||t->curpos_dirty||lines_r_dirty(t))
		{
#ifdef GL
			glClear(GL_COLOR_BUFFER_BIT);
#else
			SDL_FillRect	( s, NULL, 0 );
#endif

#ifndef GL
				sdl_draw_terminal(f.t,showhex,0);
#else
				if(nerv)
				{
				    nerverot_update(nerv);
				    glPushMatrix();
				    //glTranslatef(0,0,-5.5);
				    nerverot_draw(3,nerv);
				    glPopMatrix();
				    dirty=1;
				}
				glPushMatrix();
				glTranslatef(r1x,r1y,0);
				glScalef(sx,sy,0.004);
				draw_terminal(&f,showhex);
				dirty|=zoomize(&f);
				glPopMatrix();
#endif

#ifndef GL
			SDL_UpdateRect(s,0,0,0,0);
#else
			SDL_GL_SwapBuffers( );
#endif
			f.t->curpos_dirty=false;
			lines_r_clean(f.t);
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
			else	fprintf( stderr, "testgl: OpenGL error: %d\n", gl_error );
			
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
		                     
		_mutexV(upd_t_data.lock);
//		printf("---------unlocked wating\n");
		SDL_Event event;
		if(SDL_WaitEvent( &event ))
		{
	    	    _mutexP(upd_t_data.lock);
//		    printf("---------locked goooin %i\n", event.type);
		    if(x)SDL_RemoveTimer(x);x=0;	    	    
		    do {
			int mod=event.key.keysym.mod;
			int key=event.key.keysym.sym;
    			Uint8 *keystate = SDL_GetKeyState(NULL);

			switch( event.type )
			{
				case SDL_KEYUP:
				{
					if ( (key == SDLK_RCTRL) )
					{
						showhex=0;
						dirty=1;
					}
				}
				break;
				case SDL_KEYDOWN:
					if(mod&KMOD_RCTRL)
					{
						showhex=dirty=1;
						switch (key)
						{
							case SDLK_F6:
							    savemode(w,h);
							break;
							case SDLK_F8:
							    loadl2();
							break;
#ifdef GL
							case SDLK_F9:
							    lv-=1;	glLineWidth(lv);
							break;
							case SDLK_F10:
							    lv+=1;	glLineWidth(lv);
							break;
#endif
							case SDLK_F11:
							    shrink=1;
							break;
							case SDLK_F12:
							    grow=1;
							break;
							case SDLK_END:
							    resizooo(&f, 0,1,keystate);
							break;
							case SDLK_HOME:
							    resizooo(&f, 0,-1,keystate);
							break;
							case SDLK_DELETE:
							    resizooo(&f, -1,0,keystate);
							case SDLK_PAGEDOWN:
							    resizooo(&f, 1,0,keystate);
							break;
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
						}
					}
					else
					{
					    if ( (key >= SDLK_F1) && (key <= SDLK_F15) )
					    {
						char *k;
						if(asprintf(&k ,"kf%i", key-SDLK_F1+1)!=-1)
						{
						    rote_vt_terminfo(f.t, k);
						    free(k);
						}
					    }
					    else
					    if ( (key == SDLK_SPACE) )
						keyp(&f,32);
					    else
					    if ( (key == SDLK_BACKSPACE) )
						rote_vt_terminfo(f.t, "kbs");
					    else
					    if ( (key == SDLK_ESCAPE) )
						keyp(&f,27);
					    else
					    if ( (key == SDLK_LEFT) )
						rote_vt_terminfo(f.t, "kcub1");
					    else
					    if ( (key == SDLK_RIGHT) )
						rote_vt_terminfo(f.t, "kcuf1");
					    else
					    if ( (key == SDLK_UP) )
						rote_vt_terminfo(f.t, "kcuu1");
					    else
					    if ( (key == SDLK_DOWN) )
						rote_vt_terminfo(f.t, "kcud1");
					    else
					    if ( (key == SDLK_END) )
						rote_vt_terminfo(f.t, "kend");
					    else
					    if ( (key == SDLK_HOME) )
						rote_vt_terminfo(f.t, "khome");
					    else
					    if ( (key == SDLK_DELETE) )
						rote_vt_terminfo(f.t, "kdch1");
					    else
					    if ( (key == SDLK_PAGEDOWN) )
						rote_vt_terminfo(f.t, "knp");
					    else
					    if ( (key == SDLK_INSERT) )
					    	rote_vt_terminfo(f.t, "kich1");
					    else
					    if ( (key == SDLK_PAGEUP) )
						rote_vt_terminfo(f.t, "kpp");
					    else
					    if ( (key == SDLK_RETURN) )
						keyp(&f,10);
					    else
					    if( event.key.keysym.unicode && ( (event.key.keysym.unicode & 0xFF80) == 0 ) )
						keyp(&f, event.key.keysym.unicode);
					}
				break;
	 			case SDL_QUIT:
					done = 1;
					break;
				case SDL_VIDEORESIZE:
					w=event.resize.w;h=event.resize.h;
					printf("videoresize %i %i\n", w,h);
					dirty=1;
					if (s=SDL_SetVideoMode( w,h, bpp, s->flags ) ) 
						printf("hmm\n");
					wm(w,h);
					break;
				case SDL_MOUSEMOTION:
					break;
				case SDL_USEREVENT:
					break;
			}
		    }
		    while (SDL_PollEvent(&event));
		    if (shrink||grow)
		    { 
		        resize(&w,&h,&bpp,&s->flags,&shrink,&grow);
		        // wm(w,h);
		    }
		    _mutexV(upd_t_data.lock);
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
	printf("bye\n");
	return 0;
}
