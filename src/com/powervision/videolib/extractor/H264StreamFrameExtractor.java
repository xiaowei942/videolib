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
    public void setSps(ByteBuffer sps) {
        super.setSps(sps);
    };

    public void setPps(ByteBuffer pps) {
        super.setPps(pps);
    };

    public void setSpsLength(int length) {
        super.setSpsLength(length);
    };

    public void setPpsLength(int length) {
        super.setPpsLength(length);
    };
    //Get one frame video data, we must implement it blocking
    public ByteBuffer getSps() {
        return JniNativesProxy.getSps();
    };
    public ByteBuffer getPps() {
        return JniNativesProxy.getPps();
    };
    public int getSpsLength() {
        return JniNativesProxy.getSpsLength();
    };
    public int getPpsLength() {
        return JniNativesProxy.getPpsLength();
    };
}
