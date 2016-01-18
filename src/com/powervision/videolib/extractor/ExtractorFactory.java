package com.powervision.videolib.extractor;

import android.util.Log;

/**
 * Created by liwei on 15-7-28.
 */
public class ExtractorFactory {
    public  H264FrameExtractor createFileDataExtractor(String path) {
        return new H264FileFrameExtractor(path);
    };

    public  H264FrameExtractor createStreamDataExtractor() {
        Log.i("TRANSFER", "createStreamDataExtractor");
        return new H264StreamFrameExtractor();
    };
}