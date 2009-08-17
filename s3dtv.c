int dirty;
int o;
#include "s3d.h"
#include "s3d_keysym.h"
#include "stdio.h"
#include "stdlib.h"
#include "roteterm/rote.h"
#define S3D

#include "gltext.c"
int showhex=0;

#include "glterm.c"
RoteTerm *t;



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
    if((noes && (*r)!=10 && (*r)!=13 ) || !noes) keyp(t,*r);
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
					if(mod&S3D_KMOD_RCTRL)
					{
						switch (key)
						{
							case S3DK_F8:
							    loadl2();
							break;
							case S3DK_BACKSPACE:
							    clip(1);
							break;
							case S3DK_F11:
							    clip(0);
							break;
							case S3DK_F12:
							    dirty=1;
							    showhex=!showhex;
							    printf("showhex=!showhex;\n");
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
					{
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
    rote_vt_update(t);

    if(dirty||t->curpos_dirty || lines_r_dirty(t))
    {
	dirty=0;
	draw_terminal(t,showhex);
	t->curpos_dirty=0;
	lines_r_clean(t);
	bpep();
    }
   usleep(1000); // sleep is good
}

static int stop(struct s3d_evt *event)
{
    s3d_quit();
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
    o=s3d_new_object();
	float bla[12]=
	{1, 1, 1, 1,
        1, 1, 1, 1,
        1, 1, 1, 1};

    int b;
    int c;
    for (b=1;b<9;b++)
    {
	for (c=0;c<3;c++)
	{
	    bla[c*4]=1.0/((float)b/2.0);
	}
	s3d_push_materials_a(o,bla, 1); // push a red and a cyan material
    }

    t=rote_vt_create(20,80);
    lines_r_clean(t);
    rote_vt_forkpty(t,"bash");

    s3d_set_callback(S3D_EVENT_QUIT, stop);
    s3d_set_callback(S3D_EVENT_KEY, keypress);


    initbufs();/*
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(10,0,"XXX");
    draw_line(10,110,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,30,"XXX");
    draw_line(10,10,"XXX");
    draw_line(03,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,02,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    draw_line(0,0,"XXX");
    draw_line(10,10,"XXX");
    draw_line(0,20,"XXX");
    bpush();  */  s3d_flags_on(o, S3D_OF_VISIBLE);
    s3d_mainloop(mainloop);
    rote_vt_destroy(t);
}
