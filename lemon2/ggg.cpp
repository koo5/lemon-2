#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct gggw:public obj
{
    string url;
    stringstream raw;
    SAVE(gggw)
    {
	YAML_EMIT_PARENT_MEMBERS(out,obj)
	save(url)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,obj)
    	load(url)
    }
    gggw(const string uurl="root.cz")
    {
	url=uurl;
	s.x=0.01;
	s.y=-0.03;
	s.z=0.002;
    }
    size_t write_raw(void *ptr, size_t size)
    {
	raw.write((char*)ptr, size);
	return size;
    }
    void draw(int picking)
    {
	glColor4f(0,0,1,alpha);
	draw_text(raw.str().c_str());
    }
};


static size_t write_raw_callback(void *ptr, size_t size, size_t nmemb, void *data)
{
    return static_cast<gggw*>(data)->write_raw(ptr, size * nmemb);
}

struct ggg:public gggw
{
    SAVE(ggg)
    {
	YAML_EMIT_PARENT_MEMBERS(out,gggw)
    }
    LOAD
    {
    	YAML_LOAD_PARENT_MEMBERS(doc,gggw)
    }
    ggg(){}
    void go()
    {
	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &write_raw_callback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)this);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "lemon-ggg/0.01");
	curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
    }
    void keyp(int key,int uni,int mod)
    {
	go();
	cout << "xxx" << endl;
    }
};


