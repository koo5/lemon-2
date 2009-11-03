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

inline void glColor4f(float r,float g,float b,float a)
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
