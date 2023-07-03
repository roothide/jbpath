#ifndef JBPATH_H
#define JBPATH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#include <string>
#endif

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

const char* jbpath_alloc(const char* path); /* free after use */
const char* jbpathat_alloc(int fd, const char* path); /* free after use */
const char* jbpath_revert_alloc(const char* path);  /* free after use */

const char* rootfs_alloc(const char* path);  /* free after use */
const char* jbroot_alloc(const char* path); /* free after use */

/**/

const char* jbpath(const char* path); /* use cache in c */
const char* jbpath_revert(const char* path);  /* use cache in c */

const char* rootfs(const char* path); /* add /rootfs/ prefix */
const char* rootfs_revert(const char* path); /* strip /rootfs/ prefix */
const char* jbroot(const char* path); /* add jbroot path */
const char* jbroot_revert(const char* path); /* strip jbroot path */

#ifdef __OBJC__
NSString* __attribute__((overloadable)) jbpath(NSString* path);
NSString* __attribute__((overloadable)) jbpath_revert(NSString* path);

NSString* __attribute__((overloadable)) rootfs(NSString* path);
NSString* __attribute__((overloadable)) rootfs_revert(NSString* path);
NSString* __attribute__((overloadable)) jbroot(NSString* path);
NSString* __attribute__((overloadable)) jbroot_revert(NSString* path);
#endif

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
std::string jbpath(std::string path);
std::string jbpath_revert(std::string path);

std::string rootfs(std::string path);
std::string rootfs_revert(std::string path);
std::string jbroot(std::string path);
std::string jbroot_revert(std::string path);
#endif

#endif /* JBPATH_H */
