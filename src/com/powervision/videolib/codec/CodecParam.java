package com.powervision.videolib.codec;

import android.view.Surface;
import android.view.SurfaceHolder;
import com.powervision.videolib.extractor.H264FrameExtractor;

/**
 * Created by liwei on 15-7-24.
 */
public class CodecParam {
    public static int codecType = Codec.CODEC_TYPE_DEFAULT;
    public H264FrameExtractor extractor = null;
    public Object obj = null;
    public Surface surface = null;
    public SurfaceHolder holder = null;
    public int width = -1;
    public int height = -1;
}
