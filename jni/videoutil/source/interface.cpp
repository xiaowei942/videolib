#include <stdio.h>
#include <iostream>
#include <mp4v2/mp4v2.h>

#include "transfer.h"
#include "interface.h"
#include "mp4_writer.h"
#include "mp4_extractor.h"
#include <android/log.h>
using namespace std;

#define LOG_TAG "INTERFACE"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define FILE_PATH "/sdcard/test.h264"
#define MP4_FILE_PATH "/sdcard/test.mp4"
#define MP4_FPS 30

/*
 * Class:     com_powervision_videolibtest_MyActivity
 * Method:    native_test
 * Signature: (I)I
 */
JNIEXPORT int JNICALL Java_com_powervision_videolibtest_MyActivity_native_1test
  (JNIEnv *env, jobject thiz) {
       	H264_Extractor *extractor = new H264_Extractor();
	extractor->get_to_list(FILE_PATH);
	extractor->get_sps_pps();

#if 0
	MP4FileHandle mp4_file = MP4CreateEx("/home/liwei/Desktop/test.mp4", 0, 1, 1, 0, 0, 0, 0);
	if(mp4_file == MP4_INVALID_FILE_HANDLE) {
		printf("Create mp4 file failed\n");
		return -1;
	}

	MP4SetTimeScale(mp4_file, 90000);
	MP4TrackId video = MP4AddH264VideoTrack(mp4_file, 90000, 90000/25, 1280, 720, extractor->sps[1], extractor->sps[2], extractor->sps[3], 3);
	if(video == MP4_INVALID_TRACK_ID) {
		printf("Create video track failed\n");
		return -1;
	}
	MP4AddH264SequenceParameterSet(mp4_file, video, extractor->sps, 26);
	MP4AddH264PictureParameterSet(mp4_file, video, extractor->pps, 5);
#endif

	Mp4_Writer *writer = new Mp4_Writer(1280, 720);
	writer->SetMp4FileName(MP4_FILE_PATH);
	writer->SetMp4Fps(MP4_FPS);
	writer->DoStartRecord();

	unsigned char *payload_data;
	unsigned int payload_size;
	unsigned int time_stamp;
	int i=0;
	while( ((payload_data = extractor->get_frame(payload_size, time_stamp)) != NULL ) && (i<1000) ) {
		writer->WriteEncodedVideoFrame(payload_data, payload_size, time_stamp);
		extractor->release_frame(&payload_data);
		i++;
	}
	writer->DoStopRecord();
	return 0;
}


/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_writerInit
 * Signature: (II)I
 */
JNIEXPORT Mp4_Writer * JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1writerInit
  (JNIEnv *env, jobject thiz, jint width, jint height) {
	Mp4_Writer *writer = new Mp4_Writer(width, height);
	if(writer) {
		return writer;
	}
	return NULL;
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_setFileName
 * Signature: (Ljava/lang/Object;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1setFileName
  (JNIEnv *env, jobject thiz, Mp4_Writer *obj, jstring file_name) {
	const char *name = env->GetStringUTFChars(file_name, 0);
	obj->SetMp4FileName(name);
	env->ReleaseStringUTFChars(file_name, name);
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_setFps
 * Signature: (Ljava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1setFps
  (JNIEnv *env, jobject thiz, Mp4_Writer *obj, jint fps) {
	obj->SetMp4Fps(fps);
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_startRecord
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1startRecord
  (JNIEnv *env, jobject thiz, Mp4_Writer *obj) {
	obj->DoStartRecord();
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_stopRecord
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1stopRecord
  (JNIEnv *env, jobject thiz, Mp4_Writer *obj) {
	obj->DoStopRecord();
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_writeFrame
 * Signature: (Ljava/lang/Object;[BJJ)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1writeFrame
  (JNIEnv *env, jobject thiz, Mp4_Writer *obj, jbyteArray data, jlong size, jlong ts) {
	jbyte *payload_data = env->GetByteArrayElements(data, 0);
	obj->WriteEncodedVideoFrame((unsigned char *)payload_data, size, ts);
	env->ReleaseByteArrayElements(data, payload_data, 0);
  }


/******************  Transfer  ********************/

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_transferInit
 * Signature: (II)I
 */
JNIEXPORT Transfer* JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1transferInit
  (JNIEnv *env, jobject thiz, jint width, jint height) {
	LOGI("Enter native_transferInit");
	Transfer *transfer = new Transfer(width, height);
	if(transfer) {
		LOGI("Leave native_transferInit");
		return transfer;
	}
	LOGI("Leave native_transferInit return NULL");
	return NULL;
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_transferUnInit
 * Signature: (Ljava/lang/Object;)V
 */
JNIEXPORT void JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1transferUnInit
  (JNIEnv *env, jobject thiz, jint obj) {
	LOGI("Enter native_transferUnInit");
	Transfer *transfer = (Transfer *)obj;
	if(transfer) {
		delete transfer;
		transfer = NULL;
	}
	LOGI("Leave native_transferUnInit");
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_initSocket
 * Signature: (II)I
 */
JNIEXPORT int JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1initSocket
  (JNIEnv *env, jobject thiz, Transfer *obj, jstring serv_ip, jstring loc_ip, jint port) {
	LOGI("Enter native_initSocket");
	const char *server_ip = env->GetStringUTFChars(serv_ip, 0);
	const char *local_ip = env->GetStringUTFChars(loc_ip, 0);
	jint ret = obj->initSocket(server_ip, local_ip, port);
	env->ReleaseStringUTFChars(loc_ip, local_ip);
	env->ReleaseStringUTFChars(serv_ip, server_ip);
	LOGI("Leave native_initSocket");
	return ret;
  }

/*
 * Class:     com_powervision_videolib_jni_JniNatives
 * Method:    native_unInitSocket
 * Signature: (II)I
 */
JNIEXPORT int JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1unInitSocket
  (JNIEnv *env, jclass thiz, jint obj, jint port) {
	Transfer *transfer = (Transfer *)obj;
	return transfer->unInitSocket(port);
  }

JNIEXPORT int JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1getSps
  (JNIEnv *env, jclass thiz, jint obj, jbyteArray sps) {
	Transfer *transfer = (Transfer *)obj;
	uint8_t *p = (uint8_t *)env->GetByteArrayElements(sps, JNI_FALSE);
	int ret = transfer->getSps(p);
	return ret;
  }

JNIEXPORT int JNICALL Java_com_powervision_videolib_jni_JniNatives_native_1getPps
  (JNIEnv *env, jclass thiz, jint obj, jbyteArray pps) {
	Transfer *transfer = (Transfer *)obj;
	uint8_t *p = (uint8_t *)env->GetByteArrayElements(pps, JNI_FALSE);
	int ret = transfer->getPps(p);
	return ret;
  }

JNIEXPORT int JNICALL  Java_com_powervision_videolib_jni_JniNatives_native_1startReceive
  (JNIEnv *env, jclass thiz, jint obj) {
	Transfer *transfer = (Transfer *)obj;
	return transfer->startReceive();
  }

JNIEXPORT int JNICALL  Java_com_powervision_videolib_jni_JniNatives_native_1stopReceive
  (JNIEnv *env, jclass thiz, jint obj) {
	Transfer *transfer = (Transfer *)obj;
	return transfer->stopReceive();
  }

JNIEXPORT int JNICALL  Java_com_powervision_videolib_jni_JniNatives_native_1startProcess
  (JNIEnv *env, jclass thiz, jint obj) {
	Transfer *transfer = (Transfer *)obj;
	return transfer->startProcess();
  }

JNIEXPORT int JNICALL  Java_com_powervision_videolib_jni_JniNatives_native_1stopProcess
  (JNIEnv *env, jclass thiz, jint obj) {
	Transfer *transfer = (Transfer *)obj;
	return transfer->stopProcess();
  }

JNIEXPORT jbyteArray JNICALL  Java_com_powervision_videolib_jni_JniNatives_native_1getframe
  (JNIEnv *env, jclass thiz, jint obj, jint size) {
	Transfer *transfer = (Transfer *)obj;
	uint8_t *buf = transfer->get_frame((uint32_t &)size);
	jbyteArray array = env->NewByteArray(size);
 	env->SetByteArrayRegion(array, 0, size, (jbyte *)buf);
	return array;
  }

#if 1
static JNINativeMethod methods[] = {
	{ "native_writerInit", "(II)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1writerInit },
	{ "native_setFileName", "(ILjava/lang/String;)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1setFileName },
	{ "native_setFps", "(II)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1setFps },
	{ "native_startRecord", "(I)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1startRecord },
	{ "native_stopRecord", "(I)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1stopRecord },
	{ "native_writeFrame", "(I[BJJ)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1writeFrame },
	/************************* For Image Transfer ************************/	
	{ "native_transferInit", "(II)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1transferInit },
	{ "native_transferUnInit", "(I)V", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1transferUnInit },
	{ "native_initSocket", "(ILjava/lang/String;Ljava/lang/String;I)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1initSocket },
	{ "native_unInitSocket", "(II)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1unInitSocket },
	{ "native_getSps", "(I[B)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1getSps },
	{ "native_getPps", "(I[B)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1getPps },
	{ "native_startReceive", "(I)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1startReceive },
	{ "native_stopReceive", "(I)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1stopReceive },
	{ "native_startProcess", "(I)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1startProcess },
	{ "native_stopProcess", "(I)I", (void *)Java_com_powervision_videolib_jni_JniNatives_native_1stopProcess }
};

static const char * classPathName = "com/powervision/videolib/jni/JniNatives";

static int registerNativeMethods(JNIEnv* env, const char* className,
        JNINativeMethod* gMethods, int numMethods) 
{ 
        jclass clazz;
        clazz = env->FindClass(className);
        if (clazz == NULL) { 
                LOGE("Native registration unable to find class '%s'", className); 
                return JNI_FALSE;
        }
        if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
                LOGE("RegisterNatives failed for '%s'", className);
                return JNI_FALSE;
        }

        return JNI_TRUE;
}

static int registerNatives(JNIEnv* env)
{
        if (!registerNativeMethods(env, classPathName,
                     methods, sizeof(methods) / sizeof(methods[0]))) {
                return JNI_FALSE;
        }

        return JNI_TRUE;
}

typedef union {
        JNIEnv* env;
        void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
        UnionJNIEnvToVoid uenv;
        JNIEnv* env = NULL;
        LOGI("JNI_OnLoad!");

        if (vm->GetEnv((void**)&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
                LOGE("ERROR: GetEnv failed");
                return -1;
        }

        env = uenv.env;;

        if (registerNatives(env) != JNI_TRUE) {
                LOGE("ERROR: registerNatives failed");
                return -1;
	}

        return JNI_VERSION_1_4;
}
#endif
