/********************************************************************************************************
 * @file QREncoder.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
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
