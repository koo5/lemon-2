
#include "stdio.h"

int main()
{

fputs("\x1B[?1002h", stdout);
while(fputc(getchar(),stdout));

}