package com.powervision.videolib.writer;

import com.powervision.videolib.jni.JniUtils;

/**
 * Created by liwei on 15-7-24.
 */
public class Mp4Writer implements FileWriter {

    private int nativeWriterObject;

    @Override
    public void open() {

    }

    @Override
    public void writeFrame(byte[] data, long size, long ts) {

    }

    @Override
    public void close() {
        JniUtils.Mp4Writer.native_stopRecord(nativeWriterObject);
    }
}
