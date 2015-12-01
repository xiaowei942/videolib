package com.powervision.videolib.extractor;

import android.os.Environment;
import android.util.Log;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by liwei on 15-7-24.
 */
public class H264FileFrameExtractor extends H264FrameExtractor implements Runnable {
    //For debug
    private static final boolean DEBUG = false;
    private static String TAG = null;

    //Video source file location.
    private static final String DEFAULT_DATA_FILE_PATH = Environment.getExternalStorageDirectory() + "/" + "default.h264";

    //Customized file path
    private String filePath;

    //Video input dependent stream
    private FileInputStream fis = null;

    //Length of the file
    private int fileLength = -1;

    //Maximium stream length
    private static final int MAX_SISE = 20*1024*1024;

    //Save the file stream data
    private byte[] bytes = new byte[MAX_SISE];

    private int status;

    private int decodeNaluIndex = 2;
    private static List<Integer> naluList = new ArrayList<Integer>();

    //Constructor
    public H264FileFrameExtractor() {
        TAG = getClass().getSimpleName();
    }

    //Constructor
    public H264FileFrameExtractor(String path) {
        TAG = getClass().getSimpleName();
        if(path == null) {
            filePath = DEFAULT_DATA_FILE_PATH;
        } else {
            filePath = path;
        }
    }

    @Override
    public byte[] getFrame() {
        int start = naluList.get(decodeNaluIndex);
        int count = naluList.get(decodeNaluIndex+1)-naluList.get(decodeNaluIndex);

        byte[] temp = new byte[count];
        System.arraycopy(bytes, start, temp, 0, count);

        if(decodeNaluIndex==naluList.size()-2) {
            //循环发送数据
            decodeNaluIndex = 2;
        } else {
            decodeNaluIndex++;
        }

        if(DEBUG) {
            Log.i(TAG, "*************************** START *******************************");
            for(int i=0; i<count; i++) {
                Log.i(TAG, "0x" + Integer.toHexString(temp[i]) + " ");
            }
            Log.i(TAG, "***************************  END  *******************************");
        }
        return temp;
    }

    public int open() {
        if(filePath != null) {
            int ret = openFile(filePath);
            return ret;
        } else {
            int ret = openFile(DEFAULT_DATA_FILE_PATH);
            return ret;
        }
    }

    @Override
    public void close() {
        closeFile(fis);
    }

    @Override
    public void start() {
        this.run();
    }

    //Open raw video file
    private int openFile(String file) {
        try {
            fis = new FileInputStream("/mnt/sdcard/test.h264");//file);
            return 0;
        } catch (FileNotFoundException e) {
            Log.e(TAG, "Cannot open input file stream.");
            e.printStackTrace();
            return -1;
        }
    }

    //Close
    private boolean closeFile(FileInputStream fis) {
        if(fis != null) {
            try {
                fis.close();
            } catch (IOException e) {
                Log.e(TAG, "Cannot close input file stream.");
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }

    //Get data from video file stream in bytes.
    private int getData(FileInputStream fis) {
        int count = 0;
        try {
            count = fis.read(bytes);
            return count;
        } catch (IOException e) {
            Log.e(TAG, "Cannot get data from file.");
            e.printStackTrace();
            return -1;
        }
    }

    private int getNaluList(byte[] src) {
        for(int i=0; i<fileLength; i++) {
            if( (src[i] == 0x0) && (src[i+1] == 0x0)) {
                if(src[i+2] == 0x01) {

                }else if((src[i+2] == 0x0) && (src[i+3] == 0x01)) {
                    naluList.add(i);
                    i+=3;
                }
            }
        }
        naluList.add(fileLength);

        if(DEBUG) {
            for(int count=0; count<naluList.size(); count++) {
                Log.i(TAG, "Count: " + count + "  Value: " + naluList.get(count).toString());
            }
        }

        return naluList.size();
    }

    private boolean hasSpsPps() {
        boolean hasSps = false;
        boolean hasPps = false;

        int index = naluList.get(0);
        for(int i=0; i<5; i++) {
            int temp = bytes[index+i];
            if((temp & 0x0f) == 0x07) {
                hasSps = true;
                int spsLength = naluList.get(1)-naluList.get(0);
                setSpsLength(spsLength);
                ByteBuffer sps = ByteBuffer.wrap(bytes, naluList.get(0), spsLength);
                setSps(sps);
                break;
            }
        }

        if(hasSps) {
            index = naluList.get(1);
            for(int j=0; j<5; j++) {
                int temp = bytes[index+j];
                if((temp & 0x0f) == 0x08) {
                    hasPps = true;
                    int ppsLength = naluList.get(2)-naluList.get(1);
                    setPpsLength(ppsLength);
                    ByteBuffer Pps = ByteBuffer.wrap(bytes, naluList.get(1), ppsLength);
                    setPps(Pps);
                    return true;
                }
            }
        }

        return false;
    }

    @Override
    public void run() {
        fileLength = getData(fis);
        if( fileLength == -1) {
            status = EXTRACTOR_STATUS_FAILED;
            return;
        };

        if(getNaluList(bytes)<=0) {
            status = EXTRACTOR_STATUS_FAILED;
            return;
        }

        if(!hasSpsPps()) {
            status = EXTRACTOR_STATUS_FAILED;
            return;
        }

        status = EXTRACTOR_STATUS_OK;
    }
}
