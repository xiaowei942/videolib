package com.powervision.videolib.codec;

/**
 * Created by liwei on 15-7-24.
 */
public class CodecFactory {
    public static Codec createCodec(CodecParam param) {
        switch (param.codecType) {
            case Codec.CODEC_TYPE_OTHER:
                return null;
            case Codec.CODEC_TYPE_MEDIACODEC:
                return new PowerCodec(param);
            case Codec.CODEC_TYPE_DEFAULT:
                return new PowerCodec(param);
            default:
                return null;
        }
    }


}
