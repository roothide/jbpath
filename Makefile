
ARCHS = arm64
TARGET := iphone:clang:latest:15.0
#THEOS_PACKAGE_SCHEME=rootless

THEOS_PLATFORM_DEB_COMPRESSION_TYPE = gzip

THEOS_DEVICE_IP = iphone11.local

include $(THEOS)/makefiles/common.mk


LIBRARY_NAME = libjbpath libjbpathapis

libjbpath_FILES = libjbpath/libjbpath/jbroot.c libjbpath/libjbpath/jbroot.cpp libjbpath/libjbpath/jbroot.m libjbpath/libjbpath/cache.cpp
libjbpath_LDFLAGS += -install_name @loader_path/.jbroot/usr/lib/libjbpath.dylib
libjbpath_INSTALL_PATH = /usr/lib

libjbpathapis_FILES = jbpath-shim/jbpath-shim/jbpath_shim.c jbpath-shim/jbpath-shim/jbpath_mktemp.c jbpath-shim/jbpath-shim/jbpath_shim.cpp jbpath-shim/jbpath-shim/jbpath_rootfs.c
libjbpathapis_CFLAGS += -I./
libjbpathapis_LDFLAGS += -install_name @loader_path/.jbroot/usr/lib/libjbpathapis.dylib -L$(THEOS_OBJ_DIR) -ljbpath
libjbpathapis_INSTALL_PATH = /usr/lib

include $(THEOS_MAKE_PATH)/library.mk


TOOL_NAME = updatelink

updatelink_FILES = jbpath-updatelink/updatelink/main.c
updatelink_CFLAGS += -I./
updatelink_LDFLAGS += -L$(THEOS_OBJ_DIR) -ljbpath
updatelink_INSTALL_PATH = /usr/sbin
updatelink_CODESIGN_FLAGS = -Sentitlements.plist


include $(THEOS_MAKE_PATH)/tool.mk


after-libjbpathapis-all::
	./jbpathapis-redirect $(THEOS_OBJ_DIR)/libjbpathapis.dylib
	ldid -S $(THEOS_OBJ_DIR)/libjbpathapis.dylib
	clang -E jbpath-shim/jbpath-shim/jbpath_shim.h > jbpath-shim/jbpath-shim/libjbpath_shim.h

clean::
	rm -rf ./packages/*