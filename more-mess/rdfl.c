
#define rdfl  GetFileIntoCharPointer1
#include <errno.h>

char * GetFileIntoCharPointer1(char *pFile)
{
    FILE * fp = fopen(pFile,"r");
    if (fp == NULL) 
    {
	return 0;
    }
    char *pData=0;
    if ((!fseek(fp, 0, SEEK_END)))
    {
        errno=0;
        long size = ftell(fp);
        if (!errno)
        {
	    if(!fseek(fp, 0, SEEK_SET))
    	    {
	        if ((pData =  malloc(size + 10)))
		{
		    if(fread(pData, sizeof(char), size, fp))
			pData[size]=0;
		    else
		    {
			free(pData);
			pData=0;
		    }
/*		    pData[size+1]='w';
		    pData[size+2]='r';
		    pData[size+3]='o';
		    pData[size+4]='n';
		    pData[size+5]='g';
		    pData[size+6]='w';
		    pData[size+7]='a';
		    pData[size+8]='y';
		    pData[size+9]=0;*/
		}
	    }
	}
    }
    fclose(fp);
    return pData;
}

