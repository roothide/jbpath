
ARCHS = arm64
TARGET := iphone:clang:latest:15.0
THEOS_PACKAGE_SCHEME=rootless

THEOS_PLATFORM_DEB_COMPRESSION_TYPE = gzip

THEOS_DEVICE_IP = iphone11.local

include $(THEOS)/makefiles/common.mk


LIBRARY_NAME = libjbpath libjbpathapis

libjbpath_FILES = libjbpath/libjbpath/jbpath.c libjbpath/libjbpath/jbpath.cpp libjbpath/libjbpath/jbpath.m libjbpath/libjbpath/cache.cpp
libjbpath_LDFLAGS += -install_name @loader_path/.jbroot/usr/lib/libjbpath.dylib
libjbpath_INSTALL_PATH = /var/jb/usr/lib

libjbpathapis_FILES = jbpath-shim/jbpath-shim/jbpath_shim.c jbpath-shim/jbpath-shim/jbpath_shim.cpp
libjbpathapis_LDFLAGS += -install_name @loader_path/.jbroot/usr/lib/libjbpathapis.dylib -L$(THEOS_OBJ_DIR) -ljbpath
libjbpathapis_INSTALL_PATH = /var/jb/usr/lib

include $(THEOS_MAKE_PATH)/library.mk


TOOL_NAME = updatelink

updatelink_FILES = jbpath-updatelink/updatelink/main.c
updatelink_LDFLAGS += -L$(THEOS_OBJ_DIR) -ljbpath
updatelink_INSTALL_PATH = /var/jb/usr/sbin

include $(THEOS_MAKE_PATH)/tool.mk


after-libjbpathapis-all::
	./jbpathapis-redirect $(THEOS_OBJ_DIR)/libjbpathapis.dylib
	ldid -S $(THEOS_OBJ_DIR)/libjbpathapis.dylib

