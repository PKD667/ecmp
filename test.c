#include "hashtable.h"
#include <stdio.h>

#include "spm/libspm.h"

int open(char* path,struct package* pkg);

int main()
{
    DEBUG = 3;
    struct package pkg = {0};
    open("./test.ecmp",&pkg);

    // print package

    printf("name: %s\n",pkg.name);
    printf("version: %s\n",pkg.version);
    printf("type: %s\n",pkg.type);
    printf("url: %s\n",pkg.url);
    printf("license: %s\n",pkg.license);
    printf("make: %s\n",pkg.info.make);
    printf("install: %s\n",pkg.info.install);
    printf("download: %s\n",pkg.info.download);
    printf("prepare: %s\n",pkg.info.prepare);
    printf("special: %s\n",pkg.info.special);
    


    return 0;
}