package com.powervision.videolib.jni;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class JniNatives {

    public static void loadlibrary() {
        String[] LIBS = new String[] { "yuv", "yuv2rgb",
                "mp4v2", "video" };
        for (int i = 0; i < LIBS.length; i++) {
            try {
                System.loadLibrary(LIBS[i]);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    public static native int native_writerInit(int width, int height);
    public static native void native_setFileName(int obj, String fileName);
    public static native void native_setFps(int obj, int fps);
    public static native void native_startRecord(int obj);
    public static native void native_stopRecord(int obj);
    public static native void native_writeFrame(int obj, byte[] data, long size, long ts);

    public static native int native_convertYUV420P2ARGB(byte[] data, byte[] yuv420, int width, int height);
    public static native int native_convertYUV420SP2ARGB(byte[] data, byte[] yuv420, int width, int height);

    /*********** For Image Transfer ************/
    public static native int native_transferInit(int width, int height);
    public static native void native_transferUnInit(int obj);
    public static native int native_initSocket(int obj, String serverIp, String localIp, int localPort);
    public static native int native_unInitSocket(int obj, int localPort);
    public static native int native_getSps(int obj, byte[] sps);
    public static native int native_getPps(int obj, byte[] pps);
}
