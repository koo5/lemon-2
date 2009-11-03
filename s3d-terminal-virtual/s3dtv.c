int generic=0;
int dirty;
#include "s3d.h"
#include "s3d_keysym.h"
#include "stdio.h"
#include "stdlib.h"
#include "roteterm/rote.h"

#include "s3dlines"
#include "../gltext.c"
//tohle patri ksobe

int showhex=0;
int ok=0;
int butt=0;
//#define S3D
#include "glterm.c"

RoteTerm *t;
int afterlife=0;
int mouse;
int mousex=-1;
int mousey;
int boogieman;
int boogiemanw=0;
int boogiemanh=0;
int msx, msy, mex, mey;
int sel=0;
int selpop=0;
int selob;
int globmousecopying=0;

keyp(RoteTerm *t,char k)
{
    rote_vt_keypress(t,k);
}


void clip(int noes)
{
    char * r=rotoclipin();
    char *s=r;
    if (r)
    {
        while (*r)
        {
            if (!noes || ((*r)!=10 && (*r)!=13 )) keyp(t,*r);
            r++;
        }
        free(s);
    }
}


setclip(char *x)
{
    printf("%s\n", x);

}

void copy(void)
{
    int i,j;
    int sx,sy,ex,ey;
    char *x;
    if (!sel)return;
    int up;
    if (msy<mey)up=0;
    else if (msy>mey)up=1;
    else
        up=(msx>mex);

    x=malloc(((abs(msy-mey))+1)*t->cols+1);
    if (!x)return;
    if (!up)
    {
        sx=msx;
        sy=msy;
        ex=mex;
        ey=mey;
    }
    else
    {
        sx=mex;
        sy=mey;
        ex=msx;
        ey=msy;
    }
    char  * p=x;
    int k=0;
    if (sy!=ey)
    {
        for (j=sx; j<t->cols; j++)
            (p[k++])=t->cells[sy][j].ch;
        for (i=sy+1; i<ey; i++)
            for (j=0; j<t->cols; j++)
                (p[k++])=t->cells[i][j].ch;
        for (j=0; j<ex; j++)
            (p[k++])=t->cells[ey][j].ch;
    }
    else
        for (j=sx; j<ex; j++)
            (p[k++])=t->cells[ey][j].ch;
    p[k]=0;
//    rotoclipout(x);
    free(x);
}

int lines_r_dirty(RoteTerm *rt)
{
    int x;
    for (x=0; x<rt->rows; x++)
        if (rt->line_dirty[x])
            return 1;
    return 0;
}
void lines_r_clean(RoteTerm *rt)
{
    int x;
    for (x=0; x<rt->rows; x++)
        rt->line_dirty[x]=0;
}
void lines_r_not_clean(RoteTerm *rt)
{
    int x;
    for (x=0; x<rt->rows; x++)
        rt->line_dirty[x]=1;
}

void scrollup(void)

{


}



void mainloop(void)
{
    #define UP 100
    static int a = UP;
    a--;
    if (a==-1) a=UP;
    int u;
    if (!(u=rote_vt_update(t)) && !afterlife)
        s3d_quit();
    if (!a||dirty||((u==2)&&(t->curpos_dirty || lines_r_dirty(t))))
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
/*
void shooboogieman(RoteTerm *rt)
{

	s3d_pop_polygon(boogieman, (rt->cols)*(rt->rows));
	s3d_pop_vertex (boogieman, (rt->cols+1)*(rt->rows+1));

}
*/


void pokeboogieman(RoteTerm *rt)
{
    if ((boogiemanw>=rt->cols)&&(boogiemanh>=rt->rows))
        return;

    if (boogiemanh*boogiemanw)
    {
        s3d_pop_polygon(boogieman, boogiemanh*boogiemanw);
        s3d_pop_vertex (boogieman, boogiemanh*boogiemanw);
    }

    int i,j;
    for (i=0; i<rt->cols+1; i++)
        for (j=0; j<rt->rows+1; j++)
        {
            float y=(j*-25+(rt->rows+2)*25/2)-25;
            float x=(i*wdt-(rt->cols-1)*wdt/2);
            s3d_push_vertex(boogieman,xoom*x,yoom*y,0);
        }
    for (i=0; i<rt->cols; i++)
        for (j=0; j<rt->rows; j++)
        {
            s3d_push_polygon(boogieman,(1+i)*(1+rt->rows)+j, i*(rt->rows+1)+j+1, i*(rt->rows+1)+j,0);
            s3d_push_polygon(boogieman, (1+i)*(1+rt->rows)+j+1, i*(rt->rows+1)+j+1, (1+i)*(rt->rows+1)+j,1);
        }
    boogiemanh=rt->rows;
    boogiemanw=rt->cols;
}/*expain into infinity, boogieman!!!*/


void reshapeboogieman(RoteTerm *rt)
{
    if (boogiemanh*boogiemanw)
    {
        s3d_pop_vertex (boogieman, (1+boogiemanh)*(1+boogiemanw));
    }

    int i,j;
    for (i=0; i<boogiemanw+1; i++)
        for (j=0; j<boogiemanh+1; j++)
        {
            float y=(j*-25+(rt->rows+2)*25/2)-25;
            float x=(i*wdt-(rt->cols-1)*wdt/2);
            s3d_push_vertex(boogieman,xoom*x,yoom*y,0);
        }
}


void resizooo(RoteTerm *t, int x, int y)
{

//	shooboogieman(t);

    rote_vt_resize(t, t->rows+y,t->cols+x);
    lines_r_not_clean(t);//4 valgrinds unitialized value blahblahs

    pokeboogieman(t);
}


static int keypress(struct s3d_evt *event)
{
    struct s3d_key_event *keys = (struct s3d_key_event *)event->buf;
    int key=keys->keysym;
    int mod=keys->modifier;
    if (mod&S3D_KMOD_RSHIFT)
    {
        switch (key)
        {
        case S3DK_INSERT:
            clip(mod&S3D_KMOD_RCTRL);
        }
    }

    if (mod&S3D_KMOD_RCTRL)
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
        case S3DK_F7:
            afterlife=!afterlife;
            break;
        case S3DK_F8:
            loadl2("l2");
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
    else if (mod&S3D_KMOD_RALT)
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
            {    }
        else

            if ( (key >= S3DK_F1) && (key <= S3DK_F15) )
            {
                char *k;
                if (asprintf(&k ,"kf%i", key-S3DK_F1+1)!=-1)
                {
                    rote_vt_terminfo(t, k);
                    free(k);
                }
            }
            else if ( (key == S3DK_SPACE) )
                keyp(t,32);
            else if ( (key == S3DK_BACKSPACE) )
                rote_vt_terminfo(t, "kbs");
            else if ( (key == S3DK_ESCAPE) )
                keyp(t,27);
            else if ( (key == S3DK_LEFT) )
                rote_vt_terminfo(t, "kcub1");
            else if ( (key == S3DK_RIGHT) )
                rote_vt_terminfo(t, "kcuf1");
            else if ( (key == S3DK_UP) )
                rote_vt_terminfo(t, "kcuu1");
            else if ( (key == S3DK_DOWN) )
                rote_vt_terminfo(t, "kcud1");
            else if ( (key == S3DK_END) )
                rote_vt_terminfo(t, "kend");
            else if ( (key == S3DK_HOME) )
                rote_vt_terminfo(t, "khome");
            else if ( (key == S3DK_DELETE) )
                rote_vt_terminfo(t, "kdch1");
            else if ( (key == S3DK_PAGEDOWN) )
                rote_vt_terminfo(t, "knp");
            else if ( (key == S3DK_INSERT) )
                rote_vt_terminfo(t, "kich1");
            else if ( (key == S3DK_PAGEUP) )
                if (mod&S3D_KMOD_RSHIFT)
                    scrollup();
                else
                    rote_vt_terminfo(t, "kpp");
            else if ( (key == S3DK_RETURN) )
            {
                if (mod&(S3D_KMOD_RSHIFT|S3D_KMOD_LSHIFT))
                    keyp(t,10);
                else
                    keyp(t,13);
            }
            else if (mod&(S3D_KMOD_RALT|S3D_KMOD_LALT))
            {
                char x[2];

                x[0]=27;
                x[1]=keys->keysym;
                rote_vt_write(t, &x[0], 2);//thx twkm

            }
            else
            {
                if ( (key==S3DK_LSHIFT)||(key==S3DK_RSHIFT))
                {
                    globmousecopying=!keys->state;
                }
                else
                {
                    keyp(t, keys->unicode);
//					    	printf("%c, %i\n",  keys->keysym,  keys->unicode);
                }
            }
    }
}

void mousez(char s, int x, int y)
{
    if (t->docellmouse)
    {
        char xy[6];
        xy[0]=27;
        xy[1]='[';
        xy[2]='M';
        xy[3]=s;
        xy[4]=33+x;
        xy[5]=33+y;
        rote_vt_write(t, xy, 6);//thx void
    }
}



static int but(struct s3d_evt *ev)
{
    if ((((struct s3d_but_info*)ev->buf)->button==0)&&
            (((struct s3d_but_info*)ev->buf)->state==1))
    {
        //s3d_flags_off(mouse, S3D_OF_VISIBLE);
        butt=0;
        if (globmousecopying)
            copy();
        else
            mousez('#', mex,mey);
    }
    /* 0 = down, 1 = up, 2 = moving */
}

float x2s3d(float x)
{
    return (-t->cols*10/2+x)*xoom;
}
float y2s3d(float y)
{
    return (-t->rows*26/2+y)*-yoom;
}

void showsel(void)
{
    if (sel)
    {
        s3d_flags_on(selob, S3D_OF_VISIBLE);
        if (selpop)
        {
            s3d_pop_vertex(selob, 7);
        }
        selpop=1;

        s3d_push_vertex(selob, x2s3d(13+msx*10), y2s3d(msy*26), 0);
        s3d_push_vertex(selob, x2s3d(13+msx*10)-10, y2s3d(msy*26), 0);
        s3d_push_vertex(selob, x2s3d(13+msx*10)-5, y2s3d(msy*26), 0);
        s3d_push_vertex(selob, x2s3d(13+mex*10)-5, y2s3d(13+mey*26), 0);
        s3d_push_vertex(selob, x2s3d(13+msx*10)-5, y2s3d(26+msy*26), 0);
        s3d_push_vertex(selob, x2s3d(13+msx*10)-10, y2s3d(26+msy*26), 0);
        s3d_push_vertex(selob, x2s3d(13+msx*10), y2s3d(26+msy*26), 0);
    }
    else
        s3d_flags_off(selob, S3D_OF_VISIBLE);
}


void updatemouse(void)//moving while button down
{
    mex=mousex/10;
    mey=mousey/26;
    if (mex<0)mex=0;
    if (mey<0)mey=0;
    if (mex>=t->cols)mex=t->cols-1;
    if (mey>=t->rows)mey=t->rows-1;
    mousez('@', mex,mey);
    if ((mex!=msx)||(mey!=msy))sel=1;
    showsel();
    copy();
}

static int click(struct s3d_evt *ev)
{
    int x,y,p;
    struct pclick_event *e;
    e =(struct pclick_event*) ev->buf;
    if (ntohl(e->o)==boogieman)
    {
        p=ntohl(e->p);
        p/=2;
        msx=x=p / boogiemanh;
        msx=y=p % boogiemanh;
        t->cells[y][x].ch='*';
        dirty=1;
        mousex=x*10+13;
        mousey=y*26+13;
        mousez(' ', x,y);
        butt=1;
    }
}

static int camcamcam(struct s3d_evt *e)
{
    struct s3d_obj_info *event;
    event =(struct s3d_obj_info*) e->buf;
    float zx,zy;

    if (!(strncmp(event->name, "sys_pointer0",12)))
    {
        if (butt)
        {
            printf("but %f %f %f\n", event->trans_x, event->trans_y, event->trans_z);
            mousex=((event->trans_x+1)/2)*t->cols;
            mousey=((event->trans_y+1)/2)*t->rows;
            updatemouse();
        }
    }

    if (!(strncmp(event->name, "sys_cam",7)))
    {
        //(
        printf("%s  | ,",event->name);
        printf("%f\n", event->scale);
        //)
        if (event->scale==1)
            zx=zy=1;
        else	if (event->scale<1)//its tall
        {
            zx=1;
            zy=1/event->scale;
        }
        else	if (event->scale>1)
        {
            zy=1;
            zx=event->scale;
        }
        zoomx=zx*12.5;
        zoomy=zy*10;
        yoom=zoomy/t->rows/26;
        xoom=zoomx/t->cols/13;

//	shooboogieman(t);
//	pokeboogieman(t);
        reshapeboogieman(t);
        dirty=1;// mainloop();
    }
    return(0);
}

static int init(struct s3d_evt *ev)
{
    generic=strcmp(ev->buf, "\0\1\2kook s3d server");
}

int main(int a, char **v)
{
    if (s3d_init(&a,&v,"test"))
    {
        printf("opsie\n");
        s3d_usage();
        exit(1);
    }

    tex=s3d_new_object();
    selob=s3d_new_object();
    boogieman=s3d_new_object();
    s3d_flags_on(boogieman, S3D_OF_SELECTABLE);//|S3D_OF_VISIBLE);

    float bla[12]=
        {0, 0, 0, 1,
         0, 0, 0, 1,
         0, 0, 0, 1
        };
    float wla[12]=
        {0, 1, 0.1, 1,
         0, 0, 0.1, 1,
         0, 0, 0.1, 1
        };

    if ((a==2)&&!strcmp(v[1],"r"))
    {
        bla[0]=bla[3]=bla[6]=1;
        bla[3]=bla[7]=bla[11]=1;

    }
    else if ((a==2)&&!strcmp(v[1],"g"))
    {
        bla[1]=bla[4]=bla[7]=1;
        bla[3]=bla[7]=bla[11]=1;

    }
    else if ((a==2)&&!strcmp(v[1],"b"))
    {
        bla[2]=bla[5]=bla[8]=1;
        bla[3]=bla[7]=bla[11]=1;
    }
    else
        bla[0]=bla[1]=bla[2]=bla[3]=bla[4]=bla[5]=bla[6]=bla[7]=bla[8]=bla[9]=bla[10]=bla[11]=1;


    s3d_push_materials_a(boogieman,bla, 1);
    s3d_push_materials_a(boogieman,wla, 1);
    s3d_push_materials_a(selob,bla, 1);

    s3d_push_line(selob, 0,1,0);
    s3d_push_line(selob, 1,2,0);


    bla[0]=bla[1]=bla[2]=bla[3]=bla[4]=bla[5]=bla[6]=bla[7]=bla[8]=bla[9]=bla[10]=bla[11]=1;
    s3d_push_materials_a(tex,bla, 1);
    bla[0]=bla[1]=bla[2]=bla[3]=bla[4]=bla[5]=bla[6]=bla[7]=bla[8]=bla[9]=bla[10]=bla[11]=1;
    s3d_push_materials_a(tex,bla, 1);

    int b;
    int c;//31
    for (b=1; b<9; b++)
    {
        bla[0]=b/8.0;
        s3d_push_materials_a(tex,bla, 1);
    }
    bla[0]=1;
    for (b=9; b<17; b++)
    {
        bla[1]=9+b/8.0;
        s3d_push_materials_a(tex,bla, 1);
    }
    bla[1]=1;
    for (b=17; b<25; b++)
    {
        bla[2]=17+b/8.0;
        s3d_push_materials_a(tex,bla, 1);
    }
    bla[2]=1;
    for (b=25; b<32; b++)
    {
        bla[2]=bla[1]=25+b/7.0;
        s3d_push_materials_a(tex,bla, 1);
    }
    
    t=rote_vt_create(30,120);
    //lines_r_clean(t);
    rote_vt_forkpty(t,"sh");
    dirty=1;

    zoomx=16.666;
    zoomy=10;
    float byoom=zoomy/t->rows;
    float bxoom=zoomx/t->cols;
    yoom=byoom/26;
    xoom=bxoom/13;

    pokeboogieman(t);

    s3d_flags_on(tex, S3D_OF_VISIBLE);
    s3d_set_callback(S3D_EVENT_QUIT, stop);
    s3d_set_callback(S3D_EVENT_KEY, keypress);
    s3d_set_callback(S3D_EVENT_OBJ_PCLICK, click);
    s3d_set_callback(S3D_EVENT_OBJ_INFO, camcamcam);
    s3d_set_callback(S3D_EVENT_MBUTTON, but);
//    s3d_set_callback(S3D_EVENT_INIT, init);

    loadl2("l2");
    initbufs();
    s3d_mainloop(mainloop);
    rote_vt_destroy(t);
}
