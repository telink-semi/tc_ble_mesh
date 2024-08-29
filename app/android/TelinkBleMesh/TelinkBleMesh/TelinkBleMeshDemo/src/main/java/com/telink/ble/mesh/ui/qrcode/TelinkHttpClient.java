/********************************************************************************************************
 * @file TelinkHttpClient.java
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
//    public static String URL_BASE = "http://47.115.40.63:8080/";

    public final static String URL_UPLOAD = "upload";

    public final static String URL_DOWNLOAD = "download";

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

    public Call upload(String baseUrl, String meshJson, long timeoutSeconds, Callback callback) {
        String url = baseUrl + URL_UPLOAD;
        FormBody formBody = new FormBody.Builder()
                .add("data", meshJson)
                .add("timeout", timeoutSeconds + "")
                .build();
        return sentRequest(url, formBody, "upload", callback);
    }

    public Call download(String baseUrl, String uuid, Callback callback) {
        String url = baseUrl + URL_DOWNLOAD;
        FormBody formBody = new FormBody.Builder()
                .add("uuid", uuid).build();
        return sentRequest(url, formBody, "download", callback);
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
