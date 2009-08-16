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
	    int selectedtext;
	    selectedtext=((rt->ccol==j)&&(rt->crow==i));
	    //actually , not selected but under cursor
	    if(selectedtext) //do nice rotation animation hehe
	    {

		zspillit(lok,nums[0],1.3);
	    }
/*		material=1;
		zspillit(lok,nums[0],1.3);
		material=0;
		if((oldcrow!=rt->crow)||(oldccol!=rt->ccol))
		    rotor=0;
		int oldpepv=vb.pos;
		int oldpepi=ib.pos;
		s3d_pep_vertices(object, vb.buf, vb.pos+vb.pep);
		nulizze(&vb);
		s3d_pep_lines(object, ib.buf, ib.pos+ib.pep);
		nulizze(&ib);
		curposx=lok.x;
		curposy=lok.y;
		draw(lok,rt->cells[i][j].ch);
		int oldmaterial=material;
		material=cmaterial;
		if(showhex)// so that 2d world is not sad.
		{
		    xy loko;
		    loko.y=lok.y+13;
		    char *h;
		    int g = asprintf(&h,"%i",rt->cells[i][j].ch);
		    if (g!=-1)
		    {
			loko.x=lok.x-7.5*g;
			material=1;
			lokdraw_line(loko,g!=-1 ? h : "???");
			material=0;
		    }
		}
		material=oldmaterial;
		s3d_pep_vertices(cursor, vb.buf, vb.pos+vb.pep);
		nulizze(&vb);
		s3d_pep_lines(cursor, ib.buf, ib.pos+ib.pep);
		nulizze(&ib);
		ib.pep=oldpepi;
		vb.pep=oldpepv;
	    }
	    else*/
		draw(lok,rt->cells[i][j].ch);
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
