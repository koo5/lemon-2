int dirty;
//int o;
#include "s3d.h"
#include "s3d_keysym.h"
#include "stdio.h"
#include "stdlib.h"
#include "roteterm/rote.h"

#include "../gltext.c"
int showhex=0;
int ok=0;
#include "glterm.c"
RoteTerm *t;

int afterlife=0;

keyp(RoteTerm *t,char k)
{
    rote_vt_keypress(t,k);
}


void clip(int noes)
{
char * r=rotoclipin();
char *s=r;
if(r)
{
while(*r)
{
    if(!noes || ((*r)!=10 && (*r)!=13 )) keyp(t,*r);
    r++;
}
free(s);
}
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
void lines_r_not_clean(RoteTerm *rt)
{
    int x;
    for (x=0;x<rt->rows;x++)
	rt->line_dirty[x]=1;
}

void scrollup(void)

{


}

void resizooo(RoteTerm *t, int x, int y)
{
	rote_vt_resize(t, t->rows+y,t->cols+x);
	lines_r_not_clean(t);//4 valgrinds unitialized value blahblahs
}

static int keypress(struct s3d_evt *event)
{
    struct s3d_key_event *keys = (struct s3d_key_event *)event->buf;
    int key=keys->keysym;    
    int mod=keys->modifier;
					if(mod&S3D_KMOD_RSHIFT)
					{
						switch (key)
						{
							case S3DK_INSERT:
							    clip(mod&S3D_KMOD_RCTRL);
						}
					}
					
					if(mod&S3D_KMOD_RCTRL)
					{
						switch (key)
						{
							case S3DK_F5:
							    wdt-=0.1;
							    dirty=1;
							break;
							case S3DK_F6:
							    wdt+=0.1;
							    dirty=1;
							break;
							case S3DK_F8:
							    loadl2();
							    dirty=1;
							break;
							case S3DK_F12:
							    dirty=1;
							    showhex=!showhex;
							    printf("showhex=!showhex;\n");
							break;
							case S3DK_F10:
							    dirty=1;
							    ok=!ok;
							    printf("ok\n");
							break;
							case S3DK_END:
							    resizooo(t, 0,1);
							break;
							case S3DK_HOME:
							    resizooo(t, 0,-1);
							break;
							case S3DK_DELETE:
							    resizooo(t, -1,0);
							break;
							case S3DK_PAGEDOWN:
							    resizooo(t, 1,0);
							break;
						}
					}
					else
					if(mod&S3D_KMOD_RALT)
					{
						switch (key)
						{
							case S3DK_END:
							    resizooo(t, 0,100);
							break;
							case S3DK_HOME:
							    resizooo(t, 0,-100);
							break;
							case S3DK_DELETE:
							    resizooo(t, -100,0);
							break;
							case S3DK_PAGEDOWN:
							    resizooo(t, 100,0);
							break;
						}
					}
					else
					{
					    if ( (key == S3DK_F1) )
					    {
						keyp(t,27);
						keyp(t,'[');
						keyp(t,'M');
						keyp(t,32);
						keyp(t,33);
						keyp(t,33);
						
					    
					    }
					    else

					    if ( (key >= S3DK_F1) && (key <= S3DK_F15) )
					    {
						char *k;
						if(asprintf(&k ,"kf%i", key-S3DK_F1+1)!=-1)
						{
						    rote_vt_terminfo(t, k);
						    free(k);
						}
					    }
					    else
					    if ( (key == S3DK_SPACE) )
						keyp(t,32);
					    else
					    if ( (key == S3DK_BACKSPACE) )
						rote_vt_terminfo(t, "kbs");
					    else
					    if ( (key == S3DK_ESCAPE) )
						keyp(t,27);
					    else
					    if ( (key == S3DK_LEFT) )
						rote_vt_terminfo(t, "kcub1");
					    else
					    if ( (key == S3DK_RIGHT) )
						rote_vt_terminfo(t, "kcuf1");
					    else
					    if ( (key == S3DK_UP) )
						rote_vt_terminfo(t, "kcuu1");
					    else
					    if ( (key == S3DK_DOWN) )
						rote_vt_terminfo(t, "kcud1");
					    else
					    if ( (key == S3DK_END) )
						rote_vt_terminfo(t, "kend");
					    else
					    if ( (key == S3DK_HOME) )
						rote_vt_terminfo(t, "khome");
					    else
					    if ( (key == S3DK_DELETE) )
						rote_vt_terminfo(t, "kdch1");
					    else
					    if ( (key == S3DK_PAGEDOWN) )
						rote_vt_terminfo(t, "knp");
					    else
					    if ( (key == S3DK_INSERT) )
					    	rote_vt_terminfo(t, "kich1");
					    else
					    if ( (key == S3DK_PAGEUP) )
					    if(mod&S3D_KMOD_RSHIFT)
						scrollup();
					    else
						rote_vt_terminfo(t, "kpp");
					    else
					    if ( (key == S3DK_RETURN) )
						keyp(t,10);
					    else
					    if( keys->unicode && ( (keys->unicode & 0xFF80) == 0 ) )
						keyp(t, keys->unicode);
					}
}

void mainloop(void)
{
    if (!rote_vt_update(t) && !afterlife)
	s3d_quit();
    if(dirty||t->curpos_dirty || lines_r_dirty(t))
    {
	dirty=0;
	lines_r_clean(t);
	t->curpos_dirty=0;

	draw_terminal(t,showhex);
	bpep();


    }
   usleep(1000); // sleep is good
}

static int stop(struct s3d_evt *event)
{
    s3d_quit();
    return(0);
}

static int camcamcam(struct s3d_evt *e)
{
    struct s3d_obj_info *event;
    event =(struct s3d_obj_info*) e->buf;
    float zx,zy;
    if(!(strncmp(event->name, "sys_cam",7)))
    {
    //(
    printf("%s  | ,",event->name);
    printf("%f\n", event->scale);
    //)
	if(event->scale==1)
	    zx=zy=1;
	else	if(event->scale<1)//its tall
	{
	    zx=1;
	    zy=1/event->scale;
	}
	else	if(event->scale>1)
	{
	    zy=1;
	    zx=event->scale;
	}
	zoomx=zx*12.5;
	zoomy=zy*10;
	dirty=1; mainloop();
    }
    return(0);
}



int main(int a, char **v)
{
    if(s3d_init(&a,&v,"test"))
    {
	printf("opsie\n");
	s3d_usage();
	s3d_quit();
    }
    tex=s3d_new_object();
	float bla[12]=
	{1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1};

    int b;
    int c;
    for (b=1;b<9;b++)
    {
	for (c=0;c<1;c++)
	{
	    bla[c*4]=1.0/((float)b/2.0);
	}
	s3d_push_materials_a(tex,bla, 1);
    }

    t=rote_vt_create(30,120);
    //lines_r_clean(t);
    rote_vt_forkpty(t,"bash");

    s3d_flags_on(tex, S3D_OF_VISIBLE|S3D_OF_SELECTABLE);
    s3d_set_callback(S3D_EVENT_QUIT, stop);
    s3d_set_callback(S3D_EVENT_KEY, keypress);
    s3d_set_callback(S3D_EVENT_OBJ_INFO, camcamcam);
    zoomx=1.666;
    zoomy=1;
    loadl2();
    initbufs();
    s3d_mainloop(mainloop);
    rote_vt_destroy(t);
}
