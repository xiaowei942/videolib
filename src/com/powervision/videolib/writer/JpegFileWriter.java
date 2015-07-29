package com.powervision.videolib.writer;

import com.powervision.videolib.codec.ICodec;

/**
 * Created by liwei on 15-7-27.
 */
public class JpegFileWriter extends FileWriter {
    @Override
    public int open() {
        return 0;
    }

    @Override
    public void close() {

    }

    @Override
    public void writeFrame(byte[] data, long size, long ts) {

    }
}
