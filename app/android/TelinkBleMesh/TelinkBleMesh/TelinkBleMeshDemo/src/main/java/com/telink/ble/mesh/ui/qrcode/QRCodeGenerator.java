/********************************************************************************************************
 * @file QRCodeGenerator.java
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
import android.os.AsyncTask;
import android.os.Handler;

import com.google.zxing.WriterException;
import com.google.zxing.qrcode.decoder.ErrorCorrectionLevel;
import com.telink.ble.mesh.util.MeshLogger;

/**
 * QRCode generator
 * Created by kee on 2016/9/14.
 */
public class QRCodeGenerator extends AsyncTask<Void, Void, Bitmap> {
    private QREncoder mEncoder;
    private Bitmap mResult;
    // qr-code generate success
    public final static int QRCode_Generator_Success = 1;
    // generate fail
    public final static int QRCode_Generator_Fail = 2;
    private Handler mHandler;
    private int size;
    private String data;

    public QRCodeGenerator(Handler handler, int size, String data) {
        super();
        mResult = null;
        this.mHandler = handler;
        this.size = size;
        this.data = data;
        initEncoder();
    }

    private void initEncoder() {
        QREncoder.Builder builder = new QREncoder.Builder();
        builder.setBackground(0xFFFFFFFF);
        builder.setCodeColor(0xFF000000);
        builder.setCharset(GZIP.GZIP_ENCODE);
        builder.setWidth(size);
        builder.setHeight(size);
//        errorDialogBuilder.setPadding(2);
        builder.setLevel(ErrorCorrectionLevel.L);
        mEncoder = builder.build();

    }

    public Bitmap getResult() {
        return mResult;
    }

    public void clear() {
        this.mResult = null;
    }

    @Override
    protected Bitmap doInBackground(Void... params) {
        String src = this.data;
        MeshLogger.w("raw: " + src + " -- " + src.getBytes().length + " bytes");
        /*src = GZIP.compressed(src);
        try {
            src = GZIP.bytesToHexString(src.getBytes(GZIP.GZIP_ENCODE));
            MeshLogger.w("compressed: " + src + "  -- " + src.getBytes(GZIP.GZIP_ENCODE).length + " bytes");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            mHandler.sendEmptyMessage(QRCode_Generator_Fail);
            return null;
        }*/
        try {
            mResult = mEncoder.encode(src);
        } catch (WriterException e) {
            e.printStackTrace();
            mHandler.sendEmptyMessage(QRCode_Generator_Fail);
        }
        return mResult;
    }


    @Override
    protected void onPostExecute(Bitmap bitmap) {
        super.onPostExecute(bitmap);
        this.mResult = bitmap;
        mHandler.sendEmptyMessage(QRCode_Generator_Success);
    }
}
