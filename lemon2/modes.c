#ifndef openbsd 
long long strtonum(const char *nptr, long long minval, long long maxval, const char **errstr)
{
    return atoi(nptr);
}
#endif

#ifndef HAVE_FGETLN
#include "../more-mess/fgetln.c"
#endif

struct XY parsemodeline(char *r)
{
    xy m;
    m.x=-1;
    m.y=0;
    int x;
    int y;
    char * ypos;
    char *  pos;
//    printf("%s\n",r);
#ifndef strcasestr
#define strcasestr strstr
#endif
    if((ypos=(char*)strcasestr(r,"x")))
    {
	*ypos = 0;
        ypos++;
    }
    if((pos=strstr(r," ")))
    {
	*pos=0;
	if(pos>ypos)ypos=pos+1;
    }
    if((x=strtonum(r,1,65535,0)))
    {
        if((y=strtonum(ypos,1,55555,0))){
	    m.x=x;
	    m.y=y;
//	    printf("%iX%i\n",x,y);
	    return m;
	}
    }
    return m;
}



void savemode(int w,int h,char*mdfl)
{
    FILE * fp = fopen(mdfl,"w");
    if (fp == NULL) 
	return;
    fprintf(fp,"%i %i",w,h);
    fclose(fp);
}

