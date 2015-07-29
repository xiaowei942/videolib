package com.powervision.videolib.converter;

import com.powervision.videolib.jni.JniNativesProxy;

/**
 * Created by liwei on 15-7-24.
 */
public class FormatConverter {
    public int YUV420P2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        return JniNativesProxy.getInstance().convertYUV420P2ARGB(data, yuv420, width, height);
    }

    public int YUV420SP2ARGB(byte[] data, byte[] yuv420, int width, int height) {
        return JniNativesProxy.getInstance().convertYUV420SP2ARGB(data, yuv420, width, height);
    }

}
