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
//        JniNativesProxy.startProcess(nativeTransferObject);
        //unInitSocket(6007);
        transferUnInit();
    }

    @Override
    public void start() {
        JniNativesProxy.startReceive(nativeTransferObject);
        //JniNativesProxy.startProcess(nativeTransferObject);
    }

    @Override
    public int getFrameSize() {
        return frameSize;
    }

    @Override
    public byte[] getFrame() {
//        Log.i("getFrame", "Enter");
//        int size = JniNativesProxy.getFrame(nativeTransferObject, frame);
//        Log.i("getFrame", "Leave with size: " + size);
//        if(size <= 0) {
//            return null;
//        }
//        frameSize = size;
//        return frame;

       return JniNativesProxy.getFrame(nativeTransferObject);
    }

    @Override
    public String getDescribe() {
        return JniNativesProxy.getDescribe(nativeTransferObject);
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

        sps= JniNativesProxy.getSps(nativeTransferObject, sps);
        int size = sps.length;
        setSpsLength(size);

        if(size > 0 && size<sps.length*2) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(sps, 0, size);
            return byteBuffer;
        }
        return null;
    }

    public ByteBuffer getPps(byte[] pps) {
       pps= JniNativesProxy.getPps(nativeTransferObject, pps);
        int size=pps.length;
        setPpsLength(size);

        if(size>0 && size<pps.length*2) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(pps, 0, size);
            return byteBuffer;
        }
        return null;
    }
}
