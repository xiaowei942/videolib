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

    public static class Mp4Writer {
        public static int writerInit(int width, int height) {
            int ret = JniNatives.Mp4Writer.native_writerInit(width, height);
            return ret;
        };

        public static void native_setFileName(int obj, String fileName) {
            JniNatives.Mp4Writer.native_setFileName(obj, fileName);
        };

        public static void setFps(int obj, int fps) {
            JniNatives.Mp4Writer.native_setFps(obj, fps);
        };

        public static void startRecord(int obj) {
            JniNatives.Mp4Writer.native_startRecord(obj);
        };
        public static  void stopRecord(int obj) {
            JniNatives.Mp4Writer.native_stopRecord(obj);
        };

        public static void native_writeFrame(int obj, byte[] data, long size, long ts) {
            JniNatives.Mp4Writer.native_writeFrame(obj, data, size, ts);
        };
    }

    public static class FormatConverter {
        public static int convertYUV420P2RGB(byte[] data, byte[] yuv420, int width, int height) {
            int ret = JniNatives.FormatConverter.convertYUV420P2RGB(data, yuv420, width, height);
            return ret;
        }
    }
}
