package com.powervision.videolib.codec;

import com.powervision.videolib.render.IRenderer;

/**
 * Created by liwei on 15-7-27.
 */
public interface ICodec {
    public void renderFrame(IRenderer render);
}
