package com.powervision.videolib.capture;

import android.graphics.Bitmap;
import android.os.Environment;
import android.text.format.Time;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by liwei on 15-7-24.
 */
public class CaptureFrame {

    int pictureQuality = 100;
    String filePath = null;
    Bitmap captureBitmap = null;
    Bitmap.CompressFormat pictureFormat = Bitmap.CompressFormat.JPEG;

    CaptureFrame(Bitmap bitmap) {
        String path = makeCaptureFileName();
        setFilePath(path);
        setCaptureBitmap(bitmap);
    }

    CaptureFrame(String name, Bitmap bitmap, int format, int quality) {
        setFilePath(name);
        setCaptureBitmap(bitmap);
        setPictureQuality(quality);
    }

    public boolean setFilePath(String path) {
        if(path == null) {
            return false;
        } else {
            filePath = path;
            return true;
        }
    }

    public boolean setPictureQuality(int quality) {
        if(quality > 0 && quality <= 100) {
            pictureQuality = quality;
            return true;
        } else {
            return false;
        }
    }

    public boolean setCaptureBitmap(Bitmap bitmap) {
        if(bitmap != null) {
            captureBitmap = bitmap;
            return true;
        } else {
            return false;
        }
    }

    public void setFormat(int format) {
        if(format == 0) {
            pictureFormat = Bitmap.CompressFormat.JPEG;
        } else if(format == 1) {
            pictureFormat = Bitmap.CompressFormat.PNG;
        }
    }

    private String makeCaptureFileName() {
        Time time = new Time();
        time.setToNow();
        return new String(Environment.getExternalStorageDirectory() + "/" + time.year + "-" + time.month + "-" + time.monthDay + "-" + time.hour + "-" + time.minute + "-" + time.second + ".jpg");
    }

    public class Capture implements Runnable {
        @Override
        public void run() {
            do {
                try {
                    saveBitmapToFile(filePath, captureBitmap, pictureQuality);
                    Thread.sleep(100);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }while(false);
        }
    }


    private void saveBitmapToFile(String bitName, Bitmap bitmap, int quality) throws IOException {
        File f = new File(bitName);
        f.createNewFile();
        FileOutputStream fOut = null;

        try {
            fOut = new FileOutputStream(f);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        bitmap.compress(pictureFormat, quality, fOut);
        try {
            fOut.flush();
            fOut.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
