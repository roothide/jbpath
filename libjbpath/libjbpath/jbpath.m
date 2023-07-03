#include "jbpath.h"

NSString* __attribute__((overloadable)) jbpath(NSString* path)
{
    const char* _p = jbpath_alloc(path.UTF8String);
    if(!_p) return path;
    NSString* newpath = [NSString stringWithUTF8String:_p];
    free((void*)_p);
    return newpath;
}

NSString* __attribute__((overloadable)) jbroot(NSString* path)
{
    const char* _p = jbroot_alloc(path.UTF8String);
    NSString* newpath = [NSString stringWithUTF8String:_p];
    free((void*)_p);
    return newpath;
}

NSString* __attribute__((overloadable)) jbroot_revert(NSString* path)
{
    const char* _p = jbroot_revert(path.UTF8String);
    NSString* newpath = [NSString stringWithUTF8String:_p];
    free((void*)_p);
    return newpath;
}

