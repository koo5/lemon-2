void resize(int *w,int *h,int *bpp,Uint32 *video_flags,int *shrink, int *grow)
{
    printf("resizooo\n");
    xy r = parsemodes(*w,*h,"modes",0,*shrink,*grow);
    if ((r.x!=-1))
    {
	if (SDL_SetVideoMode( r.x,r.y, *bpp, *video_flags ) ) 
	{
	    *w=r.x;
	    *h=r.y;
	}
    }
    *grow=*shrink=0;
}

