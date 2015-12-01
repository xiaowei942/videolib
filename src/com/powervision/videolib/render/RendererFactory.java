package com.powervision.videolib.render;

import android.view.View;
import com.powervision.videolib.codec.ICodec;

/**
 * Created by liwei on 15-7-28.
 */
public class RendererFactory {
    public static IRenderer createSurfaceViewRenderer(View view) {
        return new SurfaceViewRenderer(view);
    }
}
