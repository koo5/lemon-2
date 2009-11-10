int halflight;


void draw_text(char *a)
{
    if(!a)return;
  xy lok;
  lok.x=0;
  lok.y=0;
  glBegin(GL_LINE_STRIP);
  do 
  {
	lok=draw(lok,*a,1);
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


void lokdraw_line(xy lok,const char *a)
{// of text
glBegin(GL_LINE_STRIP);
do 
{
	lok=draw(lok,*a,1);
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
    char *trmsqrm=0;
    static int oldcrow, oldccol;
    RoteTerm * rt=f->t;
    xy lok;
    lok.x=0;

    int scroll=min(tscroll,rt->logl);
    int j=0;
    int i;
    if(rt->log)
    {
	for (i=0; i<scroll; i++)
	{
	    if(!rt->log[i])break;
	    lok.y=-(1+i)*26*f->scale;
	    while(rt->log[i][j].ch)
	    {
		lok.x=j*13*f->scale;
	        draw(lok,rt->log[i][j].ch,f->scale);
	        j++;
	    }
	}
    }

    int isundercursor;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=(scroll+i)*26*f->scale;
	int gotlog=1;
	for (j=0; j<rt->cols; j++)
	{
#ifdef GL
    glBegin(GL_LINE_STRIP);
#endif

	    lok.x=j*13*f->scale;
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
	    int bold=(!tscroll)&&((rt->cells[i][j].attr)&128);
	    int color=((rt->cells[i][j].attr));
	    
	    if(!theme)
	        glColor4f(1,color/255.0,color/255.0,1);
	    else if (theme==1)
	        glColor4f(color/255.0,1,color/255.0,1);
	    else if (theme==2)
	        glColor4f(color/255.0,color/255.0,1,1);
	    else if (theme==3)
	        glColor4f(1,1,color/255.0,1);
//	    printf("%d", color);
		

	    	    
	    isundercursor=(!rt->cursorhidden)&&(!tscroll)&&((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor

#ifdef GL
	    if(isundercursor) //do nice rotation animation hehe
	    {
		static int rotor;
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
			draw(molok,rt->cells[i][j].ch,f->scale);
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
	    draw(lok,rt->cells[i][j].ch,f->scale);
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

    glPopMatrix();
    oldcrow=rt->crow;//4 cursor rotation
    oldccol=rt->ccol;


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


