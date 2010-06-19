
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


void do_color(int attr,face *f)
{
    int c=ROTE_ATTR_XFG(attr);//0-15
    setcolor(colors[c].r/255.0,colors[c].g/255.0,colors[c].b/255.0,1);
}

void draw_terminal(face *f)
{
    RoteTerm * rt=f->t;
    xy lok;
    lok.x=0;
    lok.y=0;
    int j=0;
    int i;
    #ifdef GL
    glBegin(GL_LINE_STRIP);
    #endif
    int scroll=min(f->scroll,rt->logl);
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
		do_color(rt->log[i][j+1].attr, f);
	        draw(lok,rt->log[i][j+1].ch,f->scale,f->scale);
	    }
	}

    }
    int isundercursor;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=(scroll+i)*26*f->scale;
	if(f->scale>1)lok.y+=(f->scale-1)/2*26;
	for (j=0; j<rt->cols; j++)
	{
	    lok.x=j*13*f->scale;
	    if(f->scale>1)lok.x+=(f->scale-1)/2*13;
	    {
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
	    }
	    do_color(rt->cells[i][j].attr,f);
	    isundercursor=(!rt->cursorhidden)&&((rt->ccol==j)&&(rt->crow==i));
	    #ifdef GL
	    if(isundercursor||(selface==f&&selstartx<=j&&selstarty<=i&&selendx>=j&&selendy>=i))
	    {
		if((f->oldcrow!=rt->crow)||(f->oldccol!=rt->ccol))
		    f->rotor=0;//if cursor moved, reset letter rotor
		//zspillit(lok,nums[0],2.4*f->scale);//draw cursor
		glEnd();
		if(isundercursor)
		{
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
		    int i;
		    int steps=10;
		    for (i=0; i<360; i+=steps)
		    {
			glRotatef(steps, 0,0,1);
			glPushMatrix();
			glTranslatef(0,100,0);
			glBegin(GL_QUADS);
			glColor4f(1,1,0,0.2);
			glVertex2f(-1,0);
			glVertex2f(1,0);
			glVertex2f(2,10);
			glVertex2f(-2,10);
			glEnd();
			glPopMatrix();
		    }
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
	    draw(lok,rt->cells[i][j].ch,f->scale,f->scale);
	}

    }
    #ifdef GL
    f->oldcrow=rt->crow;//4 cursor rotation
    f->oldccol=rt->ccol;
    glEnd();
    #endif
}
