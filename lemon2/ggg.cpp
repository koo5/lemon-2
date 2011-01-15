#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <../pstreams-0.7.0/pstream.h>

string char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;

    string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}


string urlencode(const string &c)
{
    //http://www.zedwood.com/article/111/cpp-urlencode-function
    string escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
             (65 <= c[i] && c[i] <= 90) ||//abc...xyz
             (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
             (c[i]=='~' || c[i]=='_' || c[i]=='-' || c[i]=='.')
        )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}


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
	string x = "http://morph.talis.com/?input=&output=json&data-uri[]=" + urlencode(url);
	curl_easy_setopt(curl_handle, CURLOPT_URL, x.c_str());
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
	cout << raw.str() << endl;
	ostringstream f;
	f << dmps << runtime_id << ".html";
	string fn(f.str());
	ofstream o(fn);
	o << raw;
	o.close();
	string cmd;
	cmd = "links -dump "+fn+" || w3m -dump " +fn+ " || lynx -dump -force_html " +fn+ " || cat " +fn;
	redi::ipstream in(cmd);
	string str;
	while (in >> str) {
        std::cout << str << std::endl;
	}
    }
};


