LOCAL_PATH:=$(call my-dir)
include $(CLEAR_VARS)
#ANDROID_NDK_ROOT is my enviroment variable it points to /opt/android/android-ndk-r8e
NDK = $(ANDROID_NDK_ROOT)

LOCAL_MODULE:= libmp4v2
LOCAL_CPPFLAGS := -O2 -fexceptions -DHAVE_SOCKLEN_T -DHAVE_STRUCT_IOVEC
LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES:= \
	source/src/3gp.cpp source/src/atom_ac3.cpp \
	source/src/atom_amr.cpp source/src/atom_avc1.cpp source/src/atom_avcC.cpp \
	source/src/atom_chpl.cpp source/src/atom_colr.cpp source/src/atom_d263.cpp \
	source/src/atom_dac3.cpp source/src/atom_damr.cpp source/src/atom_dref.cpp \
	source/src/atom_elst.cpp source/src/atom_enca.cpp source/src/atom_encv.cpp \
	source/src/atom_free.cpp source/src/atom_ftyp.cpp source/src/atom_ftab.cpp \
	source/src/atom_gmin.cpp source/src/atom_hdlr.cpp source/src/atom_hinf.cpp \
	source/src/atom_hnti.cpp source/src/atom_href.cpp source/src/atom_mdat.cpp \
	source/src/atom_mdhd.cpp source/src/atom_meta.cpp source/src/atom_mp4s.cpp \
	source/src/atom_mp4v.cpp source/src/atom_mvhd.cpp source/src/atom_nmhd.cpp \
	source/src/atom_ohdr.cpp source/src/atom_pasp.cpp source/src/atom_root.cpp \
	source/src/atom_rtp.cpp source/src/atom_s263.cpp source/src/atom_sdp.cpp \
	source/src/atom_sdtp.cpp source/src/atom_smi.cpp source/src/atom_sound.cpp \
	source/src/atom_standard.cpp source/src/atom_stbl.cpp source/src/atom_stdp.cpp \
	source/src/atom_stsc.cpp source/src/atom_stsd.cpp source/src/atom_stsz.cpp \
	source/src/atom_stz2.cpp source/src/atom_text.cpp source/src/atom_tfhd.cpp \
	source/src/atom_tkhd.cpp source/src/atom_treftype.cpp source/src/atom_trun.cpp \
	source/src/atom_tx3g.cpp source/src/atom_udta.cpp source/src/atom_url.cpp \
	source/src/atom_urn.cpp source/src/atom_uuid.cpp source/src/atom_video.cpp \
	source/src/atom_vmhd.cpp source/src/atoms.h source/src/cmeta.cpp \
	source/src/descriptors.cpp source/src/descriptors.h source/src/exception.cpp \
	source/src/exception.h source/src/enum.h source/src/enum.tcc source/src/impl.h \
	source/src/isma.cpp source/src/log.h source/src/log.cpp source/src/mp4.cpp source/src/mp4array.h \
	source/src/mp4atom.cpp source/src/mp4atom.h source/src/mp4container.cpp \
	source/src/mp4container.h source/src/mp4descriptor.cpp source/src/mp4descriptor.h \
	source/src/mp4file.cpp source/src/mp4file.h source/src/mp4file_io.cpp \
	source/src/mp4info.cpp source/src/mp4property.cpp source/src/mp4property.h \
	source/src/mp4track.cpp source/src/mp4track.h source/src/mp4util.cpp source/src/mp4util.h \
	source/src/ocidescriptors.cpp source/src/ocidescriptors.h source/src/odcommands.cpp \
	source/src/odcommands.h source/src/qosqualifiers.cpp source/src/qosqualifiers.h \
	source/src/rtphint.cpp source/src/rtphint.h source/src/src.h source/src/text.cpp \
	source/src/text.h source/src/util.h source/src/bmff/bmff.h source/src/bmff/impl.h \
	source/src/bmff/typebmff.cpp source/src/bmff/typebmff.h \
	source/src/itmf/CoverArtBox.cpp source/src/itmf/CoverArtBox.h \
	source/src/itmf/Tags.cpp source/src/itmf/Tags.h source/src/itmf/generic.cpp \
	source/src/itmf/generic.h source/src/itmf/impl.h source/src/itmf/itmf.h \
	source/src/itmf/type.cpp source/src/itmf/type.h \
	source/src/qtff/ColorParameterBox.cpp source/src/qtff/ColorParameterBox.h \
	source/src/qtff/PictureAspectRatioBox.cpp \
	source/src/qtff/PictureAspectRatioBox.h source/src/qtff/coding.cpp \
	source/src/qtff/coding.h source/src/qtff/impl.h source/src/qtff/qtff.h \
	source/libplatform/endian.h source/libplatform/impl.h \
	source/libplatform/io/File.cpp source/libplatform/io/File.h \
	source/libplatform/io/FileSystem.cpp source/libplatform/io/FileSystem.h \
	source/libplatform/number/random.h source/libplatform/platform.h \
	source/libplatform/platform_base.h source/libplatform/platform_posix.h \
	source/libplatform/process/process.h \
	source/libplatform/prog/option.cpp source/libplatform/prog/option.h \
	source/libplatform/sys/error.cpp source/libplatform/sys/error.h \
	source/libplatform/time/time.cpp source/libplatform/time/time.h \
	source/libplatform/warning.h source/libplatform/io/File_posix.cpp \
	source/libplatform/io/FileSystem_posix.cpp \
	source/libplatform/number/random_posix.cpp \
	source/libplatform/process/process_posix.cpp \
	source/libplatform/time/time_posix.cpp \
	source/libutil/Database.cpp \
	source/libutil/Database.h source/libutil/Timecode.cpp source/libutil/Timecode.h \
	source/libutil/TrackModifier.cpp source/libutil/TrackModifier.h \
	source/libutil/Utility.cpp source/libutil/Utility.h source/libutil/crc.cpp \
	source/libutil/crc.h source/libutil/impl.h source/libutil/other.cpp source/libutil/other.h \
	source/libutil/util.h

LOCAL_ARM_MODE := arm
LOCAL_CFLAGS := -fexceptions

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/source \
	$(LOCAL_PATH)/source/include \
	$(LOCAL_PATH)/source/libplatform \
	$(LOCAL_PATH)/source/libutil

LOCAL_LDFLAGS := \
        $(NDK)/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a/libsupc++.a

LOCAL_LDLIBS := -llog

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libbinder \
    libcutils
LOCAL_CXXFLAGS :=-fexceptions -Wno-write-strings

include $(BUILD_SHARED_LIBRARY)
