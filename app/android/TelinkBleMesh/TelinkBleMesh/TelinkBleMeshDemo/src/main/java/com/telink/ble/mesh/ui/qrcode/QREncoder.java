/********************************************************************************************************
 * @file     QREncoder.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.ui.qrcode;

import android.graphics.Bitmap;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.EncodeHintType;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import com.google.zxing.qrcode.QRCodeWriter;
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel;

import java.util.EnumMap;
import java.util.Map;

public final class QREncoder {

    private Builder mBuilder;
    private QRCodeWriter mWriter;

    private QREncoder(Builder builder) {
        this.mBuilder = builder;
        this.mWriter = new QRCodeWriter();
    }

    public Bitmap encode(String contents) throws WriterException {
        BitMatrix result = this.mWriter.encode(contents, BarcodeFormat.QR_CODE, this.mBuilder.width, this.mBuilder.height, this.mBuilder.hints);
        int width = result.getWidth();
        int height = result.getHeight();
        int[] pixels = new int[width * height];
        int offset;

        for (int y = 0; y < height; y++) {
            offset = y * width;
            for (int x = 0; x < width; x++) {
                pixels[offset + x] = result.get(x, y) ? this.mBuilder.codeColor : this.mBuilder.background;
            }
        }

        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.setPixels(pixels, 0, width, 0, 0, width, height);

        return bitmap;
    }

    public static class Builder {

        private Map<EncodeHintType, Object> hints;
        private ErrorCorrectionLevel correctionLevel = ErrorCorrectionLevel.L;
        private int padding = 0;
        private String charset = "UTF-8";
        private int background = 0xFFFFFFFF;
        private int codeColor = 0xFF000000;
        private int width;
        private int height;

        public Builder setCharset(String charset) {
            if (charset == null || charset.trim().isEmpty())
                charset = "UTF-8";
            this.charset = charset;
            return this;
        }

        public Builder setBackground(int background) {
            this.background = background;
            return this;
        }

        public Builder setCodeColor(int codeColor) {
            this.codeColor = codeColor;
            return this;
        }

        public Builder setPadding(int padding) {
            if (padding < 0)
                padding = 0;
            this.padding = padding;
            return this;
        }

        public Builder setWidth(int width) {
            this.width = width;
            return this;
        }

        public Builder setHeight(int height) {
            this.height = height;
            return this;
        }

        public Builder setLevel(ErrorCorrectionLevel level) {
            this.correctionLevel = level;
            return this;
        }

        private Map<EncodeHintType, Object> buildHints() {
            Map<EncodeHintType, Object> hints = new EnumMap<>(EncodeHintType.class);
            //SymbolShapeHint
            hints.put(EncodeHintType.CHARACTER_SET, this.charset);
            hints.put(EncodeHintType.MARGIN, this.padding);
            hints.put(EncodeHintType.ERROR_CORRECTION, this.correctionLevel);
            return hints;
        }

        public QREncoder build() {
            this.hints = this.buildHints();
            return new QREncoder(this);
        }
    }
}
