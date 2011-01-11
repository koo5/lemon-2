struct ggg:public obj
{
    string url;
    SAVE(ggg)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
	save(url)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    	load(url)
    }
    ggg(const string uurl="bash")
    {
	url=uurl;
    }
};

