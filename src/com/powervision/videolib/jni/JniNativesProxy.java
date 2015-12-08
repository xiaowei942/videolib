package com.powervision.videolib.jni;

/**
 * Created by liwei on 15-7-26.
 */
public class JniNativesProxy {
    static {
        JniNatives.loadlibrary();
    }

    private static JniNativesProxy mInstance = null;
    public static synchronized JniNativesProxy getInstance() {
        if(mInstance != null) {
            mInstance = new JniNativesProxy();
            return mInstance;
        }

        return mInstance;
    }

    public static int writerInit(int width, int height) {
        int ret = JniNatives.native_writerInit(width, height);
        return ret;
    };

    public static void setFileName(int obj, String fileName) {
        JniNatives.native_setFileName(obj, fileName);
    };

    public static void setFps(int obj, int fps) {
        JniNatives.native_setFps(obj, fps);
    };

    public static void startRecord(int obj) {
        JniNatives.native_startRecord(obj);
    };

    public static  void stopRecord(int obj) {
        JniNatives.native_stopRecord(obj);
    };

    public static void writeFrame(int obj, byte[] data, long size, long ts) {
        JniNatives.native_writeFrame(obj, data, size, ts);
    };

    public static int convertYUV420P2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        int ret = JniNatives.native_convertYUV420P2ARGB(data, yuv420, width, height);
        return ret;
    }

    public static int convertYUV420SP2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        int ret = JniNatives.native_convertYUV420SP2ARGB(data, yuv420, width, height);
        return ret;
    }
    //-------------lbg 2015.12.07---------------
    public static ByteBuffer getPps() {
        return JniNatives.native_getPps();
    }
    public static ByteBuffer getSps() {
        return JniNatives.native_getSps();
    }
    public static int getPpsLength() {
        return JniNatives.native_getPpsLength();
    }
    public static int getSpsLength() {
        return JniNatives.native_getSpsLength();
    }
}
