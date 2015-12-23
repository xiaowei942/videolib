package com.powervision.videolib.extractor;

import android.util.Log;
import com.powervision.videolib.jni.JniNatives;
import com.powervision.videolib.jni.JniNativesProxy;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class H264StreamFrameExtractor extends H264FrameExtractor {
    int mWidth = -1;
    int mHeight = -1;
    String serverIp = "192.168.42.202";
    String localIp = "192.168.42.129";
    int dataPort = 6007;
    byte sps[] = new byte[512];
    byte pps[] = new byte[128];

    private int nativeTransferObject;

    H264StreamFrameExtractor() {
        frame = new byte[5*1024*1024];
        init();
    }

    public int init() {
        nativeTransferObject = transferInit(mWidth, mHeight);
        if(open() < 0) {
            return -1;
        }
        return 0;
    }

    @Override
    public int open() {
        int ret = initSocket("192.168.42.202", "192.168.42.129", 6007);
        return 0;
    }

    @Override
    public void close() {
        JniNativesProxy.stopReceive(nativeTransferObject);
        JniNativesProxy.startProcess(nativeTransferObject);
        unInitSocket(6007);
        transferUnInit();
    }

    @Override
    public void start() {
        JniNativesProxy.startReceive(nativeTransferObject);
        JniNativesProxy.startProcess(nativeTransferObject);
    }

    @Override
    public int getFrameSize() {
        return frameSize;
    }

    @Override
    public byte[] getFrame() {
        Log.i("getFrame", "Enter");
        byte frame2[] = new byte[102400];
        int size = JniNativesProxy.getFrame(nativeTransferObject, frame2);
        Log.i("getFrame", "Leave with size: " + size);
        if(size <= 0) {
            return null;
        }
        frameSize = size;
        return frame2;
    }

    public void setSps(ByteBuffer sps) {
        super.setSps(sps);
    };

    public void setPps(ByteBuffer pps) {
        super.setPps(pps);
    }

    public void setSpsLength(int length) {
        super.setSpsLength(length);
    }

    public void setPpsLength(int length) {
        super.setPpsLength(length);
    }

    @Override
    public boolean isPrepared() {
        return JniNativesProxy.native_isPrepared(nativeTransferObject);
    }


    /************* Implements by native methods *************/

    private int transferInit(int width, int height) {
        return JniNativesProxy.getInstance().transferInit(width, height);
    }

    private void transferUnInit() {
        JniNativesProxy.getInstance().transferUnInit(nativeTransferObject);
    }

    public int initSocket(String serverIp, String localIp, int localPort) {
        return JniNativesProxy.initSocket(nativeTransferObject, serverIp, localIp, localPort);
    }

    public int unInitSocket(int port) {
        return JniNativesProxy.unInitSocket(nativeTransferObject, port);
    }

    @Override
    public ByteBuffer getSps() {
        return getSps(sps);
    }

    @Override
    public ByteBuffer getPps() {
        return getPps(pps);
    }

    public ByteBuffer getSps(byte[] sps) {
        int size = JniNativesProxy.getSps(nativeTransferObject, sps);
        setSpsLength(size);

        if(size != 0) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(sps, 0, size);
            return byteBuffer;
        }
        return null;
    }

    public ByteBuffer getPps(byte[] pps) {
        int size = JniNativesProxy.getPps(nativeTransferObject, pps);
        setPpsLength(size);

        if(size != 0) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(pps, 0, size);
            return byteBuffer;
        }
        return null;
    }
}
