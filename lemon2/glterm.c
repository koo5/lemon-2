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
    char *trmsqrm=0;
    static int oldcrow, oldccol;
    RoteTerm * rt=f->t;
#ifdef GL
//    lx=0;ly=0;lw=0;lh=0; 
//    glimits=1;
    
    glPushMatrix();
    glBegin(GL_LINE_STRIP);
#endif
    xy lok;
    lok.x=0;    lok.y=0;



    int i,j;
    int isundercursor;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=i*26;
	int gotlog=1;
	for (j=0; j<rt->cols; j++)
	{
	    lok.x=j*13;
	    if(!tscroll)//draw_edges_between_different_bgs
	    {
		//halflight=1;
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
		//halflight=0;
	    }
	    //bold=(!tscroll)&&((rt->cells[i][j].attr)&128);
	    	    
	    isundercursor=(!tscroll)&&((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor
/*
#ifdef GL
	    if(isundercursor) //do nice rotation animation hehe
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
*/
	    if(isundercursor)
	    {	// but still cursor square
		zspillit(lok,nums[0],1.2);
	    }

//#endif
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

    glEnd();
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


