#include "s3d.h"
#include "s3d_keysym.h"
#include "stdio.h"
#include "stdlib.h"
#include "../gltext.c"
#define hfgm 1
int o[26][26];
int pos=124;
int popl=0;
int popv=0;
int apopv=0;
int apopl=0;
int * txt=0;
int aoo;
int oo;
float s=6.0/26.0;/*tile height*/
float sw;
int aim=-1;
int ltxt=-1;
int griddist=0.00002;
float px,py;

char * trytorealloc(char * chacha, int y)
{
    char * tmp;
    tmp=realloc(chacha, y);
    if(tmp)
	return tmp;
    return  chacha;
}

void p(char * x)
{
    printf(x);printf("\n");
}
void showpos(void)
{
    printf("pos %i\n", pos);
}
void showaim(void)
{
    printf("aim %i\n", aim);
}


void dispaim(float ptx, float pty)
{
				float w=0.3;
				float t=0.1;
				float tx=-t+sw*ptx+0.5*sw;
				float wy=-w-s*pty+0.5*s;
				float wx=-w+sw*ptx+0.5*sw;
				float ty=-t-s*pty+0.5*s;
				s3d_push_vertex(aoo, tx,wy,griddist);
				s3d_push_vertex(aoo, wx,wy,griddist);
				s3d_push_vertex(aoo, wx,ty,griddist);
				s3d_push_line(aoo, 0,1,0);
				s3d_push_line(aoo, 1,2,0);
				 tx=t+sw*ptx+0.5*sw;
				 wy=-w-s*pty+0.5*s;
				 wx=w+sw*ptx+0.5*sw;
				 ty=-t-s*pty+0.5*s;
				s3d_push_vertex(aoo, tx,wy,griddist);
				s3d_push_vertex(aoo, wx,wy,griddist);
				s3d_push_vertex(aoo, wx,ty,griddist);
				s3d_push_line(aoo, 3,4,0);
				s3d_push_line(aoo, 4,5,0);
				 tx=t+sw*ptx+0.5*sw;
				 wy=w-s*pty+0.5*s;
				 wx=w+sw*ptx+0.5*sw;
				 ty=t-s*pty+0.5*s;
				s3d_push_vertex(aoo, tx,wy,griddist);
				s3d_push_vertex(aoo, wx,wy,griddist);
				s3d_push_vertex(aoo, wx,ty,griddist);
				s3d_push_line(aoo, 6,7,0);
				s3d_push_line(aoo, 7,8,0);
				 tx=-t+sw*ptx+0.5*sw;
				 wy=w-s*pty+0.5*s;
				 wx=-w+sw*ptx+0.5*sw;
				 ty=t-s*pty+0.5*s;
				s3d_push_vertex(aoo, tx,wy,griddist);
				s3d_push_vertex(aoo, wx,wy,griddist);
				s3d_push_vertex(aoo, wx,ty,griddist);
				s3d_push_line(aoo, 9,10,0);
				s3d_push_line(aoo, 10,11,0);
				apopl=8;
				apopv=12;
}

void donoob(int m)
{
			s3d_push_vertex(oo, -sw,s,0.1);
			s3d_push_vertex(oo, sw*27,-s*27,0.1);
			s3d_push_line(oo, 0, 1,m );
			popl++;popv=2;
}


void disp(void)
{
    if(popl)
    {
	s3d_pop_line(oo, popl);
	printf("pop line\n");
    }
    if(popv)
    {
	s3d_pop_vertex(oo, popv);
	printf("pop vert\n");
    }
    if(apopl||apopv)
    {
	s3d_pop_line(aoo, apopl);
	s3d_pop_vertex(aoo, apopv);
    }
    apopl=apopv=0;    
    popl=popv=0;
#ifdef hfgm
    if(ltxt!=-1)
	s3d_flags_off(ltxt, S3D_OF_VISIBLE);
    if(txt&&(pos<128)&&txt[pos])
    {
	ltxt=txt[pos];
	s3d_flags_on(txt[pos], S3D_OF_VISIBLE);
    }
#endif
    char *x=l2[pos];
    printf("editoing %c (%x)\n",pos,pos);
    printf("|TEST--->\n");
    int y=x[0];
    printf("%i\n", y);
    printf("<---checkoing compexity...\n");
    int c=0;
    while(x[c])
    {
        printf(".");
        if(((x[c])>'z')||((x[c])<'a'))
        {
		printf("d:too complex:[0x%x]\n",x[c]);
		donoob(0);
		return;
	}
	c++;
    }
    c=0;
    while((x[c]))
    {
	    float ptx=x[c++]-'a';
	    float pty=x[c++]-'a';
	    if((popv==aim)||(!x[c]&&(aim>popv)))
	        dispaim(ptx,pty);
	    s3d_push_vertex(oo, sw*ptx+0.5*sw,-s*pty+0.5*s,0.1);
	    if(popv)
	    {
		s3d_push_line(oo, popv-1, popv,1 );
		popl++;
	    }
	    popv++;
    }
    if (!popv)
	donoob(1);
}


int click(struct s3d_evt *e)
{
    int g = (int) * ((unsigned long *)e->buf);
    int i,j;
    for (i=0;i<26;i++)
    for (j=0;j<26;j++)
    if(g==o[i][j])
    {
		char *x=l2[pos];
		printf("editing pos %i (%s) ( ",pos,x);
		int c=0;
		while(x[c])
		{
		    printf("%x,",x[c]);c++;
		}
		printf("\b)\n");
		c=0;
		while(x[c])
		{
		    if(((x[c])<'a')||((x[c])>'z'))
		    {
			printf("e:too complex\n");
			return;
		    }
		    c++;
		}
		c=0;
		if((aim>=0)&&(strlen(x)/2>aim))
		{
		    x[aim*2]=i+'a';
		    x[aim*2+1]=j+'a';
		    printf("moving point %i to %c%c\n" , aim, 'a'+i, 'a'+j);

		}
		else if(aim>-1)
		{
		    int l=strlen(x)+2+1;
		    x=realloc(l2[pos], l);
		    if(!x){free(l2[pos]);return 0;}
		    printf("adding point %c%c\n", 'a'+i, 'a'+j);
		    x[l-3]=i+'a';
		    x[l-2]=j+'a';
		    x[l-1]=0;
		    l2[pos]=x;
		    printf("x is now %s, len %i", x, strlen(x));
		}
		disp();
		return(0);
    }
}

void del(void)
{
		if(!x[0]||(aim==-1))return ;
		char *x=l2[pos];
		printf("deleting from pos %i (%s) ( ",pos,x);
		int c=0;
		while(x[c])
		    printf("%x,",x[c++]);
		printf("\b)\n");
		c=0;
		while(x[c])
		{
		    if(((x[c])<'a')||((x[c])>'z'))
		    {
			printf("e:too complex\n");
			return;
		    }
		    c++;
		}
		c=0;
		if(1)
		{
		    int y=strlen(x);
		    if(y>aim*2+2)
		    {
			printf("deling from inside\n");
			memmove(&x[aim*2], &x[aim*2+2],y-aim*2+1);
		    }
		    trytorealloc(x, y);
		}
		disp();
}



void go_left(void)
{
    pos--;
    if (pos<0)pos=l2numitems-1;
    disp();
    showpos();

}
void go_right(void)
{
    pos++;
    if(pos==l2numitems)pos=0;
    disp();
    showpos();
}
void aim_up(void)
{
    aim--;
    if (aim<0)aim=0;
    disp();
    showaim();
}
void aim_down(void)
{
    aim++;
    disp();
    showaim();
}

void clr(void)
{
    l2[pos]=realloc(l2[pos],1);
    l2[pos][0]=0;
    disp();
}

int keypress(struct s3d_evt *event)
{
    struct s3d_key_event *keys = (struct s3d_key_event *)event->buf;
    int key=keys->keysym;    
    int mod=keys->modifier;
	switch (key)
	{
		case S3DK_LEFT:
		    go_left();
		break;
		case S3DK_RIGHT:
		    go_right();
		break;
		case S3DK_UP:
		    aim_up();
		break;
		case S3DK_DOWN:
		    aim_down();
		break;
		case S3DK_RETURN:
		    savel2();
		break;
		case S3DK_DELETE:
		    del();
		break;
		case S3DK_c:
		    clr();
		break;
	}
}

void mainloop(void)
{
   usleep(1000); // sleep is good
}

int stop_quit(struct s3d_evt *event)
{
    s3d_quit();
    return(0);
}

int main(int a, char **v)
{
    p("Hi");
    if(s3d_init(&a,&v,"test"))
    {
	printf("opsie\n");
	s3d_usage();
    }
    else
    {
	float mw=1;
	sw=s/2.0;
	px=-26*sw;
	py=13*s;

#ifdef hfgm
	
	if (s3d_select_font("courier new")) {
		printf("font not found\n");
	}else
	{
	    if((txt=malloc(128*4)))
	    {
		int i;
		float w;
		char buf[2];
	        buf[1]=0;
	        for (i=0;(i<128);i++)
	        {
		    buf[0]=i;
	    	    txt[i]=s3d_draw_string(buf,&w);
	    	    if(txt[i])
	    	    {
	    	        if(w>mw)mw=w;
			s3d_scale(txt[i], 5);
		    }
		}
		px=-mw/2;
		for (i=0;(i<128);i++)
		    s3d_translate(txt[i],px,py-4,0.00001);
	    }
	}
#endif

	sw=mw*s;//s/2.0;
	
	float red[12]=
	{1, 0, 0, 1,
         1, 0, 0, 1,
         1, 0, 1, 1};
	float black[12]=
	{0, 0, 0, 1,
         0, 0, 0, 1,
         0, 0, 0, 1};
	float teal[12]=
	{0, 1, 1, 1,
         1, 1, 0, 1,
         0, 0, 1, 1};
	 
	 
	oo=s3d_new_object();
	s3d_translate(oo, px,py,0);

	s3d_push_materials_a(oo,red, 1);
	s3d_push_materials_a(oo,teal,1);

	s3d_flags_on(oo, S3D_OF_VISIBLE);
	aoo=s3d_new_object();
	s3d_translate(aoo, px, py,0);
	s3d_push_materials_a(aoo,red, 1);

	s3d_flags_on(aoo, S3D_OF_VISIBLE);

	showpos();
	loadl2();
	disp();
	 
	int b,c;
	for(b=0;b<26;b++)
	{
	    black[0]=black[4]=black[8]=b/(float)26;
	    for(c=0;c<26;c++)
	    {
	        black[1]=black[5]=black[9]=(float)c/26;
	        int ob=s3d_new_object();
	        o[b][c]=ob;
    		s3d_translate(ob, px+sw*b, py-s*c,0);
		s3d_push_materials_a(ob,black, 1);
		float vs2[12]={0,0,0,sw,0,0,sw,s,0,0,s,0};
		s3d_push_vertices(ob, vs2, 4);
		if(0)
	        {
	    	    s3d_push_materials_a(ob,red, 1);
		    float vs[12]={0,0,griddist,sw,0,griddist,sw,s,griddist,0,s,griddist};
	    	    s3d_push_vertices(ob, vs, 4);
	    	    int ls[12]={0,1,1,1,2,1,2,3,1,3,0,1};
	    	    s3d_push_lines(ob,ls,4);
		    int ps[8]={4,6,5,0,4,7,6,0};
		    s3d_push_polygons(ob, ps, 2);
		}
		else
		{
		    int ps[8]={0,2,1,0,0,3,2,0};
		    s3d_push_polygons(ob, ps, 2);
		}
		s3d_flags_on(o[b][c], S3D_OF_VISIBLE|S3D_OF_SELECTABLE);
    	    }
	}

	s3d_set_callback(S3D_EVENT_OBJ_CLICK, click);
	s3d_set_callback(S3D_EVENT_QUIT, stop_quit);
	s3d_set_callback(S3D_EVENT_KEY, keypress);
	s3d_mainloop(mainloop);
    }
}
