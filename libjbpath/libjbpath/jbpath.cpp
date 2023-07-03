#include "jbpath.h"

std::string jbpath(std::string path)
{
    const char* _p = jbpath_alloc(path.c_str());
    if(!_p) return path;
    std::string newpath(_p);
    free((void*)_p);
    return newpath;
}

std::string jbroot(std::string path)
{
    const char* _p = jbroot_alloc(path.c_str());
    std::string newpath(_p);
    free((void*)_p);
    return newpath;
}

std::string jbroot_revert(std::string path)
{
    const char* _p = jbroot_revert(path.c_str());
    std::string newpath(_p);
    free((void*)_p);
    return newpath;
}
