#include <string>
#include <vector>
#include <fstream>
#include <iostream>
vector<string> font;
const char *nullglyph="dzdptuzpznxnxpzp";
double r,g,b,a;
void setcolor(double rr,double gg,double bb,double aa)
{
    r=rr;g=gg;b=bb;a=aa;
}

void tokenize(const string& str, vector<string>& tokens, const string& delimiters = "+")
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos)
    {
	// Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
string prepare(string y)
{
    if(y.find('+')==string::npos)
	return y;
    else
    {
	string ret;
	vector<string>tokens;
	tokenize(y,tokens);
	for(int i=0;i<tokens.size();i++)
	{
	    char* endptr=0;
	    int b=strtol(tokens.at(i).c_str(),&endptr,10);
	    if(!endptr)
    	    {
    		if(font.size()<b)
    		    ret.append(font.at(b));
	    }
	    else
		ret.append(tokens.at(i));
	}
	return prepare(ret);
    }
}


 
void _spillit(xy lok, const char*x, float offset)
{
    int first=1;
    #ifdef SDLD
	have_first=0;
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
                dooooot(offset+lok.x+xdot,offset+lok.y+ydot);
                glColor4d(r,g,b,a);
            }

            if ((!*x)||(' '==(*x)))/* last dot*/
            {
                dooooot(offset+lok.x+xdot,offset+lok.y+ydot);
    		glColor4d(0,0,0,0);
                dooooot(offset+lok.x+xdot,offset+lok.y+ydot);
        	if(!*x)
        	{
            	    return;
        	}	
            }
            else // just ordinary dot
            {
                dooooot(offset+lok.x+xdot,offset+lok.y+ydot);
            }
        }
    }
}
void spillit(xy lok, const char *x)
{
    _spillit(lok,x,0);
    _spillit(lok,x,0);
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
	font.push_back(x);
    for(int i=0;i<font.size();i++)
	font.at(i)=prepare(font.at(i));
	
}
