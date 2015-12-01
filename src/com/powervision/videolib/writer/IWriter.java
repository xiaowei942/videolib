package com.powervision.videolib.writer;

import com.powervision.videolib.codec.ICodec;

/**
 * Created by liwei on 15-7-27.
 */
public interface IWriter {
    public abstract void writeFrame(byte[] data, long size, long ts);
}
