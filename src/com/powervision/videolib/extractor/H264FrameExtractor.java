package com.powervision.videolib.extractor;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public abstract class H264FrameExtractor extends Extractor implements IExtractor{
    //Get one frame video data, we must implement it blocking
    public ByteBuffer getSps() {
        return Sps;
    }

    public void setSps(ByteBuffer sps) {
        Sps = sps;
    }

    public ByteBuffer getPps() {
        return Pps;
    }

    public void setPps(ByteBuffer pps) {
        Pps = pps;
    };

    public int getSpsLength() {
        return spsLength;
    }

    public void setSpsLength(int length) {
        spsLength = length;
    }

    public int getPpsLength() {
        return ppsLength;
    }

    public void setPpsLength(int length) {
        ppsLength = length;
    }

    public abstract boolean isPrepared();

    private int spsLength;
    private int ppsLength;
    private ByteBuffer Sps = null;
    private ByteBuffer Pps = null;
}
