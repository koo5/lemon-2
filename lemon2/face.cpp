struct face:public terminal
{
    moomoo upd_t_data;
    SAVE(face)
    {
	YAML_EMIT_PARENT_MEMBERS(out,terminal)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,terminal)
    }
    face(char* run, double x, double y, double z, double a, double b, double c)
    {
	obj::t.x=x;obj::t.y=y;obj::t.z=z;
	r.x=a;r.y=b;r.z=c;
	add_terminal(run);
    }
    face(double x, double y, double z, double a, double b, double c)
    {
	obj::t.x=x;obj::t.y=y;obj::t.z=z;
	r.x=a;r.y=b;r.z=c;
	add_terminal("bash");
    }
    face(float x, float y, int c, int r)
    {
	add_term(c,r,"bash");
	obj::t.x=x;
	obj::t.y=y;
    }
    face(const char*run="bash")
    {
	add_terminal(run);
    }
    void type(const char * x)
    {
	while(*x)
	{
	    rote_vt_keypress(t,*x);
	    x++;
	}
    }
    ~face()
    {
	#ifdef threaded
	    SDL_DestroyMutex(upd_t_data.lock);
	    SDL_KillThread(upd_t_data.thr);
	#endif
	logit(" terminal destroyed");
    }

    #ifndef threaded
	void updateterminal()
	{
    	    rote_vt_update(t);
    	    if(d->t->stoppedscrollback)
	    	scroll=t->logl;

    	    if(!t->childpid)
    	    {
	        SDL_Event e;
	        e.type=SDL_USEREVENT;
	        e.user.code=CODE_QUIT;
	        e.user.data1=t;
	        SDL_PushEvent(&e);
	    }
	}
    #endif

    void add_term(int c,int r,const char *run)
    {
	logit("adding terminal");
	t = rote_vt_create(r,c);
	if(originalldpreload)
	    rote_vt_forkpty((RoteTerm*) t, run, "LD_PRELOAD", originalldpreload);
	else
	    rote_vt_forkpty((RoteTerm*) t, run, 0, 0);
	#ifdef threaded
	    upd_t_data.lock=SDL_CreateMutex();
	    upd_t_data.t=t;
	    logit("upd_t_data.lock=%i",upd_t_data.lock);
	    upd_t_data.thr=SDL_CreateThread(update_terminal, (void *)&upd_t_data);
	#endif
    }
    void add_terminal(const char *run)
    {
	add_term(SDL_GetVideoSurface()->w/26*3,SDL_GetVideoSurface()->h/26,run);
    }
    
	

    void lock()
    {
        #ifdef threaded
    	    _mutexP(upd_t_data.lock);
	#endif
    }
    void unlock()
    {
        #ifdef threaded
	    _mutexV(upd_t_data.lock);
	#endif
    }
};

