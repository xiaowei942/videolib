# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := libyuv2rgb
LOCAL_PRELINK_MODULE := false

LOCAL_ARM_MODE := arm
#TARGET_ARCH_ABI :=armeabi-v7a

# 采用NEON优化技术
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)   
LOCAL_ARM_NEON := true   
endif

LOCAL_SRC_FILES := \
	source/image_convert.c

# 默认包含的头文件路径
LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/../libyuv/include \
	$(LOCAL_PATH)/../libjpeg/source

LOCAL_SHARED_LIBRARIES := libutils libyuv libjpeg

# -g 后面的一系列附加项目添加了才能使用 arm_neon.h 头文件 -mfloat-abi=softfp -mfpu=neon 使用 arm_neon.h 必须
#LOCAL_CFLAGS := -D__cpusplus -g -mfloat-abi=softfp -mfpu=neon -march=armv7-a -mtune=cortex-a8
#LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -Wno-sign-compare -Wno-switch -Wno-pointer-sign -DHAVE_NEON=1 -mfpu=neon -mfloat-abi=softfp -fPIC -DANDROID
LOCAL_LDLIBS    :=  -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)
