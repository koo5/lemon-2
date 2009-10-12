int glimits=0; // look 4 lowest / leftest / highest / rightest
float lx,ly,lw,lh; //  rendered point while rendering
char ** l2; // non-builtin
int l2numitems=0;// font
float rastio=0.8;

void freel2(void)
{
    if(!l2)return;
    printf("freeinG font\n");
    int t;
    for (t=0;t<l2numitems;t++)
    {
	printf("%i\n",t);
	free(l2[t]);
    }
    free(l2);
    l2numitems=0;
    l2=0;
}

void loadl2(void)
{
    FILE * fp = fopen("l2","r");
    if (fp == NULL) 
    {
	printf("no go\n");
	return;
    }
    freel2();
    int l2size=100;
    size_t s=sizeof(char *) * 100;
    l2=malloc(s);
    if(!l2)
    {
	printf("no place 4 %i byttrez\n",s);
	return;
    }
    size_t len;
    l2numitems=0;
    while((l2[l2numitems]=fgetln(fp, &len)))
    {
	l2[l2numitems][len-1 >= 0 ? len-1 : 0]=0; // hrrrm turning newline into 0
	l2numitems++;
	if (!(l2numitems%100))
	{
	    char **tmp=realloc(l2,sizeof(char *) *(l2size+100));
	    if(!tmp)
		return;
	    l2=tmp;
	    l2size+=100;
	}
    }
    fclose(fp);
}

int red;
int bold;
int halflight;
int selectedtext;
int activetext;
int rotor;
int niceunilines=1;
char *trmsqrm=0;

static char * sgns[] = {
"dzdptuzpznxnxpzp", /*\0*/
"ogooaokkktao",/*\n*/
"oaaooz",/*<*/
"azzoaazo",/*>*/
"akzk    aszs",/*=*/
"aazzooazzaoooaozooaozo",/***/
"@llss          owaF",/*;*/
"saohnoossz",	/*(*/
"jaohqoosjz",	/*)*/
"gzwa",	/*/*/
"luaF",/*,10*/
"aooooaozoozo",	/*+*/
"dowo",/*-*/
"dxxx",	/*_*/
"mwmzqzqwmw",/*.   14*/
"aoiasoza",/*~*/
"idqdqiiiid  ivqvqqiqiv"/*:*/,
"aooazohohttttr"/*?*/,
""/* 18*/,
"gawz",/*\*/
"oaoz"/*|*/,
"oaozoozo"/*|-*/,
"hovnowho"/*v22*/,
"oaoz  hsovtvihofti"/*$*/,
"oaaoozzooa  ohos"/*<|>*/,
"-hovnowho"/*v*/
};


char *nums[]={
"@aazz        ",
"aooaoz",
"ahoaazzz",
"aaoaajoraz",
"aooaaozoooohoz",
"azzzzoaoaaza",
"azzzzoaoazaaza",
"aazaaznmamzm",
"azzzzoaoazaazazo",
"azzzzoaoaazazo",};

static char * chrz[] = {
/*A*/"azoazz"
/*B*/,"aaazzsaoziaa"
/*C*/,"azaazaaaazzz"
/*D*/,"azzmaaaz"
/*E*/,"azaazaaaamzmamazzz"
/*F*/,"azaazaaaamzmamaz"
/*G*/,"mommzmzzazaaza"
/*H*/,"azaaamzmzazz"
/*I*/,"mzmamz"
/*J*/,"itmzqtqa"
/*K*/,"azaaamzaamzz"
/*L*/,"aaazzz"
/*M*/,"azaammzazz"
/*N*/,"azaazzza"
/*O*/,"azaazazzaz"
/*P*/,"azaazazmam"
/*Q*/,"zzazaazazzFF"
/*R*/,"azaazaamzz"
/*S*/,"zaaaamzmzzaz"
/*T*/,"aazamamz"
/*U*/,"aaazzzza"
/*V*/,"aamzza"
/*W*/,"aaizmmrzza"
/*X*/,"aazzmmazza"
/*Y*/,"aammmzmmza"
/*Z*/,"aazaazzz"
};

#ifndef GL
#define GL_LINE_STRIP 3
#define GL_LINES                                0x0001

SDL_Surface *gltextsdlsurface;
Uint32 barvicka;
int have_first=0;
void glBegin(int haha)
{
    have_first=0;
}

#define glEnd() 

inline void glColor4f(float r,float g,float b,float a)
{
    if (a==0)
	barvicka=0;
    else
	barvicka=SDL_MapRGBA( gltextsdlsurface->format ,r*255,g*255,b*255,a*255);
}

void dooooot(float x,float y)
{
    static Uint32 firstc;
    static Sint16 firstx, firsty;
        
    if (y<1)y=1;    if (x<1)x=1;
    if(y>gltextsdlsurface->h/rastio-2)y=gltextsdlsurface->h/rastio-2;
    if(x>gltextsdlsurface->w/rastio-2)x=gltextsdlsurface->w/rastio-2;
    if(have_first)
    {
	if(barvicka&&firstc)
	    Draw_Line(gltextsdlsurface,rastio*firstx,rastio*firsty,rastio*x,rastio*y,barvicka);
	firstx=x;	firsty=y;
	firstc=barvicka;
    }
    else
    {
	have_first=1;
	firstx=x;
	firsty=y;
	firstc=barvicka;
    }
}

#else
inline void dooooot(float x,float y)
{
    glVertex2f(x,y);
    if(glimits)
    {
	if(lx>x)lx=x;
	if(ly>y)ly=y;
	if(lw<x)lw=x;
	if(lh<y)lh=y;	
    }
}
#endif

void zspillit(xy lok,char *x,float z)
{
    int first=1; /*were drawin 1st dot*/
    int xdot,ydot;
    float fxdot;
    float fydot;
    int flip2=0;
    int flip=0;
    while ((*x)&&(*(x+1)))
    {
	flip2=flip2||((*x==45)&&x++);if(!(*(x+1)))break; // - 
	flip=flip||((*x==124)&&x++);if(!(*(x+1)))break;  // |
    
	if(((*x)==' ')&&(*(x+1)==' ')) // 2 spaces
	{
		x++;x++;
		glColor4f(0,0,0,0);
		dooooot(lok.x+fxdot,lok.y+fydot);
		first=1;
	}
	else
	{
    	    if(*x==64) // @
    		if((*(x+1))&&(*(x+2))&&(*(x+3))&&(*(x+4))&&(*(x+5))&&(*(x+6))&&(*(x+7))&&(*(x+8))&&(*(x+9))&&(*(x+10))&&(*(x+11))&&(*(x+12)))
    		{
    		    x=strdup(x);
	    	    x[5]=x[3];x[6]=x[2];x[7]=x[1];x[8]=x[2];x[9]=x[1];x[10]=x[4];x[11]=x[3];x[12]=x[4];
		    x++;x++;x++;
		}
	    xdot=*x;
	    x++;
	    ydot=*x;
	    xdot-='a'; 
	    ydot-='a';
	    if (xdot<0) // capital
		xdot+=58;
	    if (ydot<0)
		ydot+=58;
		
	    ydot=ydot-13;
    	    xdot=xdot-13;
    	    fydot=ydot*(z+(2*z*(0-flip2))); //zoom and flip
	    fxdot=xdot*0.5*(z+(2*z*(0-flip))); //always shrink width to half
    	    fxdot=fxdot+13;
    	    fydot=fydot+13; 

	    if (first)
	    {
	    	first=0;
		glColor4f(0,0,0,0);
		dooooot(lok.x+fxdot,lok.y+fydot);
		
		if (selectedtext)
		    glColor4f(1,1,1,1);
		else if (red)
		    glColor4f(1,0,0,1);
		else if (activetext)
		    glColor4f(0,1,1,1);
		else if (halflight&&bold)
		    glColor4f(0,0.2,0.2,1);
		else if (halflight)
		    glColor4f(0,0,0.5,1);
		else if (bold)
		    glColor4f(0,0.95,0.5,1);
		else
		    glColor4f(0,1,0,1);

	    }
	    x++;
	    if (!*x)/*last dot*/
	    {
    		dooooot(lok.x+fxdot,lok.y+fydot);
		glColor4f(0,0,0,0);
    		dooooot(lok.x+fxdot,lok.y+fydot);
		return; 
	    }
	    else // just ordinary dot
	    {
    		dooooot(lok.x+fxdot,lok.y+fydot);
	    }
	}
    }
     
}
static void spillit(xy lok,char *x)
{
    zspillit(lok,x,1);
}




static xy draw(xy lok,unsigned int i)
{
    xy nlok;
    nlok=lok;
    nlok.x+=30;

    if(l2)
	if(i<l2numitems)
	    if(l2[i])
		if(strlen(l2[i]))
		{
		    spillit(lok,l2[i]);
		    return nlok;
		}

    if (!((i<65)||((i>90)&&(i<97))||(i>122)))
    {
	if ((i>='A')&&(i<='Z'))
	    zspillit(lok,chrz[i-'A'],1);
	else
	{
//	    lok.x-=6;
	    nlok.x-=6;
	    zspillit(lok,chrz[i-'a'],0.5);
	}
	return nlok;
    }
    else
    if((i>47)&&(i<58))
    {
	nlok.x=lok.x+26;
	zspillit(lok, nums[i-48],0.8); 
	return nlok;
    }
    else
    if (i==32)
	return nlok;
    else
    if(i==37)
	spillit(lok, "azza  @ddgg        -|  @ddgg        "); 
    else
    if(i==283)
    {
	draw(lok, 'e');
	spillit(lok, "haofxa");
    }
    else
    if(i==353)
    {
	draw(lok, 's');
	spillit(lok, "haofxa");
    }
    else
    if(i==269)
    {
	draw(lok, 'c');
	spillit(lok, "haofxa");
    }
    else
    if(i==345)
    {
	draw(lok, 'r');
	spillit(lok, "haofxa");
    }
    else
    if(i==382)
    {
	draw(lok, 'z');
	spillit(lok, "haofxa");
    }
    else
    if(i==253)
    {
	draw(lok, 'y');
	spillit(lok, "ofxa");
    }
    else
    if(i==225)
    {
	draw(lok, 'a');
	spillit(lok, "ofxa");
    }
    else
    if(i==237)
    {
	draw(lok, 'i');
	spillit(lok, "ofxa");
    }
    else
    if(i==233)
    {
	draw(lok, 'e');
	spillit(lok, "ofxa");
    }
    else
    if(i==201)
    {
	draw(lok, 'E');
	spillit(lok, "ofxa");
    }
    else
    if(i==282)
    {
	draw(lok, 'E');
	spillit(lok, "haofxa");
    }
    else
    if(i==344)
    {
	draw(lok, 'R');
	spillit(lok, "haofxa");
    }
    else
    if(i==356)
    {
	draw(lok, 'T');
	spillit(lok, "haofxa");
    }
    else
    if(i==221)
    {
	draw(lok, 'Y');
	spillit(lok, "ofxa");
    }
    else
    if(i==218)
    {
	draw(lok, 'U');
	spillit(lok, "ofxa");
    }
    else
    if(i==205)
    {
	draw(lok, 'I');
	spillit(lok, "ofxa");
    }
    if(i==211)
    {
	draw(lok, 'O');
	spillit(lok, "ofxa");
    }
    else
    if(i==243)
    {
	draw(lok, 'o');
	spillit(lok, "ofxa");
    }
    else
    if(i==193)
    {
	draw(lok, 'A');
	spillit(lok, "ofxa");
    }
    else
    if(i==352)
    {
	draw(lok, 'S');
	spillit(lok, "haofxa");
    }
    else
    if(i==270)
    {
	draw(lok, 'D');
	spillit(lok, "haofxa");
    }
    else
    if(i==381)
    {
	draw(lok, 'Z');
	spillit(lok, "haofxa");
    }
    else
    if(i==268)
    {
	draw(lok, 'C');
	spillit(lok, "haofxa");
    }
    else
    if(i==327)
    {
	draw(lok, 'N');
	spillit(lok, "haofxa");
    }else
    if(i==328)
    {
	draw(lok, 'n');
	spillit(lok, "haofxa");
    }
    
    else
    if(i==38)
	zspillit(lok, "zzaggangarozzr",0.9); else
    if(i==34)
	spillit(lok, "iaig  qaqg"); else
    if(i==35)
	spillit(lok, "hahz  uauz  ahzh  auzu"); else
    if(i==33)
	spillit(lok, "oaoq  ouoz"); else
    if(i==64)
	spillit(lok, "ozaooaxoovkoohov"); else
    if(i==123)
	spillit(lok, "gadddlaodrdwgz"); else
    if(i==125)
	spillit(lok, "|gadddlaodrdwgz"); else
    if(i==8217)
	spillit(lok, "kfma"); else
    if(i==91)
	spillit(lok, "oddddwow"); else
    if(i==93)
	spillit(lok, "|oddddwow"); else
    if(i==94)
	spillit(lok, "ahoazh"); else
    if(i==39)
	spillit(lok, "oaoj"); else
    if(i==96)
	spillit(lok, "mcqg"); else
    if(i==92)
	spillit(lok, sgns[19]); else
    if((i==9492))
	spillit(lok, "oaoozo");else
    if((i==9488))
	spillit(lok, "aooooz"); else
    if((i==9496))
	spillit(lok, "aooooa"); else
    if((i==9484))
	spillit(lok, "ozoozo"); else
    if(i==9672)
	spillit(lok, sgns[24]); else 
    if(i==36)
	spillit(lok, sgns[23]); else 
    if(i==9662)
	spillit(lok, sgns[22]); else
    if(i==9652)
	spillit(lok, sgns[25]); else
    if(i==9500)
	spillit(lok, sgns[21]); else
    if(i==9472)
	zspillit(lok, "aozo",1); else
    if(i==9474)
	zspillit(lok, "oaoz",1); else
    if(i==9618)
	zspillit(lok, "aszsoaas  -aszsoaas",1); else
    if(i==32)
	spillit(lok, sgns[18]); else 
    if(i==63)
	spillit(lok, sgns[17]); else 
    if(i==58)
	spillit(lok, sgns[16]); else 
    if(i==126)
	spillit(lok, sgns[15]); else
    if(i==46)
	spillit(lok, sgns[14]); else
    if(i==95)
	spillit(lok, sgns[13]); else
    if(i==45)
	spillit(lok, sgns[12]); else
    if(i==43)
	spillit(lok, sgns[11]); else
    if(i==44)
	spillit(lok, sgns[10]); else
    if(i==47)
	spillit(lok, sgns[9]); else
    if(i==41)
	spillit(lok, sgns[8]); else
    if(i==40)
	spillit(lok, sgns[7]); else
    if(i==59)
	spillit(lok, sgns[6]); else
    if(i==42)
	spillit(lok, sgns[5]); else
    if (i==61)
	spillit(lok, sgns[4]);	else
    if (i==62)
	spillit(lok, sgns[3]);	else
    if (i==60)
	spillit(lok, sgns[2]); else
    if (i==10)
	spillit(lok, sgns[1]); else
    if (!i)
	spillit(lok, sgns[0]);  else
    if(i==124)
	spillit(lok, sgns[20]);/*else 

    {
        zspillit(lok,chrz['w'-'a'],1);
        zspillit(lok,chrz['t'-'a'],1);
        zspillit(lok,chrz['f'-'a'],1);
    }
    */
}



void draw_text(char *a)
{
    if(!a)return;
  xy lok;
  lok.x=0;
  lok.y=0;
  glBegin(GL_LINE_STRIP);
  do 
  {
	lok=draw(lok,*a);
	if (*a==10)
	{
	    lok.x=0;
	    lok.y=lok.y+30;
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	}
	lok.x+=4;
	if (!*a)
	    break;
	a++;
  }
  while(1);
  glEnd();
}

void draw_line(int x,int y,const char *a)
{
  xy lok;
  lok.x=x;
  lok.y=y;
  glBegin(GL_LINE_STRIP);
  do 
  {
	lok=draw(lok,*a);
	lok.x+=4;
	if (!*a)
	    break;
	a++;
  }
  while(1);
  glEnd();
}

void lokdraw_line(xy lok,const char *a)
{// of text
glBegin(GL_LINE_STRIP);
do 
{
	lok=draw(lok,*a);
	lok.x+=4;
	if (!*a)
	    break;
	a++;
}
while(1);
glEnd();
}



/*
void draw_file(char *a)
{
    char * zumzum = 0;
    if(zumzum = GetFileIntoCharPointer1(a))
	draw_text(zumzum);
}

*/


int min(int a, int b)
{
    return a < b ? a : b;
}

int in(int a, int b, int c)
{
    return(a<=b&&c>=b);
}

//kinda rough , not used yet
void getlimits(RoteTerm *rt, limits *l)
{
    l->x=0;
    l->y=0;
    l->x2=rt->cols*13;
    l->y2=rt->rows*26;
}

int tscroll=0;
void draw_terminal(roteface *f)
{
    static int oldcrow, oldccol;
    RoteTerm * rt=f->t;
#ifdef GL
    lx=0;ly=0;lw=0;lh=0; glimits=1;
    
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
#endif
    xy lok;
    lok.x=0;    lok.y=0;
    int i,j;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=i*26;
	int gotlog=1;
	for (j=0; j<rt->cols; j++)
	{
	    lok.x=j*13;
	    if(!tscroll)//draw_edges_between_different_bgs
	    {
		halflight=1;
		if((j>0))
		    if((ROTE_ATTR_BG(rt->cells[i][j-1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaaz",1);
		if((j<rt->cols-1))
		    if((ROTE_ATTR_BG(rt->cells[i][j+1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"zazz",1);
		if((i<rt->rows-1))
		    if((ROTE_ATTR_BG(rt->cells[i+1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"azzz",1);
		if((i>0))
		    if((ROTE_ATTR_BG(rt->cells[i-1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaza",1);
		halflight=0;
	    }
	    bold=(!tscroll)&&((rt->cells[i][j].attr)&128);
	    	    
	    selectedtext=(!tscroll)&&((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor
#ifdef GL
	    if(selectedtext) //do nice rotation animation hehe
	    {
		zspillit(lok,nums[0],1.2);
		if((oldcrow!=rt->crow)||(oldccol!=rt->ccol))
		    rotor=0;
		zspillit(lok,nums[0],2);//cursor
		glEnd();
		glPushMatrix();
		    glTranslatef(lok.x+13,lok.y+13,0);
		    glPushMatrix();
		    glRotatef(rotor+=17,0,1,0);
		    glBegin(GL_LINE_STRIP);
			xy molok;molok.x=-13;molok.y=-13;
			draw(molok,rt->cells[i][j].ch);
		    glEnd();
		    glPopMatrix();
		glPopMatrix();
		glBegin(GL_LINE_STRIP);
	    }
	    else
#else
	    if(selectedtext)
	    {	// but still cursor square
		zspillit(lok,nums[0],1.2);
	    }

#endif
	    {
		if(!rt->log||!tscroll)
		    draw(lok,rt->cells[i][j].ch);
		else
		    if(in(rt->scrolltop,i,rt->scrollbottom))
			if(rt->logl=i-rt->scrolltop)
			    {
				int r= i-tscroll;
				if(r<0)
				{
				    if(tscroll-i-rt->scrolltop<rt->logl)
				    {
					//if(gotlog)gotlog=gotlog&&rt->log[tscroll-i-rt->scrolltop][j].ch;
					//if(gotlog)
				        //draw(lok,rt->log[tscroll-i-rt->scrolltop][j].ch);
				        printf("%i,%i\n", tscroll-i-rt->scrolltop,j);
				    }
				}
				else
				    draw(lok,rt->cells[i-tscroll][j].ch);
			    }
		
		
	    }
	}

    }
    trmsqrm=0; // message under terminal window
    if (!rt->childpid)
    {
	activetext=1;
	trmsqrm="HDD full";
    }
	
    if (trmsqrm)
    {
        lok.x=0;
        lok.y+=30;
//        printf("||||\n");
        lokdraw_line(lok,trmsqrm);//'
    }
#ifdef GL

    glEnd();
    glPopMatrix();
    oldcrow=rt->crow;//4 cursor rotation
    oldccol=rt->ccol;


#endif
    if(1)//return boundariesssssssssssss
    {
	f->lim.x=lx;
	f->lim.y=ly;	
	f->lim.x2=lw;	
	f->lim.y2=lh;		
        glimits=0;
    }	
}


