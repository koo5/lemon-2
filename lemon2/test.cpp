#include <sys/inotify.h>
#include <stdio.h>

#include <unistd.h>
#include <errno.h>


#include "../yaml-cpp-0.2.5/include/yaml.h"       
using namespace std;
int main()
{
    YAML::Emitter out;
    out << "hello world\n";
    cout << out.c_str();


 return 0;


}
