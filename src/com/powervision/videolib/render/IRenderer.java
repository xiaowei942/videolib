package com.powervision.videolib.render;

import com.powervision.videolib.codec.Codec;

/**
 * Created by liwei on 15-7-24.
 */
public interface IRenderer {
    public void renderFrame();
    public int getType();
}
