package com.powervision.videolib.writer;

import com.powervision.videolib.codec.ICodec;

/**
 * Created by liwei on 15-7-28.
 */
public class FileWriterFactory {
    public static FileWriter createMp4FileWriter(ICodec codec, String filename) {
        return new Mp4FileWriter(codec, filename);
    }
}
