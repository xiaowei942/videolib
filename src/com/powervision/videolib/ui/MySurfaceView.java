package com.powervision.videolib.ui;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by liwei on 15-7-24.
 */
public class MySurfaceView extends SurfaceView implements Runnable {

    private SurfaceHolder holder;
    private Canvas canvas;
    private Bitmap bitmap;
    private boolean isStop = false;
    public MySurfaceView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public MySurfaceView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    public void stop() {
        isStop = true;
    }
    public MySurfaceView(Context context) {
        super(context);
        this.setKeepScreenOn(true);
        this.setFocusable(true);
        holder = this.getHolder();
    }

    public void setHolder(SurfaceHolder hd) {
        this.holder = hd;
    }

    public void setFrameBitmap(Bitmap bmp) {
        bitmap = bmp;
    }

    @Override
    public void run() {
        while (!isStop) {
            synchronized (bitmap) {
                draw();
            }
            try {
                Thread.sleep(10);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    private void draw() {
        canvas = holder.lockCanvas();
        Log.i("MySurfaceView", "draw !!!");
        Rect rect = new Rect();
        getDrawingRect(rect);
        canvas.drawBitmap(bitmap, null, rect, null);
        holder.unlockCanvasAndPost(canvas);
    }
}


