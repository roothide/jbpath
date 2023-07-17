#include "jbroot.h"

std::string jbroot(std::string path)
{
    const char* _p = jbroot_alloc(path.c_str());
    if(!_p) return path;
    std::string newpath(_p);
    free((void*)_p);
    return newpath;
}
