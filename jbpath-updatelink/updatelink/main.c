#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/syslimits.h>
#include "jbpath.h"

#define JBROOT_SYMLINK_NAME ".jbroot"
#define JBROOT_SYMLINK_PATH "/var/jb"

int main(int argc, const char * argv[]) {
    
    struct stat jbsymst;
    assert(lstat(jbpath(JBROOT_SYMLINK_PATH), &jbsymst) == 0);
    
    char path[PATH_MAX];
    while(fgets(path, sizeof(path), stdin)) {
        size_t len = strlen(path);
        if(len && path[len-1]=='\n') path[len-1]='\0';
        
        struct stat st;
        assert(lstat(jbpath(path), &st) == 0);
        
        //don't change jbroot symlink
        if(st.st_dev==jbsymst.st_dev && st.st_ino==jbsymst.st_ino) {
            printf("jbroot symlink!\n");
            continue;
        }
        
        char slink[PATH_MAX]={0}; //readlink not padding with \0
        assert(readlink(jbpath(path), slink, sizeof(slink)-1)>0);
        
        printf("%s: %s\n", path, slink);
        
        if(strncmp(slink, JBROOT_SYMLINK_NAME, sizeof(JBROOT_SYMLINK_NAME)-1)==0
           && (slink[sizeof(JBROOT_SYMLINK_NAME)-1]=='/' || slink[sizeof(JBROOT_SYMLINK_NAME)-1]=='\0'))
        {
            char abspath[PATH_MAX];
            snprintf(abspath, sizeof(abspath), JBROOT_SYMLINK_PATH "%s", &slink[sizeof(JBROOT_SYMLINK_NAME)-1]);
            
            const char* newpath = jbpath(abspath);
            
//            assert(unlink(jbpath(path)) == 0);
//            assert(symlink(newpath, jbpath(path)) == 0);
            
            printf(".jbroot link => %s\n", newpath);
            
        }
        else if(slink[0] == '/')
        {
            const char* newpath = jbpath_revert(slink);
            if(strcmp(newpath, slink)==0) {
                printf("not in jbroot\n");
                continue;
            }
            
            newpath = jbpath(newpath);
            if(strcmp(newpath, slink)==0) {
                printf("no change\n");
                continue;
            }
            
            assert(unlink(jbpath(path)) == 0);
            assert(symlink(newpath, jbpath(path)) == 0);
            
            printf("=> %s\n", newpath);
        }
        else
        {
            printf("relative path link\n");
        }
        
    }
    
    return 0;
}
