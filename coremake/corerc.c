/*
  Copyright (c) 2006-2009, CoreCodec, Inc.
  All rights reserved.

  Redistribution and use in source and binary forms, with or without 
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, 
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, 
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the CoreCodec, Inc. nor the names of its contributors 
    may be used to endorse or promote products derived from this software 
    without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void pathunix(char* path)
{
	int i;
	for (i=0;path[i];++i)
		if (path[i]=='\\')
			path[i]='/';
}

char* getfilename(const char* path)
{
	char* i = strrchr(path,'/');
	if (i) return i+1;
	return (char*)path;
}

void truncfilename(char* path)
{
	char* i = getfilename(path);
	memmove(path,i,strlen(i)+1);
	i = strrchr(path,'.');
	if (i) *i=0;
}

int main(int argc, char** argv)
{
    FILE *f = NULL;
    FILE *e = stdout;
    char filename[256];
    char varname[256];
    char name[256];
    char align[32];
    int writeable=0;
    int corec=0;
    int n;
    
    if (argc<=1)
    {
        printf("usage: %s [-a <num>] [-w] [-corec] [-n <name>] <infile.bin> [<outfile.c>]\n",argv[0]);
        return 1;
    }

    align[0]=0;
    varname[0]=0;

    for (n=1;n<argc;++n)
    {
        if (argv[n][0]=='-')
        {
            if (argv[n][1]=='c')
                corec=1;
            else
            if (argv[n][1]=='w')
                writeable = 1;
            else
            if (argv[n][1]=='a')
                sprintf(align,"__attribute__((aligned(%d)))",atoi(argv[++n]));
            else
            if (argv[n][1]=='n')
                strcpy(varname,argv[++n]);
        }
        else
        {
            if (!f)
            {
                strcpy(name,argv[n]);
                f = fopen(argv[n],"rb");
                if (!f)
                    return 1;
            }
            else
            {
                e = fopen(argv[n],"w+");
            }
        }
    }

    pathunix(name);
    strcpy(filename,getfilename(name));
    truncfilename(name);

    if (!varname[0])
        strcpy(varname,name);

    fprintf(f,"/* DO NOT EDIT, FILE GENERATED BY CORERC */\n\n");

    if (corec)
    {
        fprintf(e,"#include \"resource/resource.h\"\n\n");
        fprintf(e,"static %s uint8_t Data[] %s = {",writeable?"":"const",align);
    }
    else
    {
        fseek(f,0,SEEK_END);
        fprintf(e,"%s int %s_size = %d;\n",writeable?"":"const",varname,(int)ftell(f));
        fseek(f,0,SEEK_SET);
        fprintf(e,"%s char %s[] %s = {",writeable?"":"const",varname,align);
    }

    for (n=0;;++n)
    {
        int ch = fgetc(f);
        if (feof(f))
            break;

        if (n)
            fprintf(e,",");
        if ((n%32)==0)
            fprintf(e,"\n");
        fprintf(e,"0x%02X",(unsigned char)ch);
    }

    fprintf(e,"};\n\n");

    if (corec)
    {
        fprintf(e,"META_START(Resource_%s_Class,0)\n"
                  "META_PARAM(STRING,NODE_NAME,T(\"%s\"))\n"
                  "META_PARAM(CUSTOM,RESOURCEDATA_SIZE,sizeof(Data))\n"
                  "META_PARAM(CUSTOM,RESOURCEDATA_PTR,Data)\n"
                  "META_END(RESOURCEDATA_ID)\n",name,filename);
    }

    fclose(f);
    if (e != stdout)
        fclose(e);
	return 0;
}
