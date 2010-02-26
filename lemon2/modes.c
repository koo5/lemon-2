#ifndef openbsd 
long long strtonum(const char *nptr, long long minval, long long maxval, const char **errstr)
{
    return atoi(nptr);
}
#endif

#ifndef HAVE_FGETLN
#include "../more-mess/fgetln.c"
#endif

//#include "../XY.h"        

// modes are compared by w so 2 cant have same ws
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
    if((ypos=(void*)strcasestr(r,"x")))    
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


xy parsemodes(int w,int h/*current w and h to know where to look while shrinking or growing (or both)*/,char *file,int start/* program start*/,int shrink,int grow)
{
	     xy r,fail;
	     xy oldr;
	     oldr.x=-1;	     oldr.y=-1;
	     fail=oldr;
	     FILE * fp = fopen(file,"r");
	     if (fp == NULL) 
	     {
	        printf("fp == NULL");
		return fail;
	     }
             char *buf, *lbuf;
             size_t len;
	     int growit = 0;
             lbuf = NULL;
             while ((buf = fgetln(fp, &len))) {
                     if ((buf[len - 1] == '\n'))
                             buf[len - 1] = '\0';
                     else {
                             /* EOF without EOL, copy and add the NUL */
                             if ((lbuf = malloc(len + 1)))
                             {
                                memcpy(lbuf, buf, len);
                                lbuf[len] = '\0';
                                buf = lbuf;
			     }
			     else
                        	buf[len - 1] = '\0';			        
                     }
		     r = parsemodeline(buf);
		     if(r.x!=-1)
		     {
		        if(growit||start)
			{
			     fclose(fp);
			    return r;	
			}
		        if(((r.x==w)&&(r.y==h))||((oldr.x!=-1)&&(oldr.x<w)&&(r.x>w)))
			{
			    if(shrink)return oldr;
			    if (grow)    
				growit=1;
			}
			oldr=r;
		     }
		     free(buf);
             }
             free(lbuf);
	     fclose(fp);
	     return fail;
}

void savemode(int w,int h,char *mdfl)
{
    FILE * fp = fopen(mdfl,"w");
    if (fp == NULL) 
	return;
    fprintf(fp,"%i %i",w,h);
    fclose(fp);
}

