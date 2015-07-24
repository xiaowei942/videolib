package com.powervision.videolib.codec;

/**
 * Created by liwei on 15-7-24.
 */
public interface Codec {
    public static final int CODEC_TYPE_DEFAULT = 0;
    public static final int CODEC_TYPE_MEDIACODEC = 1;
    public static final int CODEC_TYPE_OTHER = 2;

    public void initCodec(Object obj);
    public void openCodec();
    public void closeCodec();
    public void releaseCodec();
}
