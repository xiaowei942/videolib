package com.powervision.videolib.extractor;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-28.
 */
public abstract class Extractor {
    public static final int EXTRACTOR_STATUS_FAILED = 0;
    public static final int EXTRACTOR_STATUS_OK = 1;

    public abstract int open();
    public abstract void close();
    public abstract void start();

    private int extractorStatus = EXTRACTOR_STATUS_FAILED;
    public int getExtractorStatus() {
        return extractorStatus;
    }

    public void setExtractorStatus(int status) {
        extractorStatus = status;
    }
}
