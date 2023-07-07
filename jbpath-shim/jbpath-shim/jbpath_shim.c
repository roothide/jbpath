#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <spawn.h>
#include <utime.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
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
    if(newpath) free((void*)newpath);\
    return ret;\
}

#define JBPATHAT_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,FD,PATHARG,ATFLAG) EXPORT RET jbpath_shim_##NAME ARGTYPES {\
if(0)printf("*jbpathat_shim_%s\n", #NAME);\
    char* newpath = (char*)jbpathat_alloc(FD,PATHARG,ATFLAG);\
    RET ret = NAME ARGS;\
    if(newpath) free((void*)newpath);\
    return ret;\
}

//force define
char* realpath(const char * path, char *resolved_path);
char* realpath$DARWIN_EXTSN(const char * path, char *resolved_path);
FILE* fopen(const char * __restrict __filename, const char * __restrict __mode);
FILE* fopen$DARWIN_EXTSN(const char * __restrict __filename, const char * __restrict __mode);

int open$NOCANCEL(const char * path, int flags, ...);
int openat$NOCANCEL(int fd, const char * path, int flags, ...);
int creat$NOCANCEL(const char * path, int mode);
int fcntl$NOCANCEL(int, int, ...);
int system$NOCANCEL(const char *);

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
    if(newpath) free((void*)newpath);
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
    if(newpath) free((void*)newpath);
    return ret;
}


int JBPATH_SHIM_API(link)(const char *name1, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpath_alloc(name2);
    int ret = link(newname1, newname2);
    if(newname1) free((void*)newname1);
    if(newname2) free((void*)newname2);
    return ret;
}

int JBPATH_SHIM_API(symlink)(const char *name1, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpath_alloc(name2);
    int ret = symlink(newname1, newname2);
    if(newname1) free((void*)newname1);
    if(newname2) free((void*)newname2);
    return ret;
}


int JBPATH_SHIM_API(execvp)(const char * __file, char * const * __argv)
{
    /* If it's an absolute or relative path name, we need to handle. */
    if(__file && __file[0] && strchr(__file, '/'))
    {
        const char* newpath = jbpath_alloc(__file);
        int ret = execvp(newpath, __argv);
        if(newpath) free((void*)newpath);
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
        if(newpath) free((void*)newpath);
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

    return JBPATH_SHIM_API(execv)(path, argv);
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
    return JBPATH_SHIM_API(execve)(path, argv, envp);
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

    return JBPATH_SHIM_API(execvp)(file, argv);
}


int JBPATH_SHIM_API(rename)(const char *__old, const char *__new)
{
    const char* new__old = jbpath_alloc(__old);
    const char* new__new = jbpath_alloc(__new);
    int ret = rename(new__old, new__new);
    if(new__old) free((void*)new__old);
    if(new__new) free((void*)new__new);
    return ret;
}

int JBPATHAT_SHIM_API(linkat)(int fd1, const char *name1, int fd2, const char *name2, int flag)
{
    /* flag is for name1 and only support AT_SYMLINK_FOLLOW, but its not about find path */
    const char* newname1 = jbpathat_alloc(fd1, name1, 0);
    const char* newname2 = jbpathat_alloc(fd2, name2, 0);
    int ret = linkat(fd1, newname1, fd2, newname2, flag);
    if(newname1) free((void*)newname1);
    if(newname2) free((void*)newname2);
    return ret;
}

int JBPATHAT_SHIM_API(symlinkat)(const char *name1, int fd, const char *name2)
{
    const char* newname1 = jbpath_alloc(name1);
    const char* newname2 = jbpathat_alloc(fd, name2, 0); //***********
    int ret = symlinkat(newname1, fd, newname2);
    if(newname1) free((void*)newname1);
    if(newname2) free((void*)newname2);
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
    if(newpath) free((void*)newpath);
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
    if(newpath) free((void*)newpath);
    return ret;
}

int JBPATH_SHIM_API(copyfile)(const char * from, const char * to, copyfile_state_t state, copyfile_flags_t flags)
{
    const char* newfrom = jbpath_alloc(from);
    const char* newto = jbpath_alloc(to);
    int ret = copyfile(newfrom, newto, state, flags);
    if(newfrom) free((void*)newfrom);
    if(newto) free((void*)newto);
    return ret;
}

int JBPATHAT_SHIM_API(renameat)(int fromfd, const char *from, int tofd, const char *to)
{
    const char* newfrom = jbpathat_alloc(fromfd, from, 0);
    const char* newto = jbpathat_alloc(tofd, to, 0);
    int ret = renameat(fromfd, newfrom, tofd, newto);
    if(newfrom) free((void*)newfrom);
    if(newto) free((void*)newto);
    return ret;
}

int JBPATH_SHIM_API(renamex_np)(const char *__old, const char *__new, unsigned int flags)
{
    const char* new__old = jbpath_alloc(__old);
    const char* new__new = jbpath_alloc(__new);
    int ret = renamex_np(new__old, new__new, flags);
    if(new__old) free((void*)new__old);
    if(new__new) free((void*)new__new);
    return ret;
}

int JBPATHAT_SHIM_API(renameatx_np)(int fromfd, const char *from, int tofd, const char *to, unsigned int flags)
{
    const char* newfrom = jbpathat_alloc(fromfd, from, flags);
    const char* newto = jbpathat_alloc(tofd, to, flags);
    int ret = renameatx_np(fromfd, newfrom, tofd, newto, flags);
    if(newfrom) free((void*)newfrom);
    if(newto) free((void*)newto);
    return ret;
}

int JBPATH_SHIM_API(exchangedata)(const char * path1,const char * path2,unsigned int options)
{
    const char* newpath1 = jbpath_alloc(path1);
    const char* newpath2 = jbpath_alloc(path2);
    int ret = exchangedata(newpath1, newpath2, options);
    if(newpath1) free((void*)newpath1);
    if(newpath2) free((void*)newpath2);
    return ret;
}

char* JBPATH_SHIM_API(realpath)(const char * path, char *resolved_path)
{
    char pathbuf[PATH_MAX]={0};
    const char* newpath = jbpath_alloc(path);
    char* ret = realpath(newpath, pathbuf);

    //ret = pathbuf or NULL
    if(ret || resolved_path)
    {
        const char* rp = jbpath_revert_alloc(pathbuf);
        //if(!rp)
////        {
//            FILE* fp = fopen("/var/revert.log", "a");
//            fprintf(fp, "path=%p,%s newpath=%p,%s pathbuf=%s rp=%p,%s ret=%p,arg=%p\n", path,path, newpath,newpath, pathbuf, rp,rp,  ret,resolved_path);
//            fclose(fp);
////        }

        if(resolved_path) {
            strncpy(resolved_path, rp, PATH_MAX);
            if(ret) ret = resolved_path;
        }
        else if(ret) {
            ret = strdup(rp);
        }

        free((void*)rp);
    }
    
    if(newpath) free((void*)newpath);
    
    return ret;
}

char* JBPATH_SHIM_API(realpath$DARWIN_EXTSN)(const char * path, char *resolved_path)
{
    char pathbuf[PATH_MAX]={0};
    const char* newpath = jbpath_alloc(path);
    char* ret = realpath$DARWIN_EXTSN(newpath, pathbuf);

    if(ret || resolved_path)
    {
        const char* rp = jbpath_revert_alloc(pathbuf);
        //if(!rp)
//        {
//            FILE* fp = fopen("/var/revert.log", "a");
//            fprintf(fp, "path=%p,%s newpath=%p,%s pathbuf=%s rp=%p,%s ret=%p,arg=%p\n", path,path, newpath,newpath, pathbuf, rp,rp,  ret,resolved_path);
//            fclose(fp);
//        }

        if(resolved_path) {
            strncpy(resolved_path, rp, PATH_MAX);
            if(ret) ret = resolved_path;
        }
        else if(ret) {
            ret = strdup(rp);
        }

        free((void*)rp);
    }
    
    if(newpath) free((void*)newpath);
    
    return ret;
}

#define F_OPENFROM      56              /* SPI: open a file relative to fd (must be a dir) */
#define F_UNLINKFROM    57              /* SPI: open a file relative to fd (must be a dir) */
#define F_CHECK_OPENEVT 58              /* SPI: if a process is marked OPENEVT, or in O_EVTONLY on opens of this vnode */
#define F_OFD_SETLK             90      /* Acquire or release open file description lock */
#define F_OFD_SETLKW            91      /* (as F_OFD_SETLK but blocking if conflicting lock) */
#define F_OFD_GETLK             92      /* Examine OFD lock */
#define F_OFD_SETLKWTIMEOUT     93      /* (as F_OFD_SETLKW but return if timeout) */
#define F_OFD_GETLKPID          94      /* get record locking information */
#define F_SETCONFINED           95      /* "confine" OFD to process */
#define F_GETCONFINED           96      /* is-fd-confined? */
int JBPATH_SHIM_API(fcntl)(int fd, int cmd, ...)
{
    va_list ap;
    void *arg;

    va_start(ap, cmd);
    switch (cmd) {
    case F_GETLK:
    case F_GETLKPID:
    case F_SETLK:
    case F_SETLKW:
    case F_SETLKWTIMEOUT:
    case F_OFD_GETLK:
    case F_OFD_GETLKPID:
    case F_OFD_SETLK:
    case F_OFD_SETLKW:
    case F_OFD_SETLKWTIMEOUT:
    case F_PREALLOCATE:
    case F_PUNCHHOLE:
    case F_SETSIZE:
    case F_RDADVISE:
    case F_LOG2PHYS:
    case F_LOG2PHYS_EXT:
    case F_GETPATH:
    case F_GETPATH_NOFIRMLINK:
    case F_GETPATH_MTMINFO:
    case F_GETCODEDIR:
    case F_PATHPKG_CHECK:
    case F_OPENFROM:
    case F_UNLINKFROM:
    case F_ADDSIGS:
    case F_ADDFILESIGS:
    case F_ADDFILESIGS_FOR_DYLD_SIM:
    case F_ADDFILESIGS_RETURN:
    case F_ADDFILESIGS_INFO:
    case F_ADDFILESUPPL:
    case F_FINDSIGS:
    case F_TRANSCODEKEY:
    case F_TRIM_ACTIVE_FILE:
    case F_SPECULATIVE_READ:
    case F_CHECK_LV:
    case F_GETSIGSINFO:
        arg = va_arg(ap, void *);
        break;
    default:
        arg = (void *)((unsigned long)va_arg(ap, int));
        break;
    }
    va_end(ap);
    
    int ret = fcntl(fd, cmd, arg);
    
    if(ret == 0)
    {
        if(cmd == F_GETPATH)
        {
            char* pathbuf = arg;
            if(pathbuf) {
                const char* rp = jbpath_revert_alloc(pathbuf);
                strncpy(pathbuf, rp, PATH_MAX);
                if(rp) free((void*)rp);
            }
        }
    }
    
    return ret;
}

int stat64(const char *, void *);
void __testfunc()
{
    stat64(0,0);
}

