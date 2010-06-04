#include "linetextdata.c"


double r,g,b,a;

void setcolor(double rr,double gg,double bb,double aa)
{
    r=rr;g=gg;b=bb;a=aa;
//    printf("%f %f %f %f\n",r,g,b,a);
}



void azspillit(xy lok, const char *cx,
float a,float z)
// x scale, y scale
{
    char *x = strdup(cx);//this is just for compiler stupidity, were not actually modifying x
    int first=1; /*were drawin 1st dot*/
#ifdef SDLD
    have_first=0;
#endif
    int xdot,ydot;
    float fxdot;
    float fydot;
    int flip2=0;
    int flip=0;
    char* duped=0;
    while ((*x)&&(*(x+1)))
    {
        flip2=flip2||((*x==45)&&x++);//*(x++)
        if (!(*(x+1)))break; // -
        flip=flip||((*x==124)&&x++);
        if (!(*(x+1)))break; // |
        /*"-|", !"|-"*/
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
            if (*x==64) // @
                if ((*(x+1))&&(*(x+2))&&(*(x+3))&&(*(x+4))&&(*(x+5))&&(*(x+6))&&(*(x+7))&&(*(x+8))&&(*(x+9))&&(*(x+10))&&(*(x+11))&&(*(x+12)))
                {

                    x=strdup(x);
            	    duped=x;
                    x[5]=x[3];
                    x[6]=x[2];
                    x[7]=x[1];
                    x[8]=x[2];
                    x[9]=x[1];
                    x[10]=x[4];
                    x[11]=x[3];
                    x[12]=x[4];
                    x++;
                    x++;
                    x++;
                }
            xdot=*x;
            x++;
            ydot=*x;
            xdot-='a';
            ydot-='a';
            if (xdot<0) // capital
                xdot+=58;
            if (ydot<0)
                ydot+=58;

            ydot=ydot-13;
            xdot=xdot-13;
            fydot=ydot*(z+(2*z*(0-flip2))); //zoom and flip
            fxdot=xdot*0.5*(a+(2*a*(0-flip))); //always shrink width to half
            fxdot=fxdot+13;
            fydot=fydot+13;

            x++;
            if(first)
    	    {
    		first=0;
    		glColor4d(0,0,0,0);
                dooooot(lok.x+fxdot,lok.y+fydot);
                glColor4d(r,g,b,a);
            }

            if ((!*x)||(' '==(*x)))/* last dot*/
            {
                dooooot(lok.x+fxdot,lok.y+fydot);
    		glColor4d(0,0,0,0);
                dooooot(lok.x+fxdot,lok.y+fydot);
        	if(!*x)
        	{
            	    if(duped)free(duped);
            	    return;
        	}	

            }
            else // just ordinary dot
            {
                dooooot(lok.x+fxdot,lok.y+fydot);
            }
        }
    }
    if(duped)free(duped);
    free(x);
}

void zspillit(xy lok,const char *x,float z)
{
    azspillit(lok,x,z,z);
}
/*
static void spillit(xy lok,char *x)
{
    zspillit(lok,x,1);
}
*/


xy draw(xy lok,unsigned int i, double s, double z)
{
    xy nlok;
    nlok=lok;
    nlok.x+=13*s;

    if (do_l2&&l2)
        if (i<l2numitems)
            if (l2[i])
                if (strlen(l2[i]))
                {
                    azspillit(lok,l2[i],0.7*s,0.7*z);
                    return nlok;
                }

/*from now on its the builtin glyphs*/


    if (!((i<65)||((i>90)&&(i<97))||(i>122)))
    {
        if ((i>='A')&&(i<='Z'))
            azspillit(lok,chrz[i-'A'],0.8*s,0.8*z);
        else
        {
            azspillit(lok,chrz[i-'a'],0.5*s,0.5*z);
        }
        return nlok;
    }
    else if ((i>47)&&(i<58))
    {
        nlok.x=lok.x+26;
        azspillit(lok, nums[i-48],0.8*s,0.8*z);
        return nlok;
    }
    else if (i==32)
        return nlok;
    else if (i==37)
        azspillit(lok, "azza  @ddgg        -|  @ddgg        ",s,z);
    else if (i==283)
    {
        draw(lok, 'e',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==353)
    {
        draw(lok, 's',s,z);
        azspillit(lok, "haofxa",s,z);
    }

    else if (i==269)
    {
        draw(lok, 'c',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==345)
    {
        draw(lok, 'r',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==382)
    {
        draw(lok, 'z',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==253)
    {
        draw(lok, 'y',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==225)
    {
        draw(lok, 'a',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==237)
    {
        draw(lok, 'i',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==233)
    {
        draw(lok, 'e',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==201)
    {
        draw(lok, 'E',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==282)
    {
        draw(lok, 'E',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==344)
    {
        draw(lok, 'R',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==356)
    {
        draw(lok, 'T',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==221)
    {
        draw(lok, 'Y',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==218)
    {
        draw(lok, 'U',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==205)
    {
        draw(lok, 'I',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==211)
    {
        draw(lok, 'O',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==243)
    {
        draw(lok, 'o',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==193)
    {
        draw(lok, 'A',s,z);
        azspillit(lok, "ofxa",s,z);
    }
    else if (i==352)
    {
        draw(lok, 'S',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==270)
    {
        draw(lok, 'D',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==381)
    {
        draw(lok, 'Z',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==268)
    {
        draw(lok, 'C',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==327)
    {
        draw(lok, 'N',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==328)
    {
        draw(lok, 'n',s,z);
        azspillit(lok, "haofxa",s,z);
    }
    else if (i==38)
        azspillit(lok, "zzaggangarozzr",0.9*s,0.9*z);
    else if (i==34)
        azspillit(lok, "iaig  qaqg",s,z);
    else if (i==35)
        azspillit(lok, "hahz  uauz  ahzh  auzu",s,z);
    else if (i==33)
        azspillit(lok, "oaoq  ouoz",s,z);
    else if (i==64)
        azspillit(lok, "ozaooaxoovkoohov",s,z);
    else if (i==123)
        azspillit(lok, "gadddlaodrdwgz",s,z);
    else if (i==125)
        azspillit(lok, "|gadddlaodrdwgz",s,z);
    else if (i==8217)
        azspillit(lok, "kfma",s,z);
    else if (i==91)
        azspillit(lok, "oddddwow",s,z);
    else if (i==93)
        azspillit(lok, "|oddddwow",s,z);
    else if (i==94)
        azspillit(lok, "ahoazh",s,z);
    else if (i==39)
        azspillit(lok, "oaoj",s,z);
    else if (i==96)
        azspillit(lok, "mcqg",s,z);
    else if (i==92)
        azspillit(lok, sgns[19],s,z);
    else if ((i==9492))
        azspillit(lok, "oaoozo",s,z);
    else if ((i==9488))
        azspillit(lok, "aooooz",s,z);
    else if ((i==9496))
        azspillit(lok, "aooooa",s,z);
    else if ((i==9484))
        azspillit(lok, "ozoozo",s,z);
    else if (i==9672)
        azspillit(lok, sgns[24],s,z);
    else if (i==36)
        azspillit(lok, sgns[23],s,z);
    else if (i==9662)
        azspillit(lok, sgns[22],s,z);
    else if (i==9652)
        azspillit(lok, sgns[25],s,z);
    else if (i==9500)
        azspillit(lok, sgns[21],s,z);
    else if (i==9472)
        azspillit(lok, "aozo",s,z);
    else if (i==9474)
        azspillit(lok, "oaoz",s,z);
    else if (i==9618)
        azspillit(lok, "aszsoaas  -aszsoaas",s,z);
    else if (i==32)
        azspillit(lok, sgns[18],s,z);
    else if (i==63)
        azspillit(lok, sgns[17],s,z);
    else if (i==58)
        azspillit(lok, sgns[16],s,z);
    else if (i==126)
        azspillit(lok, sgns[15],s,z);
    else if (i==46)
        azspillit(lok, sgns[14],s,z);
    else if (i==95)
        azspillit(lok, sgns[13],s,z);
    else if (i==45)
        azspillit(lok, sgns[12],s,z);
    else if (i==43)
        azspillit(lok, sgns[11],s,z);
    else if (i==44)
        azspillit(lok, sgns[10],s,z);
    else if (i==47)
        azspillit(lok, sgns[9],s,z);
    else if (i==41)
        azspillit(lok, sgns[8],s,z);
    else if (i==40)
        azspillit(lok, sgns[7],s,z);
    else if (i==59)
        azspillit(lok, sgns[6],s,z);
    else if (i==42)
        azspillit(lok, sgns[5],s,z);
    else if (i==61)
        azspillit(lok, sgns[4],s,z);
    else if (i==62)
        azspillit(lok, sgns[3],s,z);
    else if (i==60)
        azspillit(lok, sgns[2],s,z);
    else if (i==10)
        azspillit(lok, sgns[1],s,z);
    else if (!i)
        azspillit(lok, sgns[0],s,z);
    else if (i==124)
        azspillit(lok, sgns[20],s,z);
    return nlok;
}


void draw_line(int x,int y,const char *a)
{
    xy lok;
    lok.x=x;
    lok.y=y;
    do
    {

        lok=draw(lok,*a,1,1);
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

        lok=draw(lok,*a,z,z);
        lok.x+=4;
        if (!*a)
            break;
        a++;
    }
    while (1);

}

