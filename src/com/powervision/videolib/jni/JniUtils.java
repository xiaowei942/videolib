package com.powervision.videolib.jni;

/**
 * Created by liwei on 15-7-24.
 */
public class JniUtils {
    public static class Mp4Writer {
        public static native int native_writerInit(int width, int height);
        public static native void native_setFileName(int obj, String fileName);
        public static native void native_setFps(int obj, int fps);
        public static native void native_startRecord(int obj);
        public static native void native_stopRecord(int obj);
        public static native void native_writeFrame(int obj, byte[] data, long size, long ts);
    }

    public static class FormatConverter {
        public static native int convertYUV420P2RGB(byte[] data, byte[] yuv420, int width, int height);
    }
}
