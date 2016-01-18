package com.powervision.videolib.codec;

import android.view.SurfaceView;
import com.powervision.videolib.render.IRenderer;
import com.powervision.videolib.render.Renderer;
import com.powervision.videolib.writer.Mp4FileWriter;

import java.util.ArrayList;

/**
 * Created by liwei on 15-7-27.
 */
public abstract class BaseCodec extends Codec implements ICodec{
    /*************** Renderer Start ****************/
    IRenderer mCurrentRenderer = null;
    SurfaceView aSurfaceView = null;
    ArrayList<IRenderer> renderers = new ArrayList<IRenderer>();

    public SurfaceView getaSurfaceView() {
        return aSurfaceView;
    }

    public synchronized void setCurrentRenderer(int type) {
        IRenderer renderer = findRendererListByType(type);
        if(renderer != null) {
            mCurrentRenderer = renderer;
        }
    }

    public void addRenderer(IRenderer renderer) {
        renderers.add(renderer);

    }

    public IRenderer findRendererListByType(int type) {
        for(int i=0; i<renderers.size(); i++) {
            if(type == Renderer.RendererType.RendererType_SurfaceView) {
                return renderers.get(i);
            }
        }
        return null;
    }

    /***************  Renderer End  ****************/

    /*************** Display Start ****************/
     int mFps = -1;
     int mWidth = -1;
     int mHeight = -1;

    public int getWidth() {
        return mWidth;
    }

    public void setWidth(int width) {
        mWidth = width;
    }

    public int getHeight() {
        return mHeight;
    }

    public void setHeight(int height) {
        mHeight = height;
    }

    public int getFps() {
        return mFps;
    }

    public void setFps(int fps) {
        mFps = fps;
    }
    /***************  Display End ****************/


    /*************** Writer Start ****************/
    Mp4FileWriter writer = null;
    static boolean closeWriter = false;
    /***************  Writer End ****************/

}
