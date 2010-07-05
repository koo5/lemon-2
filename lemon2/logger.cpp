class logger:public terminal
{
    public:
    logger(double x, double y, double z, double a, double b, double c)
    {
	obj::t.x=x;obj::t.y=y;obj::t.z=z;
	r.x=a;r.y=b;r.z=c;
	t = rote_vt_create(100,100);
	t->cursorhidden=1;//
    }
	
	
    ~logger()
    {
	rote_vt_destroy(t);
    }
    
    void logit(char *s)
    {
	rote_vt_write(t, s, strlen(s)+1);
	rote_vt_write(t,"\n",1);
    }
    
    void slogit(char *s)
    {
	updatestatus(s);
    }
};

