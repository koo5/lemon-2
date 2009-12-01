SDL_Surface *gltextsdlsurface;
Uint32 barvicka=255;
int have_first=0;
int gltx,glty;
#define GL_LINE_STRIP 0

void glEnd()
{
}

void glBegin(int aha)
{
    have_first=0;
}

inline void glColor4d(double r,double g,double b,double a)
{
    if (a==0)
	barvicka=0;
    else
	barvicka=SDL_MapRGBA( gltextsdlsurface->format ,r*255.0f,g*255.0f,b*255.0f,a*255.0f);
}

void dooooot(float x,float y)
{
    static Uint32 firstc;
    static Sint16 firstx, firsty;
        x=x+gltx;
        y=y+glty;
    if (y<1)y=1;    if (x<1)x=1;
    if(y>gltextsdlsurface->h-2)y=gltextsdlsurface->h-2;
    if(x>gltextsdlsurface->w-2)x=gltextsdlsurface->w-2;
    if(have_first)
    {
	if(barvicka&&firstc)
	    Draw_Line(gltextsdlsurface,firstx,firsty,x,y,barvicka);
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

void DrawLine(SDL_Surface *s, int x, int y, int x2, int y2, int c)
{

    if (y<1)y=1;    if (x<1)x=1;
    if(y>gltextsdlsurface->h-2)y=gltextsdlsurface->h-2;
    if(x>gltextsdlsurface->w-2)x=gltextsdlsurface->w-2;
    if (y2<1)y2=1;    if (x2<1)x2=1;
    if(y2>gltextsdlsurface->h-2)y2=gltextsdlsurface->h-2;
    if(x2>gltextsdlsurface->w-2)x2=gltextsdlsurface->w-2;
    Draw_Line(s,x,y,x2,y2,c);
}
