package com.powervision.videolib.render;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;

/**
 * Created by liwei on 15-7-24.
 */
public class SurfaceViewRenderer extends Renderer {
    private SurfaceHolder holder;
    private Canvas canvas;
    private Bitmap bitmap;
    private View view;

    SurfaceViewRenderer(View view) {
        setRendererType(RendererType.RendererType_SurfaceView);
        this.view = view;
    }


    private void draw(View view) {
        canvas = holder.lockCanvas();
        if(canvas != null) {
            Log.i("MySurfaceView", "draw !!!");
            Rect rect = new Rect();
            view.getDrawingRect(rect);
            canvas.drawBitmap(bitmap, null, rect, null);
        }
        holder.unlockCanvasAndPost(canvas);
    }


    @Override
    public void renderFrame() {
        draw(view);
    }
}
