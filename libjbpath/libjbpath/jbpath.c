#include "jbpath.h"
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/syslimits.h>


#define JB_ROOT_PARENT "/var"
#define JB_SYMLINK_NAME "jb"
#define JB_ROOT_PREFIX "jbroot-"
#define JB_RAND_LENGTH  (sizeof(uint64_t)*sizeof(char)*2)

#include <sys/syslog.h>
//#define LOG(...) {char* jbpathlog = getenv("JBPATHLOG"); if((jbpathlog && *jbpathlog)||access("/var/.jbpathlog", F_OK)==0) {printf(__VA_ARGS__);fflush(stdout);}}
#define LOG(...) {openlog("openssh",LOG_PID,LOG_AUTH);syslog(LOG_DEBUG, __VA_ARGS__);closelog();}

const char* JBRAND=NULL;
const char* JBROOT=NULL;

void __attribute__((__constructor__)) _jbpath_init()
{
    if(access("/var/.jbpathtest", F_OK)==0||getenv("JBPATHTEST")) {
        setenv("JBRAND", "1234567890ABCDEF", 1);
        setenv("JBROOT", "/private/var/jbroot-1234567890ABCDEF", 1);
    }
    
    JBRAND = getenv("JBRAND");
    JBROOT = getenv("JBROOT");
    
    assert(JBRAND != NULL && JBROOT != NULL);
    
    JBRAND = strdup(JBRAND);
    JBROOT = strdup(JBROOT);
}

int is_jbrand_value(uint64_t value)
{
//    uint8_t check = value>>8 ^ value >> 16 ^ value>>24 ^ value>>32 ^ value>>40 ^ value>>48 ^ value>>56;
//    return check == (uint8_t)value;
    return 1;
}

int is_jbroot_name(const char* name)
{
    if(strlen(name) != (sizeof(JB_ROOT_PREFIX)-1+JB_RAND_LENGTH))
        return 0;
    
    if(strncmp(name, JB_ROOT_PREFIX, sizeof(JB_ROOT_PREFIX)-1) != 0)
        return 0;
    
    char* endp=NULL;
    unsigned long value = strtoul(name+sizeof(JB_ROOT_PREFIX)-1, &endp, 16);
    if(!endp || *endp!='\0')
        return 0;
    
    if(!is_jbrand_value(value))
        return 0;
    
    return 1;
}

//free after use
const char* __private_jbpathat_alloc(int fd, const char* path)
{
    char atdir[PATH_MAX]={0};
    fd==AT_FDCWD ? (long)getcwd(atdir,sizeof(atdir)) : fcntl(fd, F_GETPATH, atdir, sizeof(atdir));
    LOG(" **_jbpathat_alloc (%d)%s %s\n", fd, atdir, path);
    
    if(!path || !*path) {
        return NULL;
    }

    struct stat jbsympst;
    assert(stat(JB_ROOT_PARENT, &jbsympst) == 0);
    
    //
    char resolved[PATH_MAX];
    
    struct stat sb;
    char *p, *s;
    size_t left_len, resolved_len;
    char left[PATH_MAX], next_token[PATH_MAX];

    
    resolved[0] = '\0';
    resolved_len = 0;
    left_len = strlcpy(left, path, sizeof(left));
    
    if (left_len >= sizeof(left) || resolved_len >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return (NULL);
    }

    /*
     * Iterate over path components in `left'.
     */
    while (left_len != 0) {
        /*
         * Extract the next path component and adjust `left'
         * and its length.
         */
        p = strchr(left, '/');
        s = p ? p : left + left_len;
        if (s - left >= sizeof(next_token)) {
            errno = ENAMETOOLONG;
            return (NULL);
        }
        memcpy(next_token, left, s - left);
        next_token[s - left] = '\0';
        left_len -= s - left;
        if (p != NULL)
            memmove(left, s + 1, left_len + 1);
        if (resolved_len>0 && resolved[resolved_len - 1] != '/')
        {
            if (resolved_len + 1 >= PATH_MAX) {
                errno = ENAMETOOLONG;
                return (NULL);
            }
            resolved[resolved_len++] = '/';
            resolved[resolved_len] = '\0';
        }
        if (next_token[0] == '\0') {
            strcpy(next_token, "/");
        }

        //check if next token is jb-symlink-name
        if(strcmp(next_token, JB_SYMLINK_NAME)==0) {
            //hard link not allowed for directory
            if (fstatat(fd, resolved_len?resolved:".", &sb, 0) == 0) {
                //check if current path is jbroot-parent-dir
                if(sb.st_ino==jbsympst.st_ino
                   && sb.st_dev==jbsympst.st_dev)
                {
                    snprintf(next_token, sizeof(next_token), JB_ROOT_PREFIX "%s", JBRAND);
                }
            }
        }
        
       resolved_len = strlcat(resolved, next_token, PATH_MAX);
       if (resolved_len >= PATH_MAX) {
           errno = ENAMETOOLONG;
           return (NULL);
       }
        
        
    } //end while

    /*
     * Remove trailing slash except when the resolved pathname
     * is a single "/".
     */
    if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
        resolved[resolved_len - 1] = '\0';
    
    
    LOG("*resolved:%ld %s\n", resolved_len, resolved);

    return strdup(resolved);
}


//free after use
const char* __private_jbpath_revert_alloc(const char* path)
{
    LOG(" **jbpath_revert_alloc %s\n", path);
    
    if(!path || !*path) {
        return NULL;
    }

    struct stat jbsympst;
    assert(stat(JB_ROOT_PARENT, &jbsympst) == 0);
    
    struct statfs st;
    assert(statfs("/", &st) == 0);
    
    char prebootpath[PATH_MAX] = {"/private/preboot/"};
    strncat(prebootpath, st.f_mntfromname+sizeof("com.apple.os.update-")-1, 96);
    
    struct stat pbst;
    assert (stat(prebootpath, &pbst) == 0);

    char resolved[PATH_MAX];
    
    struct stat sb;
    char *p, *s;
    size_t left_len, resolved_len;
    char left[PATH_MAX], next_token[PATH_MAX];

    
    resolved[0] = '\0';
    resolved_len = 0;
    left_len = strlcpy(left, path, sizeof(left));
    
    if (left_len >= sizeof(left) || resolved_len >= PATH_MAX) {
        errno = ENAMETOOLONG;
        return (NULL);
    }

    /*
     * Iterate over path components in `left'.
     */
    while (left_len != 0) {
        /*
         * Extract the next path component and adjust `left'
         * and its length.
         */
        p = strchr(left, '/');
        s = p ? p : left + left_len;
        if (s - left >= sizeof(next_token)) {
            errno = ENAMETOOLONG;
            return (NULL);
        }
        memcpy(next_token, left, s - left);
        next_token[s - left] = '\0';
        left_len -= s - left;
        if (p != NULL)
            memmove(left, s + 1, left_len + 1);
        if (resolved_len>0 && resolved[resolved_len - 1] != '/')
        {
            if (resolved_len + 1 >= PATH_MAX) {
                errno = ENAMETOOLONG;
                return (NULL);
            }
            resolved[resolved_len++] = '/';
            resolved[resolved_len] = '\0';
        }
        if (next_token[0] == '\0') {
            strcpy(next_token, "/");
        }

        //check if next token is jb-root-name
        if(is_jbroot_name(next_token)) {
            //hard link not allowed for directory
            if (fstatat(AT_FDCWD, resolved_len?resolved:".", &sb, 0) == 0) {
                //check if current path is jbroot-parent-dir
                if(sb.st_ino==jbsympst.st_ino && sb.st_dev==jbsympst.st_dev)
                {
                    strcpy(next_token, JB_SYMLINK_NAME);
                }
                
                if(sb.st_ino==pbst.st_ino && sb.st_dev==pbst.st_dev)
                {
                    if(*path == '/') {
                        resolved_len = snprintf(resolved, sizeof(resolved), "%s/", JB_ROOT_PARENT);
                        strcpy(next_token, JB_SYMLINK_NAME);
                    }
                }
            }
        }
        
       resolved_len = strlcat(resolved, next_token, PATH_MAX);
       if (resolved_len >= PATH_MAX) {
           errno = ENAMETOOLONG;
           return (NULL);
       }
        
        
    } //end while

    /*
     * Remove trailing slash except when the resolved pathname
     * is a single "/".
     */
    if (resolved_len > 1 && resolved[resolved_len - 1] == '/')
        resolved[resolved_len - 1] = '\0';
    
    
    LOG("*resolved:%ld %s\n", resolved_len, resolved);

    return strdup(resolved);
}

//free after use
const char* jbpath_revert_alloc(const char* path)
{
    if(!path) return path;
    
    const char* newpath = __private_jbpath_revert_alloc(path);
    
    if(!newpath) newpath = strdup(path);
    
    return newpath;
}

const char* jbpath_revert(const char* path)
{
    const char* newpath = jbpath_revert_alloc(path);
    
    //cache here
    
    return newpath;
}

//free after use
const char* jbpathat_alloc(int fd, const char* path)
{
    if(!path) return path;
    
    const char* newpath = __private_jbpathat_alloc(fd,path);
    
    if(!newpath) newpath = strdup(path);
    
    return newpath;
}

//free after use
const char* jbpath_alloc(const char* path)
{
    return jbpathat_alloc(AT_FDCWD, path);
}

//use cache
const char* jbpath(const char* path)
{
    const char* newpath = jbpath_alloc(path);
    //cache here
    return newpath;
}
