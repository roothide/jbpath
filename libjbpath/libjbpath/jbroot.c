#include "jbroot.h"

//free after use
const char* jbroot_alloc(const char* path)
{
    if(!path || !*path) return NULL;
    
    char* jbroot = getenv("JBROOT");
    int pathlen = strlen(jbroot) + strlen(path) + 2;
    char* newpath = malloc(pathlen);
    strcpy(newpath, jbroot);
    if(newpath[0]!='/')
        strlcat(newpath, "/", pathlen);
    strlcat(newpath, path, pathlen);
    return newpath;
}

//use cache
const char* jbroot(const char* path)
{
    const char* newpath = jbroot_alloc(path);
    //cache here
    return newpath;
}

const char* jbroot_revert(const char* path)
{
    return NULL;
}
