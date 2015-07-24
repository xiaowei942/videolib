package com.powervision.videolib.codec;

import android.view.Surface;
import com.powervision.videolib.extractor.DataExtractor;

/**
 * Created by liwei on 15-7-24.
 */
public class CodecParam {
    public DataExtractor extractor = null;
    public static int codecType = Codec.CODEC_TYPE_DEFAULT;
    public Object obj = null;
    public Surface surface = null;
    public int width = -1;
    public int height = -1;
}
