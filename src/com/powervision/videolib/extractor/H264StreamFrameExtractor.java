package com.powervision.videolib.extractor;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class H264StreamFrameExtractor extends H264FrameExtractor {
    @Override
    public int open() {
        return 0;
    }

    @Override
    public void close() {

    }

    @Override
    public void start() {

    }

    @Override
    public byte[] getFrame() {
        return new byte[0];
    }
}
