#include "jbpath.h"

std::string jbpath(std::string path)
{
    const char* _p = jbpath_alloc(path.c_str());
    if(!_p) return path;
    std::string newpath(_p);
    free((void*)_p);
    return newpath;
}
