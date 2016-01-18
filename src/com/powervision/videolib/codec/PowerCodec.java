package com.powervision.videolib.codec;

import android.graphics.Bitmap;
import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceView;
import com.powervision.videolib.capture.FrameCapturer;
import com.powervision.videolib.capture.OnCaptureFrameListener;
import com.powervision.videolib.extractor.H264FrameExtractor;
import com.powervision.videolib.jni.JniNativesProxy;
import com.powervision.videolib.render.IRenderer;
import com.powervision.videolib.render.Renderer;
import com.powervision.videolib.render.RendererFactory;
import com.powervision.videolib.render.SurfaceViewRenderer;
import com.powervision.videolib.writer.FileWriter;
import com.powervision.videolib.writer.FileWriterFactory;

import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class PowerCodec extends BaseCodec implements Runnable, OnCaptureFrameListener{
    private String TAG = "PowerCodec";
    private final boolean DEBUG = false;
    //视频截图模块
    OnCaptureFrameListener captureFrameListener = null;
    private FrameCapturer capturer = null;
     Bitmap captureBitmap = null;
    private boolean captureFrame = false;

    //数据获取模块
    private H264FrameExtractor extractor = null;

    //视频文件存储模块
    private FileWriter writer = null;
    boolean closeWriter = false;

    //图像渲染模块
    private IRenderer renderer = null;
     Surface mSurface = null;
     boolean hasSurface = false;
     H264FrameExtractor mExtractor = null;

     Bitmap surfaceBitmap = null;
    private byte[] brgb = null;

    //解码器部分
    MediaCodec codec = null;
    MediaCodec.BufferInfo info;
    private int decodedframes = 0;
    private boolean decoderconfigured = false;
    ByteBuffer[] decoderInputBuffers = null;
    ByteBuffer[] decoderOutputBuffers = null;
    MediaFormat decoderOutputFormat = null;
    private  final String MIME_TYPE = "video/avc";
    private  final long TIMEOUT_USEC = 1000;

    int codecStatus = 2; //0:stop 1:start 2:idle
    boolean encoderDone = false;
    boolean exitDecoder = false;

     ByteBuffer sps = null;
     ByteBuffer pps = null;
     int sps_len =0;
     int pps_len =0;

    boolean isPrepared = false;
    boolean decodeThreadExited = true;
    Thread decodeThread = null;

    void registerAllRenderers() {
        mCurrentRenderer = RendererFactory.createSurfaceViewRenderer(aSurfaceView);
        addRenderer(mCurrentRenderer);
    }

    PowerCodec(CodecParam param) {
        TAG = getClass().getSimpleName();

        mExtractor = param.extractor;
        mSurface = param.surface;
        if(mSurface == null) {
            hasSurface = false;
        } else {
            hasSurface = true;
        }
        registerAllRenderers();
        mCurrentRenderer = null;
        setCurrentRenderer(SurfaceViewRenderer.RendererType.RendererType_SurfaceView);

        capturer = FrameCapturer.getInstance();
        setCaptureFrameListener(this);

        if ((param.width % 16) != 0 || ((param.height % 16) != 0)) {
            Log.w(TAG, "WARNING: width or height not multiple of 16");
        }
        mWidth = param.width;
        mHeight = param.height;

        decodeThread = new Thread(this);
    }

    void initMediaCodec(ByteBuffer sps, int spsLength, ByteBuffer pps, int ppsLength) {
        setCurrentRenderer(Renderer.RendererType.RendererType_SurfaceView);

//        writer = FileWriterFactory.createMp4FileWriter(this, null);

//        if ((param.width % 16) != 0 || ((param.height % 16) != 0)) {
//            Log.w(TAG, "WARNING: width or height not multiple of 16");
//        }


        if (brgb == null) {
            brgb = new byte[mWidth * mHeight * 4];
        }

        if(false) {
            if (mCurrentRenderer.getType() == Renderer.RendererType.RendererType_SurfaceView) {
                // Initialize the bitmap, with the replaced color
                surfaceBitmap = Bitmap.createBitmap(mWidth, mHeight,
                        Bitmap.Config.ARGB_8888);
                    /* We cannot use following way to create a bitmap, or error occurs when setPixels
                        surfaceBitmap = Bitmap.createBitmap(irgb, mWidth, mHeight,
                                Bitmap.Config.ARGB_8888);
                    */
            }
        }
        writer = FileWriterFactory.createMp4FileWriter(this, null);
        writer.open();

        byte[] spsBuf = new byte[spsLength];
        sps.get(spsBuf, 0, spsLength);
        sps.position(0);
        byte[] ppsBuf = new byte[ppsLength];
        pps.get(ppsBuf, 0, ppsLength);
        pps.position(0);

        writer.writeFrame(spsBuf, spsLength, 1);
        writer.writeFrame(ppsBuf, ppsLength, 1);
        info = new MediaCodec.BufferInfo();

        try {
            codec = MediaCodec.createDecoderByType(MIME_TYPE);
        } catch (IOException e) {
            e.printStackTrace();
        }
        MediaFormat mediaFormat = MediaFormat.createVideoFormat(MIME_TYPE, getWidth(), getHeight());
        mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE, 2500000);
        mediaFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 35);
        mediaFormat.setInteger(MediaFormat.KEY_COLOR_FORMAT, MediaCodecInfo.CodecCapabilities.COLOR_FormatYUV420Planar);
        mediaFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1); //关键帧间隔时间 单位s
        mediaFormat.setByteBuffer("csd-0", sps);
        mediaFormat.setByteBuffer("csd-1", pps);
        Log.e("lbg width:",getWidth()+",height:"+getHeight()+"");
//        Log.e("lbg sps", Arrays.toString(spsBuf));
//        Log.e("lbg pps", Arrays.toString(ppsBuf));
        //format.setInteger("color-format", 19);
        codec.configure(mediaFormat, mSurface, null, 0);
        codec.start();

        decoderconfigured = true;
        decoderInputBuffers = codec.getInputBuffers();
        decoderOutputBuffers = codec.getOutputBuffers();
    }

    @Override
    public void initCodec(Object obj) {
        ByteBuffer sps = mExtractor.getSps();
        int sps_len = mExtractor.getSpsLength();
        ByteBuffer pps = mExtractor.getPps();
        int pps_len = mExtractor.getPpsLength();
        initMediaCodec(sps, sps_len, pps, pps_len);
    }

    @Override
    public void openCodec() {
        decodeThread.start();
    }

    @Override
    public void start() {
        Log.i(TAG, "Set Start Flag --> start");
        codecStatus = 1;
    }

    @Override
    public void stop() {
        Log.i(TAG, "Set Start Flag --> stop");
        codecStatus = 0;
        while(true) {
            if(decodeThreadExited) {
                return;
            }

            try {
                Thread.sleep(100);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void pause() {
        codecStatus = 2;
    }

    @Override
    public void closeCodec() {

    }

    @Override
    public void releaseCodec() {
        if(codec !=null){
            try{
                codec.stop();
                codec.release();
                codec =null;
            }catch (Exception e){
            e.printStackTrace();
                Log.e("PowerCodec",e.getMessage());
            }
        }
        mSurface=null;
        mExtractor=null;
        surfaceBitmap=null;
        mSurface=null;
        brgb=null;
    }

    public void renderFrame(IRenderer render) {
        render.renderFrame();
    }

    @Override
    public void decode() {
        int count = 1;
        long startMs = System.currentTimeMillis();
        long lasttime = startMs;
        while (!exitDecoder) {
            int size = 0;
            int inputBufferIndex = 0;
            if(decoderconfigured) {
                if (codecStatus == 0) { //停止
                    inputBufferIndex = codec.dequeueInputBuffer(TIMEOUT_USEC);
                    if (inputBufferIndex >= 0 ) {
                        if (codecStatus == 0) {
                            codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, MediaCodec.BUFFER_FLAG_END_OF_STREAM);
                            codecStatus = 0;
                        }
                    }
                } else if (codecStatus == 1) { //启动
                    byte[] framebuf = mExtractor.getFrame();
                    if (framebuf == null) {
                        continue;
                    }

                    size = framebuf.length;
                    if (size > 0) {
                        inputBufferIndex = codec.dequeueInputBuffer(TIMEOUT_USEC);
                        if (inputBufferIndex >= 0) {
                            if (framebuf == null) {
                                count++;
                                continue;
                            }

                            ByteBuffer bf = ByteBuffer.wrap(framebuf, 0, size);

                            bf.position(0);
                            bf.limit(size);

                            ByteBuffer inputBuffer = decoderInputBuffers[inputBufferIndex];
                            inputBuffer.clear();
                            inputBuffer.put(framebuf);
                            if (count == 3) {
                                codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, 0);
                                if (DEBUG) Log.d(TAG, "passed " + size + " bytes to decoder" + " with flags - " + 1);
                            } else {
                                codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, 0);
                                if (DEBUG) Log.d(TAG, "passed " + size + " bytes to decoder" + " with flags - " + 0);
                            }
                            framebuf=null;
                            if (closeWriter) {
                                //writer.close();
                            } else {
                                //writer.writeFrame(framebuf, size, 1);
                            }

                            count++;
                        }
                    } else {
                        if (closeWriter) {
                            writer.close();
                        }
                    }
                } else if(codecStatus == 2) { //空闲
                    try {
                        Thread.sleep(50);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

                int decoderStatus = codec.dequeueOutputBuffer(info, TIMEOUT_USEC);
                if (decoderStatus == MediaCodec.INFO_TRY_AGAIN_LATER) {
                    // no output available yet
                    if (DEBUG)
                        Log.d(TAG, "no output from decoder available");
                } else if (decoderStatus == MediaCodec.INFO_OUTPUT_BUFFERS_CHANGED) {
                    // The storage associated with the direct ByteBuffer may already be unmapped,
                    // so attempting to access data through the old output buffer array could
                    // lead to a native crash.
                    if (DEBUG)
                        Log.d(TAG, "decoder output buffers changed");
                    decoderOutputBuffers = codec.getOutputBuffers();
                } else if (decoderStatus == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    // this happens before the first frame is returned
                    decoderOutputFormat = codec.getOutputFormat();
                    if (DEBUG)
                        Log.d(TAG, "decoder output format changed: " + decoderOutputFormat);
                } else if (decoderStatus < 0) {
                    Log.e(TAG, "unexpected result from deocder.dequeueOutputBuffer: " + decoderStatus);
                } else {  // decoderStatus >= 0
                    ByteBuffer outputFrame = decoderOutputBuffers[decoderStatus];

                    if (!hasSurface) {
                        byte[] data = new byte[info.size];
                        outputFrame.get(data);
                        outputFrame.position(info.offset);

                        Log.i("WEI-->", "lastTime:" + " " + lasttime);
                        long outMs = System.currentTimeMillis();
                        long useTime = outMs - startMs;
                        lasttime = useTime;

                        while (true) {
                            Log.i(TAG, "Decoded, now convert format !!!");
                            JniNativesProxy.convertYUV420P2ARGB(brgb, data, mWidth, mHeight);
                            Log.i(TAG, "Decoded, now draw !!!");
                            renderFrame(mCurrentRenderer);
                            ByteBuffer byteBuffer = ByteBuffer.wrap(brgb);
                            surfaceBitmap.copyPixelsFromBuffer(byteBuffer);
                            //((MyActivity)obj).sv.draw();
                            if (getCaptureFrame()) {
                                captureBitmap = Bitmap.createBitmap(surfaceBitmap);
                            }

                            if (getCaptureFrame()) {
                                setCaptureFrame(false);
                                if(captureFrameListener != null)
                                    captureFrameListener.onCaptureFrame(captureBitmap);
                            }
                            break;
                        }
                    }

                    if (info.size == 0) {
                        if (DEBUG) Log.d(TAG, "got empty frame");
                    } else {
                        if (DEBUG) Log.d(TAG, "decoded, checking frame " + decodedframes++);
                    }

                    if ((info.flags & MediaCodec.BUFFER_FLAG_END_OF_STREAM) != 0) {
                        if (DEBUG) Log.d(TAG, "output EOS");
                        Log.i(TAG, "End of stream, exit decoder");
                        exitDecoder = true;
                    }

                    if (!hasSurface) {
                        codec.releaseOutputBuffer(decoderStatus, false /*render*/);
                    } else {
                        codec.releaseOutputBuffer(decoderStatus, true /*render*/);
                    }
                }
            } else { //初始化,等待extractor获取到sps，pps等
                isPrepared = mExtractor.isPrepared();
                if (!isPrepared) {
                    continue;
                }
                Log.i(TAG, "Already Prepared");

                String describe = mExtractor.getDescribe();
                if (describe != null && describe.length() > 1) {
                    Log.i(TAG, describe);
                    String[] tmpstr = describe.split(" ");
                    mWidth = Integer.parseInt(tmpstr[0].replace("width:", ""));
                    mHeight = Integer.parseInt(tmpstr[1].replace("height:", ""));
                    if(mWidth <= 0 || mHeight <=0)
                        continue;
                } else {
                    continue;
                }
                sps = mExtractor.getSps();
                sps_len = mExtractor.getSpsLength();
                pps = mExtractor.getPps();
                pps_len = mExtractor.getPpsLength();

                initMediaCodec(sps, sps_len, pps, pps_len);
                decodeThreadExited = false;
            }
            try {
                Thread.sleep(30);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        Log.i(TAG, "Decoder will exit");
        decoderconfigured = false;
        if(decodeThreadExited == false) {
            if (codec != null) {
                try {
                    codec.stop();
                    codec.release();
                    codec = null;
                } catch (Exception e) {
                    e.printStackTrace();
                    Log.e("PowerCodec", e.getMessage());
                }
            }

            if(sps != null) {
                sps = null;
            }
            if(pps != null) {
                pps = null;
            }
            decodeThreadExited = true;
        }
        Log.i(TAG, "Decoder exit");
    }

    public void setCaptureFrameListener(OnCaptureFrameListener listener) {
        this.captureFrameListener = listener;
    }

    public  Bitmap getFrameBitmap() {
        return surfaceBitmap;
    }

    public boolean getCaptureFrame() {
        return captureFrame;
    }

    public void setCaptureFrame(boolean isCap) {
        captureFrame = isCap;
    }

    public void setCloseWriter(boolean close) {
        closeWriter = close;
    }

    public SurfaceView getaSurfaceView() {
        return null;
    }

    @Override
    public void run() {
        Log.i(TAG, "lbg run decode thread");
        decode();
    }

    @Override
    public void onCaptureFrame(Bitmap bitmap) {
        capturer.setCaptureBitmap(bitmap);
        capturer.startCapture();
    }
}
