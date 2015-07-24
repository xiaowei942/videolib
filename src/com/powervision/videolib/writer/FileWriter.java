package com.powervision.videolib.writer;

/**
 * Created by liwei on 15-7-24.
 */
public interface FileWriter {
    public abstract void open();
    public abstract void writeFrame(byte[] data, long size, long ts);
    public abstract void close();
}
