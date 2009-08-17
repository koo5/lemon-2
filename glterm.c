int curposx,curposy;
void draw_terminal(RoteTerm * rt, int showhex)
{
    static int oldcrow, oldccol;
    xy lok;
    lok.x=0;    lok.y=0;
    int i,j;
    for (i=0; i<rt->rows; i++)
    {
	lok.y=(i-rt->rows/2)*-26;
	
	
	
	
	for (j=0; j<rt->cols; j++)
	{
	    lok.x=(j-rt->cols/2)*13;
	    if(1)//draw_edges_between_different_bgs
	    {
		if((j>0))
		    if((ROTE_ATTR_BG(rt->cells[i][j-1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaaz",1);
		if((j<rt->cols-1))
		    if((ROTE_ATTR_BG(rt->cells[i][j+1].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"zazz",1);
		if((i<rt->rows-1))
		    if((ROTE_ATTR_BG(rt->cells[i+1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"azzz",1);
		if((i>0))
		    if((ROTE_ATTR_BG(rt->cells[i-1][j].attr))!=(ROTE_ATTR_BG(rt->cells[i][j].attr)))
			zspillit(lok,"aaza",1);
	    }
	    printf("%i",ROTE_ATTR_FG(rt->cells[i][j].attr));
	    material=ROTE_ATTR_FG(rt->cells[i][j].attr);
	    draw(lok,rt->cells[i][j].ch);
	    material=0;
	    int selectedtext;
	    selectedtext=((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor
	    if(selectedtext) //do nice rotation animation hehe
	    {
		zspillit(lok,nums[0],1.3);
		if(showhex)
		{
		    printf("WTF\n");
		    xy loko;
		    loko.y=lok.y+13;
		    char *h;
		    int len;
		    int x = rt->cells[i][j].ch;
		    printf("0x%x\n",x);
		    h=(-1!=(len=asprintf(&h,"%i",x))?h:"??");
		    loko.x=lok.x-7.5*len;
		    draw_line(loko.x,loko.y,h);
		}
	    }

	}
    }
    char *trmsqrm=0; // message under terminal window
    if (!rt->childpid)
    {
	trmsqrm="HDD full";
    }
	
    if (trmsqrm)
    {
        draw_line(0,lok.y+30,trmsqrm);
    }
    oldcrow=rt->crow;
    oldccol=rt->ccol;
}
