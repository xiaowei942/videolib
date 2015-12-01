package com.powervision.videolib.writer;

import android.os.Environment;
import com.powervision.videolib.codec.BaseCodec;
import com.powervision.videolib.codec.ICodec;
import com.powervision.videolib.jni.JniNativesProxy;

/**
 * Created by liwei on 15-7-27.
 */
public class Mp4FileWriter extends FileWriter {
    String mFileName = null;
    int mWidth = -1;
    int mHeight = -1;
    int mFrameRate = -1;

    private int nativeWriterObject;
    public static final String MP4_FILE_PATH = Environment.getExternalStorageDirectory() + "/";
    public static final String DEFAULT_MP4_FILE_PATH = Environment.getExternalStorageDirectory() + "/" + "default.mp4";

    Mp4FileWriter(String filename, int width, int height, int fps) {
        mFileName = MP4_FILE_PATH + filename;
        mWidth = width;
        mHeight = height;
        mFrameRate = fps;
        init();
    }

    Mp4FileWriter(ICodec codec, String filename) {
        if(filename == null) {
            mFileName = DEFAULT_MP4_FILE_PATH;
        } else {
            mFileName = MP4_FILE_PATH + filename;
        }

        mWidth = ((BaseCodec)codec).getWidth();
        mHeight = ((BaseCodec)codec).getHeight();
        mFrameRate = ((BaseCodec)codec).getFps();
        init();
    }

    public int init() {
        nativeWriterObject = writerInit(mWidth, mHeight);
        setFileName(mFileName);
        setFps(mFrameRate);
        if(open() < 0) {
            return -1;
        }
        return 0;
    }

    @Override
    public int open() {
        startRecord();
        return 0;
    }

    @Override
    public void close() {
        stopRecord();
    }

    public int writerInit(int width, int height) {
        int ret = JniNativesProxy.getInstance().writerInit(mWidth, mHeight);
        return ret;
    };

    public void setFileName(String fileName) {
        JniNativesProxy.getInstance().setFileName(nativeWriterObject, fileName);
    };

    public void setFps(int fps) {
        JniNativesProxy.getInstance().setFps(nativeWriterObject, fps);
    };

    public void startRecord() {
        JniNativesProxy.getInstance().startRecord(nativeWriterObject);
    };

    public void stopRecord() {
        JniNativesProxy.getInstance().stopRecord(nativeWriterObject);
    };

    public void writeFrame(byte[] data, long size, long ts) {
        JniNativesProxy.getInstance().writeFrame(nativeWriterObject, data, size, ts);
    }
}
