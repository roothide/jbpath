#ifndef cache_hpp
#define cache_hpp

#include <stdio.h>
#include <sys/stat.h>

struct cache_key {
    const char* oldpath;
    dev_t at_dev;
    ino_t at_ino;
};


struct cache_value {
    const char* newpath;
    dev_t cache_dev;
    ino_t cache_ino;
};


int jbpath_cache_find(cache_value* cv);

#endif /* cache_hpp */
