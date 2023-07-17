#ifndef JBPATH_SHIM_INTERNAL_H
#define JBPATH_SHIM_INTERNAL_H

#ifndef JBPATH_SHIM_INTERNAL

#define MACRO_if #if
#define MACRO_else #else
#define MACRO_elif #elif
#define MACRO_endif #endif
#define MACRO_ifdef #ifdef
#define MACRO_ifndef #ifndef
#define MACRO_elifdef #elifdef
#define MACRO_elifndef #elifndef
#define MACRO_define #define


//#define JBPATH_SHIM_DEF(RET,NAME,ARGTYPES) MACRO_define NAME(...) jbpath_shim_##NAME(__VA_ARGS__)
//#define JBPATHAT_SHIM_DEF(RET,NAME,ARGTYPES) MACRO_define NAME(...) jbpathat_shim_##NAME(__VA_ARGS__)
//#define JBPATH_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,PATHARG) MACRO_define NAME(...) jbpath_shim_##NAME(__VA_ARGS__)
//#define JBPATHAT_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,FD,PATHARG,ATFLAG) MACRO_define NAME(...) jbpathat_shim_##NAME(__VA_ARGS__)

#define JBPATH_SHIM_DEF(RET,NAME,ARGTYPES) MACRO_define NAME jbpath_shim_##NAME
#define JBPATHAT_SHIM_DEF(RET,NAME,ARGTYPES) MACRO_define NAME jbpathat_shim_##NAME
#define JBPATH_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,PATHARG) MACRO_define NAME jbpath_shim_##NAME
#define JBPATHAT_SHIM_WRAP(RET,NAME,ARGTYPES,ARGS,FD,PATHARG,ATFLAG) MACRO_define NAME jbpathat_shim_##NAME

MACRO_ifndef JBPATH_SHIM_H
MACRO_define JBPATH_SHIM_H

#endif /*JBPATH_SHIM_INTERNAL*/

#include <TargetConditionals.h>

//what about tweaks? JBPATH_SHIM_DEF(void*, dlsym, (void * __handle, const char * __symbol))

JBPATH_SHIM_WRAP(void*, dlopen, (const char * __path, int __mode), (newpath,__mode), __path)

JBPATH_SHIM_DEF(int, dladdr, (const void * addr, Dl_info * info) )

JBPATH_SHIM_WRAP(int, posix_spawn, (pid_t * pid, const char * path, const posix_spawn_file_actions_t *file_actions,
                               const posix_spawnattr_t * attrp, char *const argv[], char *const envp[]),
    (pid,newpath,file_actions,attrp,argv,envp), path);


/* fcntl.h  */

JBPATH_SHIM_DEF(int, open, (const char * path, int flags, ...))

JBPATHAT_SHIM_DEF(int, openat, (int fd, const char * path, int flags, ...))

JBPATH_SHIM_WRAP(int, creat, (const char * path, int mode), (newpath,mode), path)
JBPATHAT_SHIM_DEF(int, fcntl, (int, int, ...))
/* apple private */
JBPATH_SHIM_WRAP(int, openx_np, (const char *path, int flags, filesec_t fsec), (newpath, flags, fsec), path)
JBPATH_SHIM_DEF(int, open_dprotected_np, (const char *, int, int, int, ...))
//int openat_dprotected_np(int, const char *, int, int, int, ...); //not in dsc?
//int openat_authenticated_np(int, const char *, int, int); //not in dsc?
//

/* unistd.h */
JBPATH_SHIM_DEF(char*, getwd, (char *) )
JBPATH_SHIM_DEF(char*, getcwd, (char *, size_t) );
//not really fs access, size_t confstr(int, char *, size_t) __DARWIN_ALIAS(confstr);
JBPATH_SHIM_WRAP(int, acct, (const char *filename), (newpath), filename)
JBPATH_SHIM_WRAP(int, access, (const char * path, int mode), (newpath,mode), path)
JBPATH_SHIM_WRAP(int, chdir, (const char * path), (newpath), path)
JBPATH_SHIM_WRAP(int, chown, (const char * path,uid_t uid, gid_t gid), (newpath,uid,gid), path)

JBPATH_SHIM_WRAP(long, pathconf, (const char * path, int name), (newpath,name), path)
JBPATH_SHIM_WRAP(int, rmdir, (const char * path), (newpath), path)
JBPATH_SHIM_WRAP(int, unlink, (const char * path), (newpath), path)
JBPATH_SHIM_WRAP(int, chroot, (const char * path), (newpath), path)
JBPATH_SHIM_WRAP(int, lchown, (const char * path,uid_t uid, gid_t gid), (newpath,uid,gid), path)
JBPATH_SHIM_DEF(ssize_t, readlink, (const char * path, char* buf, size_t bufsiz))
JBPATH_SHIM_WRAP(int, truncate, (const char *path,off_t length), (newpath,length), path)

JBPATH_SHIM_DEF(int, link, (const char *name1, const char *name2))

JBPATH_SHIM_DEF(int, symlink, (const char *name1, const char *name2))

JBPATH_SHIM_WRAP(int, mknod, (const char *path, mode_t mode, dev_t dev), (newpath,mode,dev), path)

JBPATH_SHIM_DEF(char*, mkdtemp, (char * path))
JBPATH_SHIM_DEF(int, mkstemp, (char * path))
JBPATH_SHIM_DEF(int,mkstemps,(char *_template, int suffixlen))
//not really fs access char* mktemp(char * path)
JBPATH_SHIM_DEF(int, mkostemp, (char * path, int oflags))
JBPATH_SHIM_DEF(int, mkostemps, (char * path, int slen, int oflags))

//only /dev/xxx,not about jbroot/ JBPATH_SHIM_WRAP(int, revoke, (const char * path), (newpath), path)
JBPATH_SHIM_WRAP(int, undelete, (const char * path), (newpath), path)
//Undefined symbol: _unwhiteout?? doesn't exists in dsc, int     unwhiteout(const char *); //not in dsc?

JBPATH_SHIM_WRAP(int,getattrlist,(const char* path, struct attrlist * attrList, void * attrBuf,size_t attrBufSize, unsigned int options), (newpath,attrList,attrBuf,attrBufSize,options), path)

JBPATH_SHIM_WRAP(int,setattrlist,(const char* path, struct attrlist * attrList, void * attrBuf,
                                  size_t attrBufSize, unsigned int options), (newpath,attrList,attrBuf,attrBufSize,options), path)

JBPATH_SHIM_DEF(int, exchangedata, (const char * path1,const char * path2,unsigned int options))

JBPATH_SHIM_WRAP(int,searchfs,(const char * path, struct fssearchblock * searchBlock,unsigned long * numMatches, unsigned int scriptCode,unsigned int options, struct searchstate * state), (newpath,searchBlock,numMatches,scriptCode,options,state), path)

JBPATH_SHIM_WRAP(int,fsctl,(const char *path,unsigned long request,void*data,unsigned int options), (newpath,request,data,options), path)

JBPATH_SHIM_WRAP(int, sync_volume_np, (const char *path, int flags), (newpath,flags), path)


JBPATH_SHIM_WRAP(int, mkpath_np, (const char *path, mode_t omode), (newpath,omode), path)
JBPATHAT_SHIM_WRAP(int, mkpathat_np, (int dfd, const char *path, mode_t omode), (dfd, newpath, omode), dfd,path,0)

#ifdef TARGET_OS_IPHONE
JBPATH_SHIM_DEF(int, mkstemp_dprotected_np, (char *path, int dpclass, int dpflags))
#endif
JBPATHAT_SHIM_DEF(char*, mkdtempat_np, (int dfd, char *path))
JBPATHAT_SHIM_DEF(int, mkstempsat_np, (int dfd, char *path, int slen))
JBPATHAT_SHIM_DEF(int,mkostempsat_np,(int dfd,char*path,int slen,int oflags))


JBPATH_SHIM_WRAP(int, execv, (const char * __path, char * const * __argv), (newpath,__argv), __path)
JBPATH_SHIM_WRAP(int, execve, (const char * __file, char * const * __argv, char * const * __envp), (newpath,__argv,__envp), __file)

JBPATH_SHIM_DEF(int, execvP, (const char * __file, const char * __searchpath, char * const * __argv))

JBPATH_SHIM_DEF(int, execvp, (const char * __file, char * const * __argv))

JBPATH_SHIM_DEF(int, execl, (const char *path, const char *arg0, ...))

JBPATH_SHIM_DEF(int, execle, (const char *path, const char *arg0, ... /*, (char *)0, char *const envp[] */))

JBPATH_SHIM_DEF(int, execlp, (const char *file, const char *arg0, ...))


/* dirent.h */
JBPATH_SHIM_WRAP(DIR*, opendir, (const char *filename), (newpath), filename)
JBPATH_SHIM_WRAP(int, scandir, (const char *dirname, struct dirent ***namelist,
    int (*select)(const struct dirent *), int (*compar)(const struct dirent **, const struct dirent **)),
    (newpath,namelist,select,compar), (dirname));
JBPATH_SHIM_WRAP(int, scandir_b, (const char *dirname, struct dirent ***namelist,
    int (^select)(const struct dirent *), int (^compar)(const struct dirent **, const struct dirent **)),
    (newpath,namelist,select,compar), (dirname));
JBPATH_SHIM_WRAP(DIR *, __opendir2, (const char * name, int flags), (newpath, flags), name)
JBPATH_SHIM_DEF(struct dirent*, readdir, (DIR *))
JBPATH_SHIM_DEF(int, readdir_r, (DIR *, struct dirent *, struct dirent **))


/* utime.h */
JBPATH_SHIM_WRAP(int, utime, (const char *file, const struct utimbuf *timep),(newpath,timep), file)
/* time.h */
JBPATH_SHIM_WRAP(int, utimes, (const char *path, const struct timeval *times),(newpath,times), path)
JBPATH_SHIM_WRAP(int, lutimes, (const char *path, const struct timeval *times),(newpath,times), path)

/* stdio.h */
JBPATH_SHIM_WRAP(FILE*, fopen, (const char* filename, const char* mode), (newpath, mode), filename)
JBPATH_SHIM_WRAP(FILE*, freopen, (const char* filename, const char* mode, FILE* fp), (newpath, mode, fp), filename)
JBPATH_SHIM_WRAP(FILE*, fopen$DARWIN_EXTSN, (const char * __restrict __filename, const char * __restrict __mode), (newpath,__mode), __filename)
JBPATH_SHIM_WRAP(int, remove, (const char * path), (newpath), path)
JBPATH_SHIM_DEF(int, rename, (const char *__old, const char *__new))

//not really fs access char *tmpnam(char *);
//not really fs access char* tempnam,(const char *__dir, const char *__prefix);

//popen$DARWIN_EXTSN
//FILE* popen(const char *command, const char *type); //shim in sh instead of here ****************************************

/* sys/stat.h */
JBPATH_SHIM_WRAP(int, chmod, (const char *path, mode_t mode), (newpath,mode), path)
JBPATH_SHIM_DEF(int, lstat, (const char * path, struct stat *sb) )
JBPATH_SHIM_WRAP(int, mkdir, (const char * path, mode_t mode), (newpath,mode), path)
JBPATH_SHIM_WRAP(int, mkfifo, (const char *path, mode_t mode), (newpath,mode), path)
JBPATH_SHIM_WRAP(int, stat, (const char *path, struct stat *sb), (newpath,sb), path) //name confict with struct ********************
//already defined in unistd.h JBPATH_SHIM_WRAP(int, mknod, (const char *path, mode_t mode, dev_t dev), (newpath,mode,dev), path)

JBPATHAT_SHIM_WRAP(int, fchmodat, (int fd, const char *path, mode_t mode, int flag), (fd,newpath,mode,flag), fd, path, flag)
JBPATHAT_SHIM_DEF(int, fstatat, (int fd, const char *path, struct stat *sb, int flag) )
JBPATHAT_SHIM_WRAP(int, mkdirat, (int fd, const char * path, mode_t mode), (fd,newpath,mode), fd, path, 0)
//ios16 JBPATHAT_SHIM_WRAP(int, mkfifoat, (int fd, const char * path, mode_t mode), (fd,newpath,mode), fd, path, 0)
//ios16 JBPATHAT_SHIM_WRAP(int, mknodat, (int fd, const char *path, mode_t mode, dev_t dev), (fd,newpath,mode,dev), fd, path, 0)

JBPATHAT_SHIM_WRAP(int, utimensat, (int fd, const char *path, const struct timespec times[2], int flag), (fd,newpath,times,flag), fd,path,flag)

//file-cmds/chmod.c->modify_file_acl? but its from apple opensource probject, this shim library is just for gnu opensource project
JBPATH_SHIM_WRAP(int, chmodx_np, (const char *path, filesec_t fsec), (newpath,fsec), path)

JBPATH_SHIM_WRAP(int, lchflags, (const char *path, __uint32_t flags), (newpath,flags), path)
JBPATH_SHIM_WRAP(int, lchmod, (const char *path, mode_t mode), (newpath,mode), path)
JBPATH_SHIM_DEF(int, lstatx_np, (const char *path, struct stat *st, filesec_t fsec) )
JBPATH_SHIM_WRAP(int, mkdirx_np, (const char *path, filesec_t fsec), (newpath, fsec), path)
JBPATH_SHIM_WRAP(int, mkfifox_np, (const char *path, filesec_t fsec), (newpath, fsec), path)
JBPATH_SHIM_WRAP(int, statx_np, (const char *path, struct stat *st, filesec_t fsec), (newpath, st, fsec), path)
JBPATH_SHIM_WRAP(int, lstat64, (const char *path, struct stat64 *st), (newpath, st), path)
JBPATH_SHIM_WRAP(int, stat64, (const char *path, struct stat64 *st), (newpath, st), path)


/* stdlib.h */
//int system(const char *) __DARWIN_ALIAS_C(system); //shim in sh instead of here *******************************
JBPATH_SHIM_DEF(char*, realpath, (const char * path, char *resolved_path))
JBPATH_SHIM_DEF(char*, realpath$DARWIN_EXTSN, (const char * path, char *resolved_path))
//already defined in unistd.h char* mktemp(char *path)
//already defined in unistd.h int mkstemp(char *path)

/* sys/unistd.h */

JBPATHAT_SHIM_WRAP(int,getattrlistat,(int fd,const char* path, struct attrlist * attrList, void * attrBuf,size_t attrBufSize, unsigned int options), (fd,newpath,attrList,attrBuf,attrBufSize,options), fd,path,0)

JBPATHAT_SHIM_WRAP(int,setattrlistat,(int fd,const char* path, struct attrlist * attrList, void * attrBuf,
                                  size_t attrBufSize, unsigned int options), (fd,newpath,attrList,attrBuf,attrBufSize,options), fd,path,0)
//ios16 ssize_t freadlink(int, char * __restrict, size_t) __API_AVAILABLE(macos(13.0), ios(16.0), tvos(16.0), watchos(9.0));

JBPATHAT_SHIM_WRAP(int, faccessat, (int fd, const char *path, int mode, int flag), (fd,newpath,mode,flag), fd, path,flag)
JBPATHAT_SHIM_WRAP(int, fchownat, (int fd, const char *path, uid_t owner, gid_t group, int flag), (fd,newpath,owner,group,flag), fd, path,flag)
JBPATHAT_SHIM_DEF(ssize_t, readlinkat, (int fd,const char* path,char* buf,size_t bufsize))
JBPATHAT_SHIM_WRAP(int, unlinkat, (int dfd, const char *path, int flag), (dfd,newpath,flag), dfd, path,flag)

JBPATHAT_SHIM_DEF(int, linkat, (int fd1, const char *name1, int fd2, const char *name2, int flag))

JBPATHAT_SHIM_DEF(int, symlinkat, (const char *name1, int fd, const char *name2))


/* mount.h */
JBPATH_SHIM_WRAP(int, mount, (const char *type, const char *dir, int flags, void *data), (type,newpath,flags,data), dir)
//??int fmount, (const char *type, int fd, int flags, void *data);
JBPATH_SHIM_WRAP(int, statfs, (const char * path, struct statfs *buf), (newpath, buf), path) //name confict with struct********************
JBPATH_SHIM_WRAP(int, statfs64, (const char * path, struct statfs64 *buf), (newpath, buf), path)
JBPATH_SHIM_WRAP(int, unmount, (const char *dir, int flags), (newpath,flags), dir)
//not a path/int getvfsbyname(const char *fsname, struct vfsconf *vfcp);

/* mman.h */
JBPATH_SHIM_DEF(int, shm_open, (const char * path, int flags, ...))

JBPATH_SHIM_WRAP(int, shm_unlink, (const char * path), (newpath), path)

/* semaphore */
JBPATH_SHIM_DEF(sem_t*, sem_open, (const char *path, int flags, ...))

JBPATH_SHIM_WRAP(int, sem_unlink, (const char * path), (newpath), path)

/* copyfile.h */
JBPATH_SHIM_DEF(int, copyfile, (const char * from, const char * to, copyfile_state_t state, copyfile_flags_t flags))

/* sys/stdio.h */
JBPATHAT_SHIM_DEF(int, renameat, (int fromfd, const char *from, int tofd, const char *to))

JBPATH_SHIM_DEF(int, renamex_np, (const char *__old, const char *__new, unsigned int flags))

JBPATHAT_SHIM_DEF(int, renameatx_np, (int fromfd, const char *from, int tofd, const char *to, unsigned int flags))

/* db.h */
JBPATH_SHIM_WRAP(DB*, dbopen, (const char *file,int flags,int mode,DBTYPE type,const void *openinfo),(newpath,flags,mode,type,openinfo),file)
//__DBINTERFACE_PRIVATE
JBPATH_SHIM_WRAP(DB*,__bt_open,(const char *file,int flags,int mode,const BTREEINFO *openinfo,int dflags),(newpath,flags,mode,openinfo,dflags),file)
JBPATH_SHIM_WRAP(DB*,__hash_open,(const char *file, int flags, int mode, const HASHINFO *info, int dflags),(newpath,flags,mode,info,dflags),file)
JBPATH_SHIM_WRAP(DB*,__rec_open,(const char *file, int flags, int mode, const RECNOINFO *info, int dflags),(newpath,flags,mode,info,dflags),file)

/* sys/statvfs.h */
JBPATH_SHIM_WRAP(int, statvfs,(const char * path, struct statvfs * st), (newpath,st), path)


/* c++ fstream */

//filebuf::open(char*,int)
JBPATH_SHIM_WRAP(void*, _ZNSt3__113basic_filebufIcNS_11char_traitsIcEEE4openEPKcj,(void* thiz,const char* __s,unsigned int mode),(thiz,newpath,mode), __s)
//ifstream::open(char*,int)
JBPATH_SHIM_WRAP(void*, _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj,(void* thiz,const char* __s,unsigned int mode),(thiz,newpath,mode), __s)
//ifstream::open(std::string,int)
JBPATH_SHIM_DEF(void*, _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj,(void* thiz,void* __s,unsigned int mode))
//ofstream::open(char*,int)
JBPATH_SHIM_WRAP(void*, _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj,(void* thiz,const char* __s,unsigned int mode),(thiz,newpath,mode), __s)
//ofstream::open(std::string,int)
JBPATH_SHIM_DEF(void*, _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj,(void* thiz,void* __s,unsigned int mode))

/* sys/socket.h */
JBPATH_SHIM_DEF(int, bind, (int sockfd, const struct sockaddr *addr, socklen_t addrlen))
JBPATH_SHIM_DEF(int, connect, (int sockfd, const struct sockaddr *addr, socklen_t addrlen))
JBPATH_SHIM_DEF(int, getpeername,(int sockfd, struct sockaddr *addr, socklen_t* addrlen))
JBPATH_SHIM_DEF(int, getsockname, (int sockfd, struct sockaddr *addr, socklen_t* addrlen))


/* ftw.h */
JBPATH_SHIM_DEF(int, ftw, (const char *, int (*)(const char *, const struct stat *, int), int) )
JBPATH_SHIM_DEF(int, nftw, (const char *, int (*)(const char *, const struct stat *, int, struct FTW *), int, int) )

/* fts.h */
JBPATH_SHIM_DEF(FTS*, fts_open, (char * const *, int, int (*)(const FTSENT **, const FTSENT **) ) )
JBPATH_SHIM_DEF(FTS*, fts_open_b, (char * const *, int, int (^)(const FTSENT **, const FTSENT **) ) )

/* sys/xattr.h */
JBPATH_SHIM_WRAP(ssize_t, getxattr, (const char *path, const char *name, void *value, size_t size, u_int32_t position, int options), (newpath,name,value,size,position,options), path)
JBPATH_SHIM_WRAP(int, setxattr, (const char *path, const char *name, const void *value, size_t size, u_int32_t position, int options), (newpath,name,value,size,position,options), path)
JBPATH_SHIM_WRAP(int, removexattr, (const char *path, const char *name, int options), (newpath,name,options), name)
JBPATH_SHIM_WRAP(ssize_t, listxattr, (const char *path, char *namebuff, size_t size, int options), (newpath,namebuff,size,options), path)

/* glob.h */
JBPATH_SHIM_DEF(int, glob, (const char * pattern, int flags, int (* errfunc) (const char *, int), glob_t * pglob))
JBPATH_SHIM_DEF(int, glob_b, (const char * pattern, int flags, int (^ errfunc) (const char *, int), glob_t * pglob))

/* sysdir.h */
//not really fs access, sysdir_search_path_enumeration_state sysdir_get_next_search_path_enumeration(sysdir_search_path_enumeration_state state, char *path);


#ifndef JBPATH_SHIM_INTERNAL
MACRO_endif /* JBPATH_SHIM_H */
#endif

#endif /* JBPATH_SHIM_INTERNAL_H */
