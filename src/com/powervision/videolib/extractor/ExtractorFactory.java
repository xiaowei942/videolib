package com.powervision.videolib.extractor;

/**
 * Created by liwei on 15-7-28.
 */
public class ExtractorFactory {
    public static H264FrameExtractor createFileDataExtractor(String path) {
        return new H264FileFrameExtractor(path);
    };

    public static H264FrameExtractor createStreamDataExtractor() {
        return new H264StreamFrameExtractor();
    };
}