# 1 "jbpath-shim/jbpath_shim.h"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 400 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "jbpath-shim/jbpath_shim.h" 2
# 27 "jbpath-shim/jbpath_shim.h"
 #ifndef JBPATH_SHIM_H
 #define JBPATH_SHIM_H




 #define dlopen jbpath_shim_dlopen

 #define posix_spawn jbpath_shim_posix_spawn;






 #define open jbpath_shim_open

 #define openat jbpathat_shim_openat

 #define creat jbpath_shim_creat

 #define openx_np jbpath_shim_openx_np






 #define access jbpath_shim_access
 #define chdir jbpath_shim_chdir
 #define chown jbpath_shim_chown

 #define pathconf jbpath_shim_pathconf
 #define rmdir jbpath_shim_rmdir
 #define unlink jbpath_shim_unlink
 #define chroot jbpath_shim_chroot
 #define lchown jbpath_shim_lchown
 #define readlink jbpath_shim_readlink
 #define truncate jbpath_shim_truncate

 #define link jbpath_shim_link

 #define symlink jbpath_shim_symlink

 #define mknod jbpath_shim_mknod

 #define mkdtemp jbpath_shim_mkdtemp
 #define mkstemp jbpath_shim_mkstemp
 #define mkstemps jbpath_shim_mkstemps

 #define mkostemp jbpath_shim_mkostemp
 #define mkostemps jbpath_shim_mkostemps


 #define undelete jbpath_shim_undelete


 #define getattrlist jbpath_shim_getattrlist

 #define setattrlist jbpath_shim_setattrlist


 #define exchangedata jbpath_shim_exchangedata

 #define searchfs jbpath_shim_searchfs

 #define fsctl jbpath_shim_fsctl

 #define sync_volume_np jbpath_shim_sync_volume_np


 #define mkpath_np jbpath_shim_mkpath_np
 #define mkpathat_np jbpathat_shim_mkpathat_np




 #define mkdtempat_np jbpathat_shim_mkdtempat_np
 #define mkstempsat_np jbpathat_shim_mkstempsat_np
 #define mkostempsat_np jbpathat_shim_mkostempsat_np


 #define execv jbpath_shim_execv
 #define execve jbpath_shim_execve

 #define execvP jbpath_shim_execvP

 #define execvp jbpath_shim_execvp

 #define execl jbpath_shim_execl

 #define execle jbpath_shim_execle

 #define execlp jbpath_shim_execlp



 #define opendir jbpath_shim_opendir
 #define scandir jbpath_shim_scandir;


 #define scandir_b jbpath_shim_scandir_b;





 #define utime jbpath_shim_utime

 #define utimes jbpath_shim_utimes
 #define lutimes jbpath_shim_lutimes


 #define fopen jbpath_shim_fopen
 #define fopen$DARWIN_EXTSN jbpath_shim_fopen$DARWIN_EXTSN
 #define remove jbpath_shim_remove
 #define rename jbpath_shim_rename
# 152 "jbpath-shim/jbpath_shim.h"
 #define chmod jbpath_shim_chmod
 #define lstat jbpath_shim_lstat
 #define mkdir jbpath_shim_mkdir
 #define mkfifo jbpath_shim_mkfifo
 #define stat jbpath_shim_stat


 #define fchmodat jbpathat_shim_fchmodat
 #define fstatat jbpathat_shim_fstatat
 #define mkdirat jbpathat_shim_mkdirat



 #define utimensat jbpathat_shim_utimensat


 #define chmodx_np jbpath_shim_chmodx_np

 #define lchflags jbpath_shim_lchflags
 #define lchmod jbpath_shim_lchmod
 #define lstatx_np jbpath_shim_lstatx_np
 #define mkdirx_np jbpath_shim_mkdirx_np
 #define mkfifox_np jbpath_shim_mkfifox_np
 #define statx_np jbpath_shim_statx_np
# 184 "jbpath-shim/jbpath_shim.h"
 #define realpath jbpath_shim_realpath
 #define realpath$DARWIN_EXTSN jbpath_shim_realpath$DARWIN_EXTSN




int getattrlistat(int, const char *, void *, void *, size_t, unsigned long) __OSX_AVAILABLE_STARTING(__MAC_10_10, __IPHONE_8_0);
int setattrlistat(int, const char *, void *, void *, size_t, uint32_t) __OSX_AVAILABLE(10.13) __IOS_AVAILABLE(11.0) __TVOS_AVAILABLE(11.0) __WATCHOS_AVAILABLE(4.0);
ssize_t freadlink(int, char * __restrict, size_t) __API_AVAILABLE(macos(13.0), ios(16.0), tvos(16.0), watchos(9.0));

 #define faccessat jbpathat_shim_faccessat
 #define fchownat jbpathat_shim_fchownat
 #define readlinkat jbpathat_shim_readlinkat
 #define unlinkat jbpathat_shim_unlinkat

 #define linkat jbpathat_shim_linkat

 #define symlinkat jbpathat_shim_symlinkat



 #define mount jbpath_shim_mount

 #define statfs jbpath_shim_statfs

 #define unmount jbpath_shim_unmount



 #define shm_open jbpath_shim_shm_open

 #define shm_unlink jbpath_shim_shm_unlink


 #define sem_open jbpath_shim_sem_open

 #define sem_unlink jbpath_shim_sem_unlink


 #define copyfile jbpath_shim_copyfile


 #define renameat jbpathat_shim_renameat

 #define renamex_np jbpath_shim_renamex_np

 #define renameatx_np jbpathat_shim_renameatx_np


 #define dbopen jbpath_shim_dbopen

 #define __bt_open jbpath_shim___bt_open
 #define __hash_open jbpath_shim___hash_open
 #define __rec_open jbpath_shim___rec_open


 #define statvfs jbpath_shim_statvfs



 #define _ZNSt3__113basic_filebufIcNS_11char_traitsIcEEE4openEPKcj jbpath_shim__ZNSt3__113basic_filebufIcNS_11char_traitsIcEEE4openEPKcj

 #define _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj jbpath_shim__ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openEPKcj

 #define _ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj jbpath_shim__ZNSt3__114basic_ifstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj

 #define _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj jbpath_shim__ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openEPKcj

 #define _ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj jbpath_shim__ZNSt3__114basic_ofstreamIcNS_11char_traitsIcEEE4openERKNS_12basic_stringIcS2_NS_9allocatorIcEEEEj


 #endif
