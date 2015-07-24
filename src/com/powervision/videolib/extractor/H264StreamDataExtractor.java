package com.powervision.videolib.extractor;

import android.provider.ContactsContract;

import java.nio.ByteBuffer;

/**
 * Created by liwei on 15-7-24.
 */
public class H264StreamDataExtractor implements DataExtractor {
    @Override
    public byte[] getFrame() {
        return new byte[0];
    }

    @Override
    public boolean openDataExtractor() {
        return false;
    }

    @Override
    public void closeDataExtractor() {

    }

    @Override
    public void start() {

    }

    @Override
    public int getStatus() {
        return 0;
    }

    @Override
    public ByteBuffer getSps() {
        return null;
    }

    @Override
    public ByteBuffer getPps() {
        return null;
    }
}
