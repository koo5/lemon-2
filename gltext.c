float xoom, yoom;
float zoomx, zoomy;
//#include "XY.h"
#include "linetextdata"






void azspillit(xy lok,char *x,float a,float z)
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
    while ((*x)&&(*(x+1)))
    {
        flip2=flip2||((*x==45)&&x++);//bug
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
                return;
            }
            else // just ordinary dot
            {
                dooooot(lok.x+fxdot,lok.y+fydot);
            }
        }
    }


}

void zspillit(xy lok,char *x,float z)
{
    azspillit(lok,x,z,z);
}
static void spillit(xy lok,char *x)
{
    zspillit(lok,x,1);
}



static xy draw(xy lok,unsigned int i)
{
    xy nlok;
    nlok=lok;
    nlok.x+=30;

    if (l2)
        if (i<l2numitems)
            if (l2[i])
                if (strlen(l2[i]))
                {
                    zspillit(lok,l2[i],0.7);
                    return nlok;
                }

    if (!((i<65)||((i>90)&&(i<97))||(i>122)))
    {
        if ((i>='A')&&(i<='Z'))
            zspillit(lok,chrz[i-'A'],0.8);
        else
        {
            zspillit(lok,chrz[i-'a'],0.5);
        }
        return nlok;
    }
    else if ((i>47)&&(i<58))
    {
        nlok.x=lok.x+26;
        zspillit(lok, nums[i-48],0.8);
        return nlok;
    }
    else if (i==32)
        return nlok;
    else if (i==37)
        spillit(lok, "azza  @ddgg        -|  @ddgg        ");
    else if (i==283)
    {
        draw(lok, 'e');
        spillit(lok, "haofxa");
    }
    else if (i==353)
    {
        draw(lok, 's');
        spillit(lok, "haofxa");
    }
    else if (i==269)
    {
        draw(lok, 'c');
        spillit(lok, "haofxa");
    }
    else if (i==345)
    {
        draw(lok, 'r');
        spillit(lok, "haofxa");
    }
    else if (i==382)
    {
        draw(lok, 'z');
        spillit(lok, "haofxa");
    }
    else if (i==253)
    {
        draw(lok, 'y');
        spillit(lok, "ofxa");
    }
    else if (i==225)
    {
        draw(lok, 'a');
        spillit(lok, "ofxa");
    }
    else if (i==237)
    {
        draw(lok, 'i');
        spillit(lok, "ofxa");
    }
    else if (i==233)
    {
        draw(lok, 'e');
        spillit(lok, "ofxa");
    }
    else if (i==201)
    {
        draw(lok, 'E');
        spillit(lok, "ofxa");
    }
    else if (i==282)
    {
        draw(lok, 'E');
        spillit(lok, "haofxa");
    }
    else if (i==344)
    {
        draw(lok, 'R');
        spillit(lok, "haofxa");
    }
    else if (i==356)
    {
        draw(lok, 'T');
        spillit(lok, "haofxa");
    }
    else if (i==221)
    {
        draw(lok, 'Y');
        spillit(lok, "ofxa");
    }
    else if (i==218)
    {
        draw(lok, 'U');
        spillit(lok, "ofxa");
    }
    else if (i==205)
    {
        draw(lok, 'I');
        spillit(lok, "ofxa");
    }
    if (i==211)
    {//29;5~29;5~ /              mmmMMmmmmmmmmmmmmm//////jjJKL mjjjjjj       jj   /
        draw(lok, 'O');
        spillit(lok, "ofxa");
    }
    else if (i==243)
    {
        draw(lok, 'o');
        spillit(lok, "ofxa");
    }
    else if (i==193)
    {
        draw(lok, 'A');
        spillit(lok, "ofxa");
    }
    else if (i==352)
    {
        draw(lok, 'S');
        spillit(lok, "haofxa");
    }
    else if (i==270)
    {
        draw(lok, 'D');
        spillit(lok, "haofxa");
    }
    else if (i==381)
    {
        draw(lok, 'Z');
        spillit(lok, "haofxa");
    }
    else if (i==268)
    {
        draw(lok, 'C');
        spillit(lok, "haofxa");
    }
    else if (i==327)
    {
        draw(lok, 'N');
        spillit(lok, "haofxa");
    }
    else if (i==328)
    {
        draw(lok, 'n');
        spillit(lok, "haofxa");
    }

    else if (i==38)
        zspillit(lok, "zzaggangarozzr",0.9);
    else if (i==34)
        spillit(lok, "iaig  qaqg");
    else if (i==35)
        spillit(lok, "hahz  uauz  ahzh  auzu");
    else if (i==33)
        spillit(lok, "oaoq  ouoz");
    else if (i==64)
        spillit(lok, "ozaooaxoovkoohov");
    else if (i==123)
        spillit(lok, "gadddlaodrdwgz");
    else if (i==125)
        spillit(lok, "|gadddlaodrdwgz");
    else if (i==8217)
        spillit(lok, "kfma");
    else if (i==91)
        spillit(lok, "oddddwow");
    else if (i==93)
        spillit(lok, "|oddddwow");
    else if (i==94)
        spillit(lok, "ahoazh");
    else if (i==39)
        spillit(lok, "oaoj");
    else if (i==96)
        spillit(lok, "mcqg");
    else if (i==92)
        spillit(lok, sgns[19]);
    else if ((i==9492))
        spillit(lok, "oaoozo");
    else if ((i==9488))
        spillit(lok, "aooooz");
    else if ((i==9496))
        spillit(lok, "aooooa");
    else if ((i==9484))
        spillit(lok, "ozoozo");
    else if (i==9672)
        spillit(lok, sgns[24]);
    else if (i==36)
        spillit(lok, sgns[23]);
    else if (i==9662)
        spillit(lok, sgns[22]);
    else if (i==9652)
        spillit(lok, sgns[25]);
    else if (i==9500)
        spillit(lok, sgns[21]);
    else if (i==9472)
        zspillit(lok, "aozo",1);
    else if (i==9474)
        zspillit(lok, "oaoz",1);
    else if (i==9618)
        zspillit(lok, "aszsoaas  -aszsoaas",1);
    else if (i==32)
        spillit(lok, sgns[18]);
    else if (i==63)
        spillit(lok, sgns[17]);
    else if (i==58)
        spillit(lok, sgns[16]);
    else if (i==126)
        spillit(lok, sgns[15]);
    else if (i==46)
        spillit(lok, sgns[14]);
    else if (i==95)
        spillit(lok, sgns[13]);
    else if (i==45)
        spillit(lok, sgns[12]);
    else if (i==43)
        spillit(lok, sgns[11]);
    else if (i==44)
        spillit(lok, sgns[10]);
    else if (i==47)
        spillit(lok, sgns[9]);
    else if (i==41)
        spillit(lok, sgns[8]);
    else if (i==40)
        spillit(lok, sgns[7]);
    else if (i==59)
        spillit(lok, sgns[6]);
    else if (i==42)
        spillit(lok, sgns[5]);
    else if (i==61)
        spillit(lok, sgns[4]);
    else if (i==62)
        spillit(lok, sgns[3]);
    else if (i==60)
        spillit(lok, sgns[2]);
    else if (i==10)
        spillit(lok, sgns[1]);
    else if (!i)
        spillit(lok, sgns[0]);
    else if (i==124)
        spillit(lok, sgns[20]);

}


void draw_line(int x,int y,const char *a)
{
    xy lok;
    lok.x=x;
    lok.y=y;
    do
    {

        lok=draw(lok,*a);
        lok.x+=4;
        if (!*a)
            break;
        a++;
    }
    while (1);

}

