/*float xoom, yoom;
float zoomx, zoomy;*/

//#include "XY.h"
#include "linetextdata"






void azspillit(xy lok,char *x,
float a,float z)
// x scale, y scale
{

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
            if (!*x)/*last dot*/
            {
                dooooot(lok.x+fxdot,lok.y+fydot);
                if(duped)free(duped);
                return;
            }
            else // just ordinary dot
            {
                dooooot(lok.x+fxdot,lok.y+fydot);
            }
        }
    }
    if(duped)free(duped);

}

void zspillit(xy lok,char *x,float z)
{
    azspillit(lok,x,z,z);
}
static void spillit(xy lok,char *x)
{
    zspillit(lok,x,1);
}



static xy draw(xy lok,unsigned int i, double s)
{
    xy nlok;
    nlok=lok;
    nlok.x+=30;

    if (l2)
        if (i<l2numitems)
            if (l2[i])
                if (strlen(l2[i]))
                {
                    zspillit(lok,l2[i],0.7*s);
                    return nlok;
                }

/*from now on its the builtin glyphs*/


    if (!((i<65)||((i>90)&&(i<97))||(i>122)))
    {
        if ((i>='A')&&(i<='Z'))
            zspillit(lok,chrz[i-'A'],0.8*s);
        else
        {
            zspillit(lok,chrz[i-'a'],0.5*s);
        }
        return nlok;
    }
    else if ((i>47)&&(i<58))
    {
        nlok.x=lok.x+26;
        zspillit(lok, nums[i-48],0.8*s);
        return nlok;
    }
    else if (i==32)
        return nlok;
    else if (i==37)
        zspillit(lok, "azza  @ddgg        -|  @ddgg        ",s);
    else if (i==283)
    {
        draw(lok, 'e',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==353)
    {
        draw(lok, 's',s);
        zspillit(lok, "haofxa",s);
    }

    else if (i==269)
    {
        draw(lok, 'c',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==345)
    {
        draw(lok, 'r',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==382)
    {
        draw(lok, 'z',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==253)
    {
        draw(lok, 'y',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==225)
    {
        draw(lok, 'a',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==237)
    {
        draw(lok, 'i',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==233)
    {
        draw(lok, 'e',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==201)
    {
        draw(lok, 'E',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==282)
    {
        draw(lok, 'E',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==344)
    {
        draw(lok, 'R',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==356)
    {
        draw(lok, 'T',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==221)
    {
        draw(lok, 'Y',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==218)
    {
        draw(lok, 'U',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==205)
    {
        draw(lok, 'I',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==211)
    {
        draw(lok, 'O',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==243)
    {
        draw(lok, 'o',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==193)
    {
        draw(lok, 'A',s);
        zspillit(lok, "ofxa",s);
    }
    else if (i==352)
    {
        draw(lok, 'S',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==270)
    {
        draw(lok, 'D',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==381)
    {
        draw(lok, 'Z',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==268)
    {
        draw(lok, 'C',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==327)
    {
        draw(lok, 'N',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==328)
    {
        draw(lok, 'n',s);
        zspillit(lok, "haofxa",s);
    }
    else if (i==38)
        zspillit(lok, "zzaggangarozzr",0.9*s);
    else if (i==34)
        zspillit(lok, "iaig  qaqg",s);
    else if (i==35)
        zspillit(lok, "hahz  uauz  ahzh  auzu",s);
    else if (i==33)
        zspillit(lok, "oaoq  ouoz",s);
    else if (i==64)
        zspillit(lok, "ozaooaxoovkoohov",s);
    else if (i==123)
        zspillit(lok, "gadddlaodrdwgz",s);
    else if (i==125)
        zspillit(lok, "|gadddlaodrdwgz",s);
    else if (i==8217)
        zspillit(lok, "kfma",s);
    else if (i==91)
        zspillit(lok, "oddddwow",s);
    else if (i==93)
        zspillit(lok, "|oddddwow",s);
    else if (i==94)
        zspillit(lok, "ahoazh",s);
    else if (i==39)
        zspillit(lok, "oaoj",s);
    else if (i==96)
        zspillit(lok, "mcqg",s);
    else if (i==92)
        zspillit(lok, sgns[19],s);
    else if ((i==9492))
        zspillit(lok, "oaoozo",s);
    else if ((i==9488))
        zspillit(lok, "aooooz",s);
    else if ((i==9496))
        zspillit(lok, "aooooa",s);
    else if ((i==9484))
        zspillit(lok, "ozoozo",s);
    else if (i==9672)
        zspillit(lok, sgns[24],s);
    else if (i==36)
        zspillit(lok, sgns[23],s);
    else if (i==9662)
        zspillit(lok, sgns[22],s);
    else if (i==9652)
        zspillit(lok, sgns[25],s);
    else if (i==9500)
        zspillit(lok, sgns[21],s);
    else if (i==9472)
        zspillit(lok, "aozo",s);
    else if (i==9474)
        zspillit(lok, "oaoz",s);
    else if (i==9618)
        zspillit(lok, "aszsoaas  -aszsoaas",s);
    else if (i==32)
        zspillit(lok, sgns[18],s);
    else if (i==63)
        zspillit(lok, sgns[17],s);
    else if (i==58)
        zspillit(lok, sgns[16],s);
    else if (i==126)
        zspillit(lok, sgns[15],s);
    else if (i==46)
        zspillit(lok, sgns[14],s);
    else if (i==95)
        zspillit(lok, sgns[13],s);
    else if (i==45)
        zspillit(lok, sgns[12],s);
    else if (i==43)
        zspillit(lok, sgns[11],s);
    else if (i==44)
        zspillit(lok, sgns[10],s);
    else if (i==47)
        zspillit(lok, sgns[9],s);
    else if (i==41)
        zspillit(lok, sgns[8],s);
    else if (i==40)
        zspillit(lok, sgns[7],s);
    else if (i==59)
        zspillit(lok, sgns[6],s);
    else if (i==42)
        zspillit(lok, sgns[5],s);
    else if (i==61)
        zspillit(lok, sgns[4],s);
    else if (i==62)
        zspillit(lok, sgns[3],s);
    else if (i==60)
        zspillit(lok, sgns[2],s);
    else if (i==10)
        zspillit(lok, sgns[1],s);
    else if (!i)
        zspillit(lok, sgns[0],s);
    else if (i==124)
        zspillit(lok, sgns[20],s);

}


void draw_line(int x,int y,const char *a)
{
    xy lok;
    lok.x=x;
    lok.y=y;
    do
    {

        lok=draw(lok,*a,1);
        lok.x+=4;
        if (!*a)
            break;
        a++;
    }
    while (1);

}

