#include "jbroot.h"
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
#define JB_ROOT_PREFIX "jbroot-"
#define JB_RAND_LENGTH  (sizeof(uint64_t)*sizeof(char)*2)

#define ROOTFS_PREFIX   "/rootfs"

#include <sys/syslog.h>
//#define LOG(...) {openlog("openssh",LOG_PID,LOG_AUTH);syslog(LOG_DEBUG, __VA_ARGS__);closelog();}
#define LOG(...) {char* jbpathlog = getenv("JBROOTLOG"); if((jbpathlog && *jbpathlog)||access("/var/.jbrootlog", F_OK)==0) {printf(__VA_ARGS__);fflush(stdout);}}


static const char* JBRAND=NULL;
static const char* JBROOT=NULL;

static void __attribute__((__constructor__)) _jbroot_init()
{
    if(access("/var/.jbroottest", F_OK)==0||getenv("JBROOTTEST"))
    {
        setenv("JBRAND", "1234567890ABCDEF", 1);
        //setenv("JBROOT", "/private/var/jbroot-1234567890ABCDEF", 1);
        // use /var/ instead of /private/var/ because stringByResolvingSymlinksInPath won't resolve /var/
        setenv("JBROOT", "/var/jbroot-1234567890ABCDEF", 1);
    }
    
    JBRAND = getenv("JBRAND");
    JBROOT = getenv("JBROOT");
    
    assert(JBRAND != NULL && JBROOT != NULL);
    
    JBRAND = strdup(JBRAND);
    JBROOT = strdup(JBROOT);
    
    if(getppid()==1)
    {
        char pwd[PATH_MAX];
        assert(getcwd(pwd, sizeof(pwd)) != NULL);
        if(strcmp(pwd, "/")==0)
            assert(chdir(JBROOT)==0);
    }
}


static int is_jbrand_value(uint64_t value)
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
static const char* __private_jbrootat_alloc(int fd, const char* path)
{
    char atdir[PATH_MAX]={0};
    fd==AT_FDCWD ? (long)getcwd(atdir,sizeof(atdir)) : fcntl(fd, F_GETPATH, atdir, sizeof(atdir));
    LOG(" **jbrootat_alloc (%d)%s %s\n", fd, atdir, path);
    
    if(!path || !*path) {
        return NULL;
    }

    struct stat jbrootst;
    assert(stat(JBROOT, &jbrootst) == 0);
    
    struct stat rootst;
    assert(stat("/", &rootst) == 0);
    
    
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

        //check if next token is ..
        if(strcmp(next_token, "..")==0)
        {
            //jbroot is always readable when jailbroken
            int jbrootfd = open(JBROOT, O_RDONLY);
            assert(jbrootfd >= 0);
            
            int checkfd = fd;
            char* checkpath = resolved;
            
            if(path[0] == '/') {
                checkfd = jbrootfd;
                while(*checkpath == '/') checkpath++;
            }
            if(*checkpath=='\0') checkpath = ".";
            //hard link not allowed for directory
            if (fstatat(checkfd, checkpath, &sb, 0) == 0) {
                //check if current path is jbroot-dir
                if(sb.st_ino==jbrootst.st_ino
                   && sb.st_dev==jbrootst.st_dev)
                {
                    snprintf(next_token, sizeof(next_token), ".");
                }
                else if(sb.st_dev==rootst.st_dev
                       && sb.st_ino==rootst.st_ino)
                { /* jbroot:/rootfs/../ => abspath jbroot:/ */
                    resolved[0] = '\0';
                    resolved_len = 0;
                    strcpy(next_token, "/");
                }
            }
            
            close(jbrootfd);
            
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
const char* jbrootat_alloc(int fd, const char* path)
{
    if(!path) return NULL;
    
    const char* fixedpath = __private_jbrootat_alloc(fd, path);
    
    if(!fixedpath) return strdup(path);
    
    // empty or relative path?
    if(fixedpath[0] != '/') return fixedpath;
    
    size_t pathlen = strlen(JBROOT) + strlen(fixedpath) + 1;
    char* newpath = malloc(pathlen);
    strcpy(newpath, JBROOT);
    strlcat(newpath, fixedpath, pathlen);
    
    free((void*)fixedpath);
    
    return newpath;
}

//free after use
const char* jbroot_alloc(const char* path)
{
    return jbrootat_alloc(AT_FDCWD, path);
}

//free after use
static const char* __private_jbroot_revert_alloc(const char* path)
{
    LOG(" **jbroot_revert_alloc %s\n", path);
    
    if(!path || !*path) {
        return NULL;
    }

    struct stat jbsympst;
    assert(stat(JB_ROOT_PARENT, &jbsympst) == 0);
    
    int jbroot_based = 0;


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
                    jbroot_based = 1;
                    
                    resolved[0] = '\0';
                    resolved_len = 0;
                    strcpy(next_token, "/");
                    
                    //continue resolve sub path?
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
    
    char* retval = NULL;
    if(jbroot_based==0 && path[0] == '/') { //revert a path out of jbroot?
        assert(resolved[0] == '/');
        retval = malloc(sizeof(ROOTFS_PREFIX)-1 + strlen(resolved) + 1);
        strcpy(retval, ROOTFS_PREFIX);//just add ROOTFS_PREFIX prefix
        strcat(retval, resolved);
    } else {
        retval = strdup(resolved);
    }
    
    LOG("*resolved:%ld %s\n", resolved_len, retval);

    return retval;
}


//use cache
const char* jbroot(const char* path)
{
    const char* newpath = jbroot_alloc(path);
    //cache here
    return newpath;
}

/* free after use */
const char* jbroot_revert_alloc(const char* path)
{
    if(!path) return path;
    
    const char* newpath = __private_jbroot_revert_alloc(path);
    
    if(!newpath) newpath = strdup(path);
    
    return newpath;
}

//use cache
const char* jbroot_revert(const char* path)
{
    const char* newpath = jbroot_revert_alloc(path);
    
    //cache here
    
    return newpath;
}
