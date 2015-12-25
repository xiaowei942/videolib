package com.powervision.videolib.codec;

import android.graphics.Bitmap;
import android.media.MediaCodec;
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
    private String TAG = null;
    private static final boolean DEBUG = true;
    //视频截图模块
    OnCaptureFrameListener captureFrameListener = null;
    private FrameCapturer capturer = null;
    static Bitmap captureBitmap = null;
    private boolean captureFrame = false;

    //数据获取模块
    private H264FrameExtractor extractor = null;

    //视频文件存储模块
    private FileWriter writer = null;
    static boolean closeWriter = false;

    //图像渲染模块
    private IRenderer renderer = null;
    static Surface mSurface = null;
    static boolean hasSurface = false;
    static H264FrameExtractor mExtractor = null;

    static Bitmap surfaceBitmap = null;
    private static byte[] brgb = null;

    //解码器部分
    MediaCodec codec = null;
    MediaCodec.BufferInfo info;
    private int decodedframes = 0;
    private boolean decoderconfigured = false;
    ByteBuffer[] decoderInputBuffers = null;
    ByteBuffer[] decoderOutputBuffers = null;
    MediaFormat decoderOutputFormat = null;
    private static final String MIME_TYPE = "video/avc";
    private static final long TIMEOUT_USEC = 1000;

    boolean inputDone = false;
    boolean encoderDone = false;
    boolean exitDecoder = false;

    void registerAllRenderers() {
        mCurrentRenderer = RendererFactory.createSurfaceViewRenderer(mSurfaceView);
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

        if(mCurrentRenderer.getType() == Renderer.RendererType.RendererType_SurfaceView) {
            // Initialize the bitmap, with the replaced color
            surfaceBitmap = Bitmap.createBitmap(mWidth, mHeight,
                    Bitmap.Config.ARGB_8888);
            /* We cannot use following way to create a bitmap, or error occurs when setPixels
                surfaceBitmap = Bitmap.createBitmap(irgb, mWidth, mHeight,
                        Bitmap.Config.ARGB_8888);
            */
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
        MediaFormat format = MediaFormat.createVideoFormat(MIME_TYPE, getWidth(), getHeight());

        format.setByteBuffer("csd-0", sps);
        format.setByteBuffer("csd-1", pps);
        //format.setInteger("color-format", 19);
        codec.configure(format, mSurface, null, 0);
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

    }

    @Override
    public void start() {
        new Thread(this).start();
    }

    @Override
    public void stop() {
        exitDecoder = true;
    }

    @Override
    public void closeCodec() {

    }

    @Override
    public void releaseCodec() {

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
                if (!inputDone) {
                    byte[] buf = mExtractor.getFrame();
                    if (buf == null) {
                        continue;
                    }

                    size = buf.length;

                    if (size > 0) {
                        inputBufferIndex = codec.dequeueInputBuffer(-1);
                        if (inputBufferIndex >= 0 ) {
                            if ( buf==null) {
                            count++;
                                continue;
                            }

                            ByteBuffer bf = ByteBuffer.wrap(buf, 0, size);

                            bf.position(0);
                            bf.limit(size);

                            ByteBuffer inputBuffer = decoderInputBuffers[inputBufferIndex];
                            inputBuffer.clear();
                            inputBuffer.put(buf);
                            if (count == 3) {
                                codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, 0);
                                if (DEBUG) Log.d(TAG, "passed " + size + " bytes to decoder" + " with flags - " + 1);
                            } else {
                                codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, 0);
                                if (DEBUG) Log.d(TAG, "passed " + size + " bytes to decoder" + " with flags - " + 0);
                            }

                            if (closeWriter) {
                                //writer.close();
                            } else {
                                //writer.writeFrame(buf, size, 1);
                            }

                            count++;
                        }
                    } else {

                        if (closeWriter) {
                            writer.close();
                        }
                    }
                } else {
                    inputDone = true;
                    inputBufferIndex = codec.dequeueInputBuffer(TIMEOUT_USEC);
                    if (inputBufferIndex >= 0) {
                        codec.queueInputBuffer(inputBufferIndex, 0, size, count * 1000, MediaCodec.BUFFER_FLAG_END_OF_STREAM);

                        if (DEBUG)
                            Log.d(TAG, "passed " + size + " bytes to decoder" + " with flags - " + MediaCodec.BUFFER_FLAG_END_OF_STREAM
                                    + (encoderDone ? " (EOS)" : ""));
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
                        exitDecoder = true;
                    }

                    if (!hasSurface) {
                        codec.releaseOutputBuffer(decoderStatus, false /*render*/);
                    } else {
                        codec.releaseOutputBuffer(decoderStatus, true /*render*/);
                    }
                }
            } else { //初始化,等待extractor获取到sps，pps等
                boolean isPrepared = mExtractor.isPrepared();
                while (!isPrepared) {
                    continue;
                }

                String describe=mExtractor.getDescribe();
                if(describe!=null && describe.length()>1) {
                    String[] tmpstr = describe.split(" ");
                    mWidth = Integer.parseInt(tmpstr[0].replace("width:", ""));
                    mHeight = Integer.parseInt(tmpstr[1].replace("height:", ""));
                }
                ByteBuffer sps = mExtractor.getSps();
                int sps_len = mExtractor.getSpsLength();
                ByteBuffer pps = mExtractor.getPps();
                int pps_len = mExtractor.getPpsLength();

                initMediaCodec(sps, sps_len, pps, pps_len);
            }
        }
    }

    public void setCaptureFrameListener(OnCaptureFrameListener listener) {
        this.captureFrameListener = listener;
    }

    public static Bitmap getFrameBitmap() {
        return surfaceBitmap;
    }

    public boolean getCaptureFrame() {
        return captureFrame;
    }

    public void setCaptureFrame(boolean isCap) {
        captureFrame = isCap;
    }

    public static void setCloseWriter(boolean close) {
        closeWriter = close;
    }

    public SurfaceView getmSurfaceView() {
        return null;
    }

    @Override
    public void run() {
        decode();
    }

    @Override
    public void onCaptureFrame(Bitmap bitmap) {
        capturer.setCaptureBitmap(bitmap);
        capturer.startCapture();
    }
}
