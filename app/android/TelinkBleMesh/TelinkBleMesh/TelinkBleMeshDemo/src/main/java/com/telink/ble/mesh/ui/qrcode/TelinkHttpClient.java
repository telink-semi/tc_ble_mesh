/********************************************************************************************************
 * @file     TelinkHttpClient.java 
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

import com.telink.ble.mesh.util.MeshLogger;

import java.util.concurrent.TimeUnit;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;

public class TelinkHttpClient {
    private static TelinkHttpClient mHttpclient = new TelinkHttpClient();

    //    public final static String URL_BASE = "http://192.168.18.59:8080/";
    public final static String URL_BASE = "http://47.115.40.63:8080/";

    public final static String URL_UPLOAD = URL_BASE + "upload";

    public final static String URL_DOWNLOAD = URL_BASE + "download";

    private final OkHttpClient client;

    private TelinkHttpClient() {
        client = new OkHttpClient.Builder()
                .connectTimeout(10, TimeUnit.SECONDS)
                .readTimeout(10, TimeUnit.SECONDS)
                .writeTimeout(10, TimeUnit.SECONDS)
                .build();
    }

    public static TelinkHttpClient getInstance() {
        return mHttpclient;
    }

    public Call upload(String meshJson, long timeoutSeconds, Callback callback) {

        FormBody formBody = new FormBody.Builder()
                .add("data", meshJson)
                .add("timeout", timeoutSeconds + "")
                .build();
        return sentRequest(URL_UPLOAD, formBody, "upload", callback);
    }

    public Call download(String uuid, Callback callback) {
        FormBody formBody = new FormBody.Builder()
                .add("uuid", uuid).build();
        return sentRequest(URL_DOWNLOAD, formBody, "download", callback);
    }

    public Call sentRequest(String url, FormBody formBody, Object tag, Callback callback) {
        MeshLogger.d("send request: " + url);
        Request request = new Request.Builder()
                .url(url).tag(tag)
                .post(formBody)
                .build();
        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }
}
