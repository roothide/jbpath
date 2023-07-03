#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <spawn.h>
#include <utime.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <copyfile.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/fcntl.h>
#include <sys/semaphore.h>
#include <sys/statvfs.h>

#define __DBINTERFACE_PRIVATE
#include <db.h>


#include "jbpath.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#define jbpathat_alloc(a,b,c) jbpathat_alloc(a,b)


#undef JBPATH_SHIM_API
#undef JBPATHAT_SHIM_API
#undef JBPATH_SHIM
#undef JBPATHAT_SHIM

#define JBPATH_SHIM_API(NAME) jbpath_shim_##NAME
#define JBPATHAT_SHIM_API(NAME) jbpath_shim_##NAME

#define EXPORT __attribute__ ((visibility ("default")))
#define JBPATH_SHIM_DEF(RET,NAME,ARGTYPES) EXPORT RET jbpath_shim_##NAME ARGTYPES ;
#define JBPATHAT_SHIM_DEF(RET,NAME,ARGTYPES) EXPORT RET jbpath_shim_##NAME ARGTYPES ;

#define JBPATH_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,PATHARG) EXPORT RET jbpath_shim_##NAME ARGTYPES {\
if(0)printf("*jbpath_shim_%s\n", #NAME);\
    char* newpath = (char*)jbpath_alloc(PATHARG);\
    RET ret = NAME ARGS;\
    free((void*)newpath);\
    return ret;\
}

#define JBPATHAT_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,FD,PATHARG,ATFLAG) EXPORT RET jbpath_shim_##NAME ARGTYPES {\
if(0)printf("*jbpathat_shim_%s\n", #NAME);\
    char* newpath = (char*)jbpathat_alloc(FD,PATHARG,ATFLAG);\
    RET ret = NAME ARGS;\
    free((void*)newpath);\
    return ret;\
}

//force define
char* realpath(const char * path, char *resolved_path);
char* realpath$DARWIN_EXTSN(const char * path, char *resolved_path);
FILE* fopen(const char * __restrict __filename, const char * __restrict __mode);
FILE* fopen$DARWIN_EXTSN(const char * __restrict __filename, const char * __restrict __mode);
//c++
void* _ZNSt3__113basic_filebufIcNS_11char_traitsIcEEE4openEPKcj(void* thiz, const char* __s, unsigned int mode);
void* _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj(void* thiz, const char* __s, unsigned int mode);
void* _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, void* __s, unsigned int mode);
void* _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj(void* thiz, const char* __s, unsigned int mode);
void* _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj(void* thiz, void* __s, unsigned int mode);

int oflag_to_atflag(int oflag)
{
    int atflag=0;
    if(oflag & O_NOFOLLOW) atflag |= AT_SYMLINK_NOFOLLOW;
    if(oflag & O_NOFOLLOW_ANY) atflag |= AT_SYMLINK_NOFOLLOW_ANY;
    return atflag;
}

#define JBPATH_SHIM_INTERNAL
#include "jbpath_shim.h"


int JBPATH_SHIM_API(open)(const char * path, int flags, ...)
{
    mode_t mode = 0;
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    const char* newpath = jbpathat_alloc(AT_FDCWD, path, oflag_to_atflag(flags));
    int ret = open(newpath, flags, mode);
    free((void*)newpath);
    return ret;
}

int JBPATH_SHIM_API(openat)(int fd, const char * path, int flags, ...)
{
    mode_t mode = 0;
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    const char* newpath = jbpathat_alloc(fd, path, oflag_to_atflag(flags));
    int ret = openat(fd, newpath, flags, mode);
    free((void*)newpath);
    return ret;
}


int JBPATH_SHIM_API(link)(const char *name1, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpath_alloc(name2);
    int ret = link(newname1, newname2);
    free((void*)newname1);
    free((void*)newname2);
    return ret;
}

int JBPATH_SHIM_API(symlink)(const char *name1, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpath_alloc(name2);
    int ret = symlink(newname1, newname2);
    free((void*)newname1);
    free((void*)newname2);
    return ret;
}


int JBPATH_SHIM_API(execvp)(const char * __file, char * const * __argv)
{
    /* If it's an absolute or relative path name, we need to handle. */
    if(__file && __file[0] && strchr(__file, '/'))
    {
        const char* newpath = jbpath_alloc(__file);
        int ret = execvp(newpath, __argv);
        free((void*)newpath);
        return ret;
    }
    return execvp(__file, __argv);
}

int JBPATH_SHIM_API(execvP)(const char * __file, const char * __searchpath, char * const * __argv)
{
    /* If it's an absolute or relative path name, we need to handle. */
    if(__file && __file[0] && strchr(__file, '/'))
    {
        const char* newpath = jbpath_alloc(__file);
        int ret = execvP(newpath, __searchpath, __argv);
        free((void*)newpath);
        return ret;
    }
    return execvP(__file, __searchpath, __argv);
}

int JBPATH_SHIM_API(execl)(const char *path, const char *arg0, ...)
{
    va_list args;
    va_start(args, arg0);

    va_list args_copy;
    va_copy(args_copy, args);
    int arg_count = 0;
    for (char *arg = va_arg(args_copy, char*); arg != NULL; arg = va_arg(args_copy, char*)) {
        arg_count++;
    }
    va_end(args_copy);

    char *argv[arg_count+1];
    for (int i = 0; i < arg_count-1; i++) {
        char *arg = va_arg(args, char*);
        argv[i] = arg;
    }
    argv[arg_count] = NULL;

    return jbpath_shim_execv(path, argv);
}

int JBPATH_SHIM_API(execle)(const char *path, const char *arg0, ... /*, (char *)0, char *const envp[] */)
{
    va_list args;
    va_start(args, arg0);

    va_list args_copy;
    va_copy(args_copy, args);
    int arg_count = 0;
    for (char *arg = va_arg(args_copy, char *); arg != NULL; arg = va_arg(args_copy, char *)) {
        arg_count++;
    }
    va_end(args_copy);

    char *argv[arg_count+1];
    for (int i = 0; i < arg_count-1; i++) {
        char *arg = va_arg(args, char*);
        argv[i] = arg;
    }
    argv[arg_count] = NULL;

    //char *nullChar = va_arg(args, char*);

    char **envp = va_arg(args, char**);
    return jbpath_shim_execve(path, argv, envp);
}

int JBPATH_SHIM_API(execlp)(const char *file, const char *arg0, ...)
{
    va_list args;
    va_start(args, arg0);

    // Get argument count
    va_list args_copy;
    va_copy(args_copy, args);
    int arg_count = 0;
    for (char *arg = va_arg(args_copy, char*); arg != NULL; arg = va_arg(args_copy, char*)) {
        arg_count++;
    }
    va_end(args_copy);

    char *argv[arg_count+1];
    for (int i = 0; i < arg_count-1; i++) {
        char *arg = va_arg(args, char*);
        argv[i] = arg;
    }
    argv[arg_count] = NULL;

    return jbpath_shim_execvp(file, argv);
}


int JBPATH_SHIM_API(rename)(const char *__old, const char *__new)
{
    const char* new__old = jbpath_alloc(__old);
    const char* new__new = jbpath_alloc(__new);
    int ret = rename(new__old, new__new);
    free((void*)new__old);
    free((void*)new__new);
    return ret;
}

int JBPATHAT_SHIM_API(linkat)(int fd1, const char *name1, int fd2, const char *name2, int flag)
{
    /* flag is for name1 and only support AT_SYMLINK_FOLLOW, but its not about find path */
    const char* newname1 = jbpathat_alloc(fd1, name1, 0);
    const char* newname2 = jbpathat_alloc(fd2, name2, 0);
    int ret = linkat(fd1, newname1, fd2, newname2, flag);
    free((void*)newname1);
    free((void*)newname2);
    return ret;
}

int JBPATHAT_SHIM_API(symlinkat)(const char *name1, int fd, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpathat_alloc(fd, name2, 0); //***********
    int ret = symlinkat(newname1, fd, newname2);
    free((void*)newname1);
    free((void*)newname2);
    return ret;
}

int JBPATH_SHIM_API(shm_open)(const char * path, int flags, ...)
{
    mode_t mode = 0;
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    const char* newpath = jbpath_alloc(path);
    int ret = shm_open(newpath, flags, mode);
    free((void*)newpath);
    return ret;
}

sem_t* JBPATH_SHIM_API(sem_open)(const char *path, int flags, ...)
{
    mode_t mode = 0;
    va_list ap;
    va_start(ap, flags);
    mode = va_arg(ap, int);
    va_end(ap);
    const char* newpath = jbpath_alloc(path);
    sem_t* ret = sem_open(newpath, flags, mode);
    free((void*)newpath);
    return ret;
}

int JBPATH_SHIM_API(copyfile)(const char * from, const char * to, copyfile_state_t state, copyfile_flags_t flags)
{
    const char* newfrom = jbpath_alloc(from);
    const char* newto = jbpath_alloc(to);
    int ret = copyfile(newfrom, newto, state, flags);
    free((void*)newfrom);
    free((void*)newto);
    return ret;
}

int JBPATHAT_SHIM_API(renameat)(int fromfd, const char *from, int tofd, const char *to)
{
    const char* newfrom = jbpathat_alloc(fromfd, from, 0);
    const char* newto = jbpathat_alloc(tofd, to, 0);
    int ret = renameat(fromfd, newfrom, tofd, newto);
    free((void*)newfrom);
    free((void*)newto);
    return ret;
}

int JBPATH_SHIM_API(renamex_np)(const char *__old, const char *__new, unsigned int flags)
{
    const char* new__old = jbpath_alloc(__old);
    const char* new__new = jbpath_alloc(__new);
    int ret = renamex_np(new__old, new__new, flags);
    free((void*)new__old);
    free((void*)new__new);
    return ret;
}

int JBPATHAT_SHIM_API(renameatx_np)(int fromfd, const char *from, int tofd, const char *to, unsigned int flags)
{
    const char* newfrom = jbpathat_alloc(fromfd, from, flags);
    const char* newto = jbpathat_alloc(tofd, to, flags);
    int ret = renameatx_np(fromfd, newfrom, tofd, newto, flags);
    free((void*)newfrom);
    free((void*)newto);
    return ret;
}

int JBPATH_SHIM_API(exchangedata)(const char * path1,const char * path2,unsigned int options)
{
    const char* newpath1 = jbpath_alloc(path1);
    const char* newpath2 = jbpath_alloc(path2);
    int ret = exchangedata(newpath1, newpath2, options);
    free((void*)newpath1);
    free((void*)newpath2);
    return ret;
}

char* JBPATH_SHIM_API(mkdtemp)(char* path)
{
    mktemp(path);
    
    const char* newpath = jbpath_alloc(path);
    char* ret = mkdtemp((char*)newpath);
    free((void*)newpath);
        
    return ret ? path : NULL;
}

int JBPATH_SHIM_API(mkstemp)(char * path)
{
    mktemp(path);
    
    const char* newpath = jbpath_alloc(path);
    int ret = mkstemp((char*)newpath);
    free((void*)newpath);
    return ret;
}

int JBPATH_SHIM_API(mkostemp)(char * path, int oflags)
{
    mktemp(path);
    
    const char* newpath = jbpath_alloc(path);
    int ret = mkostemp((char*)newpath, oflags);
    free((void*)newpath);
    return ret;
}

//https://github.com/apple-oss-distributions/Libc/blob/7861c72b1692b65f79c03f21a8a1a8e51e14c843/stdio/FreeBSD/mktemp.c#L104
int mktemps(int dfd, char* path, int slen)
{
    size_t len = strlen(path);
    if(len < slen)
        return 0;
    
    char suffix[PATH_MAX];
    strncpy(suffix, path+len-slen, slen);
    bzero(path+len-slen, slen);
    
    if(strchr(suffix, '/'))
        return 0;
    
    char tmp[PATH_MAX];
    
    while(true)
    {
        strncpy(tmp, path, PATH_MAX);
        mktemp(tmp);
        strcat(tmp, suffix);
        
        struct stat sbuf;
        if (fstatat(dfd, path, &sbuf, AT_SYMLINK_NOFOLLOW))
        {
            if(errno == ENOENT) break;
        }
    }
    
    strncpy(path, tmp, PATH_MAX);
    
    return 1;
}

int JBPATH_SHIM_API(mkstemps)(char *_template, int suffixlen)
{
    mktemps(AT_FDCWD, _template, suffixlen);
    
    const char* newpath = jbpath_alloc(_template);
    int ret = mkstemps((char*)newpath, suffixlen);
    free((void*)newpath);
    return ret;
}

int JBPATH_SHIM_API(mkostemps)(char * path, int slen, int oflags)
{
    mktemps(AT_FDCWD, path, slen);
    
    const char* newpath = jbpath_alloc(path);
    int ret = mkostemp((char*)newpath, oflags);
    free((void*)newpath);
    return ret;
}

#ifdef TARGET_OS_IPHONE
int JBPATHAT_SHIM_API(mkstemp_dprotected_np)(char *path, int dpclass, int dpflags)
{
    mktemp(path);
    
    const char* newpath = jbpathat_alloc(fd, path);
    int ret = mkstemp_dprotected_np((char*)newpath, dpclass, dpflags);
    free((void*)newpath);
    return ret;
}
#endif

char* JBPATHAT_SHIM_API(mkdtempat_np)(int dfd, char *path)
{
    mktemp(path);
    
    const char* newpath = jbpathat_alloc(dfd, path, 0);
    char* ret = mkdtempat_np(dfd, (char*)newpath);
    free((void*)newpath);
        
    return ret ? path : NULL;
}

int JBPATHAT_SHIM_API(mkstempsat_np)(int dfd, char *path, int slen)
{
    mktemps(dfd, path, slen);
    
    const char* newpath = jbpathat_alloc(dfd, path, 0);
    int ret = mkstempsat_np(dfd, (char*)newpath, slen);
    free((void*)newpath);
        
    return ret;
}

int JBPATHAT_SHIM_API(mkostempsat_np)(int dfd, char*path, int slen, int oflags)
{
    mktemps(dfd, path, slen);
    
    const char* newpath = jbpathat_alloc(dfd, path, 0);
    int ret = mkostempsat_np(dfd, (char*)newpath, slen, oflags);
    free((void*)newpath);
        
    return ret;
}

//int stat64(const char *, void *);
//void __testfunc()
//{
//    stat64(0,0);
//}

