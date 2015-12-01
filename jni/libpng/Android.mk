LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libpng
LOCAL_LDFLAGS := -lz

LOCAL_SRC_FILES := \
	source/png.c \
	source/pngerror.c \
	source/pngget.c \
	source/pngmem.c \
	source/pngpread.c \
	source/pngread.c \
	source/pngrio.c \
	source/pngrtran.c \
	source/pngrutil.c \
	source/pngset.c \
	source/pngtrans.c \
	source/pngwio.c \
	source/pngwrite.c \
	source/pngwtran.c \
	source/pngwutil.c

include $(BUILD_SHARED_LIBRARY)
