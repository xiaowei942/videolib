package com.powervision.videolib.codec;

/**
 * Created by liwei on 15-7-24.
 */
public abstract class Codec {
    public static final int CODEC_TYPE_DEFAULT = 0;
    public static final int CODEC_TYPE_MEDIACODEC = 1;
    public static final int CODEC_TYPE_OTHER = 2;

    public abstract void initCodec(Object obj);
    public abstract void openCodec();
    public abstract void start();
    public abstract void stop();
    public abstract void pause();
    public abstract void closeCodec();
    public abstract void releaseCodec();
    public abstract void decode();
}
