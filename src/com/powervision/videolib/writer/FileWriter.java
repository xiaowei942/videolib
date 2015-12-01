package com.powervision.videolib.writer;

import android.util.Log;
import com.powervision.videolib.jni.JniNativesProxy;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by liwei on 15-7-24.
 */
public abstract class FileWriter extends Writer implements IWriter {
    String TAG = this.getClass().getSimpleName();
    private String filePath = null;
    private FileOutputStream outputStream = null;

    @Override
    public int open() {
        try {
            if(filePath == null) {
                Log.e(TAG, "Cannot open output stream, filePath is null.");
                return -1;
            }
            outputStream = new FileOutputStream(filePath);
        } catch (FileNotFoundException e) {
            Log.e(TAG, "Cannot open output stream: " + filePath);
            e.printStackTrace();
        }
        return 0;
    }

    @Override
    public void close() {
        if( outputStream!= null) {
            try {
                outputStream.close();
            } catch (IOException e) {
                Log.e(TAG, "Cannot close output stream.");
                e.printStackTrace();
            }
        }
    }
}
