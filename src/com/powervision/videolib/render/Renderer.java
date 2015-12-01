package com.powervision.videolib.render;

/**
 * Created by liwei on 15-7-27.
 */
public abstract class Renderer implements IRenderer{
    int rendererType = 0;

    public static class RendererType {
        public static final int RendererType_SurfaceView = 0;
        public static final int RendererType_OpenGl = 1;
    }

    public void setRendererType(int type) {
        this.rendererType = type;
    }

    public int getmRendererType() {
        return rendererType;
    }

    @Override
    public int getType() {
        return rendererType;
    }
}
