package com.powervision.videolib.jni;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-26.
 */
public class JniNativesProxy {
    static {
        JniNatives.loadlibrary();
    }

    private static JniNativesProxy mInstance = null;
    public static synchronized JniNativesProxy getInstance() {
        if(mInstance == null) {
            mInstance = new JniNativesProxy();
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
    }

    public static int convertYUV420P2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        int ret = JniNatives.native_convertYUV420P2ARGB(data, yuv420, width, height);
        return ret;
    }

    public static int convertYUV420SP2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        int ret = JniNatives.native_convertYUV420SP2ARGB(data, yuv420, width, height);
        return ret;
    }


    /*********** For Image Transfer ************/
    public static int transferInit(int width, int height) {
        return JniNatives.native_transferInit(width, height);
    }

    public static void transferUnInit(int obj) {
        JniNatives.native_transferUnInit(obj);
    }

    public static int initSocket(int object, String serverIp, String localIp, int localPort) {
        return JniNatives.native_initSocket(object, serverIp, localIp, localPort);
    }

    public static int unInitSocket(int object, int port) {
        return JniNatives.native_unInitSocket(object, port);
    }

    public static int getSps(int object, byte[] sps) {
        return JniNatives.native_getSps(object, sps);
    }

    public static int getPps(int object, byte[] pps) {
        return JniNatives.native_getPps(object, pps);
    }

    public static int startReceive(int obj) {
        return JniNatives.native_startReceive(obj);
    }

    public static int stopReceive(int obj) {
        return JniNatives.native_stopReceive(obj);
    }

    public static int startProcess(int obj) {
        return JniNatives.native_startProcess(obj);
    }

    public static int stopProcess(int obj) {
        return JniNatives.native_stopProcess(obj);
    }

    public static int getFrame(int obj, byte[] frame) {
        return JniNatives.native_getFrame(obj, frame);
    }

    public static byte[] getFrame2(int obj) {
        return JniNatives.native_getFrame2(obj);
    }

    public static boolean native_isPrepared(int obj) {
        return JniNatives.native_isPrepared(obj);
    }
}
