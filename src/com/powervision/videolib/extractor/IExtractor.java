package com.powervision.videolib.extractor;

/**
 * Created by liwei on 15-7-28.
 */
public interface IExtractor {
    public int getFrameSize();
    public byte[] getFrame();
    public String getDescribe();
}
