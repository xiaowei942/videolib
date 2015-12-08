package com.powervision.videolib.extractor;

import com.powervision.videolib.jni.JniNatives;
import com.powervision.videolib.jni.JniNativesProxy;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class H264StreamFrameExtractor extends H264FrameExtractor {
    private ByteBuffer sps = null;
    private ByteBuffer pps = null;
    int mWidth = -1;
    int mHeight = -1;
    String serverIp = "192.168.42.202";
    String localIp = "192.168.42.129";
    int dataPort = 6007;

    private int nativeTransferObject;

    H264StreamFrameExtractor() {
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
        return 0;
    }

    @Override
    public void close() {

    }

    @Override
    public void start() {

    }

    @Override
    public byte[] getFrame() {
        return new byte[0];
    }
    public void setSps(ByteBuffer sps) {
        super.setSps(sps);
    };

    public void setPps(ByteBuffer pps) {
        super.setPps(pps);
    };

    public void setSpsLength(int length) {
        super.setSpsLength(length);
    };

    public void setPpsLength(int length) {
        super.setPpsLength(length);
    };


    /************* Implements by native methods *************/

    private int transferInit(int width, int height) {
        return JniNativesProxy.getInstance().transferInit(width, height);
    }

    public int initSocket(String serverIp, String localIp, int localPort) {
        return JniNativesProxy.initSocket(nativeTransferObject, serverIp, localIp, localPort);
    }

    public ByteBuffer getSps(byte[] sps) {
        int size = JniNativesProxy.getSps(sps);

        if(size != 0) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(sps);
            return byteBuffer;
        }
        return null;
    }

    public ByteBuffer getPps(byte[] pps) {
        int size = JniNativesProxy.getPps(pps);

        if(size != 0) {
            ByteBuffer byteBuffer = ByteBuffer.wrap(pps);
            return byteBuffer;
        }
        return null;
    }
    public int getSpsLength() {
        return JniNativesProxy.getSpsLength();
    };
    public int getPpsLength() {
        return JniNativesProxy.getPpsLength();
    };
}
