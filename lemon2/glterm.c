
typedef 
struct 
{
unsigned char r,g,b;
}
color;

char *clfl;
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





int halflight;


void draw_text_az(char *a, double y, double z)
{
    if(!a)return;
  xy lok;
  lok.x=0;
  lok.y=0;
  setcolor(1,1,1,1);
  glBegin(GL_LINE_STRIP);
  do 
  {
	lok=draw(lok,*a,y,z);
	if (*a==10)
	{
	    lok.x=0;
	    lok.y=lok.y+30*z;
	    glEnd();
	    glBegin(GL_LINE_STRIP);
	}
	if (!*a)
	    break;
	a++;
  }
  while(1);
  glEnd();
}

void draw_text(char *a)
{
    draw_text_az(a,1,1);
}


void lokdraw_line(xy lok,const char *a)
{// of text
glBegin(GL_LINE_STRIP);
do 
{
	lok=draw(lok,*a,1,1);
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

typedef struct 
{
    int oldcrow, oldccol;
}draw_terminal_data;


void do_color(int attr,face *f)
{
	    int color=((attr));
	    int c=ROTE_ATTR_XFG(color);//0-15

//	    printf("%i\n",c);
	    
	    switch (f->theme)
	    {
	    case 0:
	        setcolor(1,color/255.0,color/255.0,1);break;
	    case 1:
	        setcolor(color/255.0,1,color/255.0,1);break;
	    case 2:
	        setcolor(color/255.0,color/255.0,1,1);break;
	    case 3:
	    {
		double wtf=0.5+color/30;
	        setcolor(wtf,wtf,wtf,1);
//	        printf("%f\n", wtf);
	    }break;
	    case 4:
	        setcolor(colors[c].r/255.0,colors[c].g/255.0,colors[c].b/255.0,1);
//	    	printf("%d ", c);
		break;
	    }
}


int tscroll=0;
void draw_terminal(face *f, int selstartx, int selstarty, int selendx, int selendy, face* selface)
{
    char *trmsqrm=0;
    RoteTerm * rt=f->t;
    xy lok;
    lok.x=0;
    lok.y=0;
    int scroll=min(f->scroll,rt->logl);
    int j=0;
    int i;


    if(rt->log)
    {
	for (i=rt->logl-scroll;i<rt->logl;i++)
	{
	    if(!rt->log[i])break;
	    lok.y=(i-rt->logl+scroll)*26*f->scale;
	    if(rt->logstart) lok.y-=100;
	    if(rt->logstart) lok.x-=100;
	    for(j=0;j<rt->log[i][0].ch;j++)
	    {
		lok.x=j*13*f->scale;
#ifdef GL
		glBegin(GL_LINE_STRIP);
#endif
		do_color(rt->log[i][j+1].attr, f);
	        draw(lok,rt->log[i][j+1].ch,f->scale,f->scale);
#ifdef GL
		glEnd();
#endif
	    }
	}

    }

    int isundercursor;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=(scroll+i)*26*f->scale;
	if(f->scale>1)lok.y+=(f->scale-1)/2*26;
//	int gotlog=1;
	for (j=0; j<rt->cols; j++)
	{
#ifdef GL
	    glBegin(GL_LINE_STRIP);
#endif

	    lok.x=j*13*f->scale;
	    if(f->scale>1)lok.x+=(f->scale-1)/2*13;
	    if(!tscroll)//draw_edges_between_different_bgs
	    {
		//halflight=1;
		if((j>0))
		    if((ROTE_ATTR_BG(rt->cells[i][j-1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaaz",f->scale);
		if((j<rt->cols-1))
		    if((ROTE_ATTR_BG(rt->cells[i][j+1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"zazz",f->scale);
		if((i<rt->rows-1))
		    if((ROTE_ATTR_BG(rt->cells[i+1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"azzz",f->scale);
		if((i>0))
		    if((ROTE_ATTR_BG(rt->cells[i-1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaza",f->scale);
		//halflight=0;
	    }
//	    int bold=(!tscroll)&&((rt->cells[i][j].attr)&128);
	    do_color(rt->cells[i][j].attr,f);
	    	    
	    isundercursor=(!rt->cursorhidden)&&(!tscroll)&&((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor

#ifdef GL
	    if(isundercursor||(selface==f&&selstartx<=j&&selstarty<=i&&selendx>=j&&selendy>=i))
	    {
		if((f->oldcrow!=rt->crow)||(f->oldccol!=rt->ccol))
		    f->rotor=0;//if cursor moved, reset letter rotor
		//zspillit(lok,nums[0],2.4*f->scale);//draw cursor
		glEnd();
		if(isundercursor)
		{
		    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		    glPushMatrix();
		    glTranslatef(lok.x+13,lok.y+13,0);
		    glRotatef(f->rotor,0,0,1);
            	    glBegin(GL_LINE_LOOP);
            	    int w=13;
            	    glColor4f(1,0,0,1);
		    glVertex2f(-w*f->scale,-w*f->scale);
		    glVertex2f(+w*f->scale,-w*f->scale);
		    glVertex2f(+w*f->scale,+w*f->scale);
		    glVertex2f(-w*f->scale,+w*f->scale);
		    glEnd();
		    glPopMatrix();
		    glPushMatrix();

		    glTranslatef(lok.x+13,lok.y+13,0);
		    glRotatef(f->rotor/10, 0,0,1);
		    glColor4f(1,1,0,0.2);
		    int i;
		    int steps=10;
		    for (i=0; i<360; i+=steps)
		    {
			glRotatef(steps, 0,0,1);
			glPushMatrix();
			glTranslatef(0,100,0);
			glBegin(GL_QUADS);
			glVertex2f(-1,0);
			glVertex2f(1,0);
			glVertex2f(2,10);
			glVertex2f(-2,10);
			glEnd();
			glPopMatrix();
		    }
		    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
		    glPopMatrix();
		}
		glPushMatrix();
		    glTranslatef(lok.x+13,lok.y+13,0);
		    glPushMatrix();
		    glRotatef(f->rotor+=((SDL_GetTicks()-f->lastrotor)/10),0,1,0);
		    f->lastrotor=SDL_GetTicks();
		    glBegin(GL_LINE_STRIP);
			xy molok;molok.x=-13;molok.y=-13;
			draw(molok,rt->cells[i][j].ch,f->scale,f->scale);
		    glEnd();
		    glPopMatrix();
		glPopMatrix();
		glBegin(GL_LINE_STRIP);
	    }
	    else
#else

	    if(isundercursor)
	    {	// but still cursor square
		zspillit(lok,nums[0],1.2*f->scale);
	    }

#endif
	    draw(lok,rt->cells[i][j].ch,f->scale,f->scale);
#ifdef GL
	    glEnd();
#endif
	}

    }
    trmsqrm=0; // message under terminal window
    if (!rt->childpid)
    {
//	activetext=1;
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

//    glPopMatrix();
    f->oldcrow=rt->crow;//4 cursor rotation
    f->oldccol=rt->ccol;
    

#endif
/*
    if(1)//return boundariesssssssssssss
    {
	f->lim.x=lx;
	f->lim.y=ly;	
	f->lim.x2=lw;	
	f->lim.y2=lh;		
        glimits=0;
    }	
*/
}


