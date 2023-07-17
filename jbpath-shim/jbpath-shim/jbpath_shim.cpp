#include <stdio.h>
#include <string>

#include "jbroot.h"
#define jbpath_alloc(a) jbroot_alloc(a)
#define jbpathat_alloc(a,b) jbroot_alloc(b)
#define jbpath_revert_alloc(a) jbroot_revert(a)



using namespace std;

extern "C" {
void* _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj(void* thiz, const char* __s, unsigned int mode);
void* _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj(void* thiz, const char* __s, unsigned int mode);
void* _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, string* __s, unsigned int mode);
void* _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, string* __s, unsigned int mode);

void* jbpath_shim__ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, string* __s, unsigned int mode)
{
    char* newpath = (char*)jbpath_alloc(__s->c_str());
    void* ret = _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj(thiz,newpath,mode);
    if(newpath) free((void*)newpath);
    return ret;
}

void* jbpath_shim__ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, string* __s, unsigned int mode)
{
    char* newpath = (char*)jbpath_alloc(__s->c_str());
    void* ret = _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj(thiz,newpath,mode);
    if(newpath) free((void*)newpath);
    return ret;
}

}
