#include <string>
#include <vector>
#include <fstream>
#include <iostream>
vector<string> font;
char *nullglyph="dzdptuzpznxnxpzp";
double r,g,b,a;
void setcolor(double rr,double gg,double bb,double aa)
{
    r=rr;g=gg;b=bb;a=aa;
}

char *prepare(const char*y)
{
    char *add_to=(char*)malloc(1);
    add_to[0]=0;
    char *x=strdup(y);
    char *re=x;
    while (*x)
    {
    	if(*x<='9'&&*x>='0')
    	{
    	    add_to=(char*)realloc(add_to,strlen(add_to)+2);
    	    add_to[strlen(add_to)]=*x;
    	    add_to[strlen(add_to)+1]=0;
    	}
    	if(*x=='+')
    	{
	    int addto=atoi(add_to);
	    if(font.size()>addto)
		if(!font.at(addto).empty())
		{
		    int originalxlen=strlen(x);
		    re=x=(char*)realloc(re, strlen(re)+font.at(addto).length());
		    memmove(font.at(addto).length()+x-strlen(add_to)-1, x+1,originalxlen);
		    memmove((void*)(x-1-strlen(add_to)), (void*)font.at(addto).c_str(),font.at(addto).length());
		}
	}
    	    x++;
    }
    return re;
}



void _spillit(xy lok, const char*x)
{
    int first=1;
    #ifdef SDLD
	have_first=0
    #endif
    int xdot,ydot;
    float fxdot;
    float fydot;
    while ((*x)&&(*(x+1)))
    {
        if (((*x)==' ')&&(*(x+1)==' ')) // 2 spaces
        {
            x++;
            x++;
	    #ifdef SDLD
        	have_first=0;
	    #endif
            first=1;
        }
        else
        {
            xdot=*x;
            x++;
            ydot=*x;
            xdot-='a';
            ydot-='a';
            if (xdot<0) // capital
                xdot+=58;
            if (ydot<0)
                ydot+=58;

            x++;
            if(first)
    	    {
    		first=0;
    		glColor4d(0,0,0,0);
                dooooot(lok.x+xdot,lok.y+ydot);
                glColor4d(r,g,b,a);
            }

            if ((!*x)||(' '==(*x)))/* last dot*/
            {
                dooooot(lok.x+xdot,lok.y+ydot);
    		glColor4d(0,0,0,0);
                dooooot(lok.x+xdot,lok.y+ydot);
        	if(!*x)
        	{
            	    return;
        	}	
            }
            else // just ordinary dot
            {
                dooooot(lok.x+xdot,lok.y+ydot);
            }
        }
    }
}

void spillit(xy lok, const char *x)
{
    _spillit(lok,x);
    _spillit(lok,x);
}


xy drawchar(xy lok, unsigned int i)
{
    xy nlok;
    nlok=lok;
    nlok.x+=13;

    if (i<font.size()&&font[i].length())
        spillit(lok,font[i].c_str());
    return nlok;
}

/*
void draw_line(int x,int y,const char *a)
{
    xy lok;
    lok.x=x;
    lok.y=y;
    do
    {

        lok=drawchar(lok,*a,1,1);
        lok.x+=4;
        if (!*a)
            break;
        a++;
    }
    while (1);

}

void draw_line_z(const char *a, double z)
{
    xy lok;
    lok.x=0;
    lok.y=0;
    do
    {

        lok=drawchar(lok,*a,z,z);
        lok.x+=4;
        if (!*a)
            break;
        a++;
    }
    while (1);

}





void lokdraw_line(xy lok,const char *a)
{
    glBegin(GL_LINE_STRIP);
    do 
    {
	lok=drawchar(lok,*a,1,1);
	lok.x+=4;
	if (!*a)
	    break;
	a++;
    }
    while(1);
    glEnd();
}

*/


void draw_text(char *a)
{
  if(!a)return;
  xy lok;
  lok.x=0;
  lok.y=0;
  setcolor(0,1,0.2,1);
  glBegin(GL_LINE_STRIP);
  do 
  {
	lok=drawchar(lok,*a);
	if (*a==10)
	{
	    lok.x=0;
	    lok.y=lok.y+30;
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
void loadfont(char * fln)
{
    if(!fln)return;
    cout << fln;
    ifstream ff(fln,std::ios::in);
    if (!ff)
    {
        printf("cant load font from '%s'\n",fln);
        return;
    }
    font.clear();
    font.push_back(nullglyph);
    string x;
    while(getline(ff,x))
    {
	char *p=(prepare(x.c_str()));
	cout << "|" << p;;
	font.push_back(p);
    }
}
