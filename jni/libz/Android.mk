LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libz

LOCAL_SRC_FILES := \
	source/adler32.c \
	source/crc32.c \
	source/deflate.c \
	source/infback.c \
	source/inffast.c \
	source/inflate.c \
	source/inftrees.c \
	source/trees.c \
	source/zutil.c \
	source/compress.c \
	source/uncompr.c \
	source/gzclose.c \
	source/gzlib.c \
	source/gzread.c \
	source/gzwrite.c

include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)
