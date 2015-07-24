package com.powervision.videolib.extractor;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-3.
 */
public interface DataExtractor {
    //Get one frame video data, we must implement it blocking
    public byte[] getFrame();
    public boolean openDataExtractor();
    public void closeDataExtractor();
    public void start();
    public int getStatus();
    ByteBuffer getSps();
    ByteBuffer getPps();
}
