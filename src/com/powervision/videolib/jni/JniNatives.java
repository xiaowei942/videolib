package com.powervision.videolib.jni;

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
    //-----------lbg 2015.12.07-------------------

    public static native ByteBuffer native_getSps();
    public static native ByteBuffer native_getPps();
    public static native int native_getPpsLength();
    public static native int native_getSpsLength();
}
