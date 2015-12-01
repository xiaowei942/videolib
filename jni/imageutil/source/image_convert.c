#include <stdlib.h>
#include "image_convert.h"

#include <android/log.h>
#include <android/bitmap.h>
#include <math.h>
#include <libyuv/convert_from.h>
#include <jpeglib.h>
#define LOG_TAG "image_convert"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

jint Java_com_powervision_videolib_jni_JniNatives_native_1convertYUV420P2ARGB(JNIEnv * env,
		jobject thiz, jbyteArray dat, jbyteArray buf, jint width, jint height) {
	jbyte * yuv420p = (*env)->GetByteArrayElements(env, buf, 0);
	jbyte * rgb = (*env)->GetByteArrayElements(env, dat, 0);

	I420ToARGB((unsigned char *)yuv420p, width, 
		   (unsigned char *)(yuv420p+width*height*5/4), width/2,
		   (unsigned char *)(yuv420p+width*height), width/2,
		   rgb, width*4,
		   width, height
		  );

	(*env)->ReleaseByteArrayElements(env, buf, yuv420p, 0);
	(*env)->ReleaseByteArrayElements(env, dat, rgb, 0);
}



jint Java_com_powervision_videolib_converter_FormatConverter_native_1convertYUV420SP2ARGB(JNIEnv * env,
		jobject thiz, jbyteArray dat, jbyteArray buf, jint width, jint height) {
	/*
	jbyte * yuv420p = (*env)->GetByteArrayElements(env, buf, 0);
	jbyte * rgb = (*env)->GetByteArrayElements(env, dat, 0);

	NV12ToARGB((unsigned char *)yuv420p, width, 
		   (unsigned char *)(yuv420p+width*height), width/2,
		   rgb, width*4,
		   width, height
		  );

	(*env)->ReleaseByteArrayElements(env, buf, yuv420p, 0);
	(*env)->ReleaseByteArrayElements(env, dat, rgb, 0);
	*/
		jbyte * yuv420p = (*env)->GetByteArrayElements(env, buf, 0);
	jbyte * rgb = (*env)->GetByteArrayElements(env, dat, 0);

	I420ToARGB((unsigned char *)yuv420p, width, 
		   (unsigned char *)(yuv420p+width*height), width/2,
		   (unsigned char *)(yuv420p+width*height*5/4), width/2,
		   rgb, width*4,
		   width, height
		  );

	(*env)->ReleaseByteArrayElements(env, buf, yuv420p, 0);
	(*env)->ReleaseByteArrayElements(env, dat, rgb, 0);
}
