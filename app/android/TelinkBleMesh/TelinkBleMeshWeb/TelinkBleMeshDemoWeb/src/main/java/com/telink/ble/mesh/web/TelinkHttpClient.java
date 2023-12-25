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
package com.telink.ble.mesh.web;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.ui.qrcode.HttpRequestMethod;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.InputStream;
import java.net.CookieManager;
import java.security.KeyStore;
import java.security.SecureRandom;
import java.security.cert.CertificateFactory;
import java.util.Map;
import java.util.concurrent.TimeUnit;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.JavaNetCookieJar;
import okhttp3.OkHttpClient;
import okhttp3.Request;

public class TelinkHttpClient {
    private static TelinkHttpClient mHttpclient = new TelinkHttpClient();

//    public final static String URL_BASE = "https://192.168.18.149:8081/";
    public final static String URL_BASE = "http://192.168.18.87:8081/";
//    public final static String URL_BASE = "http://47.115.40.63:8080/";

    public final static String URL_UPLOAD = URL_BASE + "upload";


    private final OkHttpClient client;

    private TelinkHttpClient() {
        SSLSocketFactory sslSocketFactory = null;
        try {
            CertificateFactory certificateFactory = CertificateFactory.getInstance("X.509");
            KeyStore keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
            keyStore.load(null);
            InputStream certificate = TelinkMeshApplication.getInstance().getAssets().open("client_key.cer");
            keyStore.setCertificateEntry("certificateAlias - 0", certificateFactory.generateCertificate(certificate));
            SSLContext sslContext = SSLContext.getInstance("TLS");
            TrustManagerFactory trustManagerFactory =
                    TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());

            trustManagerFactory.init(keyStore);
            sslContext.init(
                    null,
                    trustManagerFactory.getTrustManagers(),
                    new SecureRandom());
            sslSocketFactory = sslContext.getSocketFactory();
        } catch (Exception e) {
            e.printStackTrace();
        }

        client = new OkHttpClient.Builder()
                .hostnameVerifier((hostname, session) -> true)
                .sslSocketFactory(sslSocketFactory)
                .cookieJar(new JavaNetCookieJar(new CookieManager()))
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


    /**************************************************************************************
     * send request
     **************************************************************************************/

    public Call get(String url, Map<String, String> params, Callback cb) {
        String getUrl = rebuildUrl(url, params);
        return get(getUrl, cb);
    }

    public Call get(String url, Callback cb) {
        Request request = new Request.Builder()
                .url(url)
                .get().build();
        return sendRequest(request, cb);
    }

    public Call post(String url, FormBody formBody, Callback cb) {
        Request request = new Request.Builder()
                .url(url)
                .post(formBody)
                .build();
        return sendRequest(request, cb);
    }


    public Call sendRequest(Request request, Callback callback) {
        MeshLogger.d("sendRequest - " + request.method() + " -- " + request.url());
        Call call = client.newCall(request);
        call.enqueue(callback);
        return call;
    }


    /**
     * @deprecated
     */
    public Call sendRequest(String url, Map<String, String> params, HttpRequestMethod
            method, Callback callback) {
        switch (method) {
            case GET:
                return get(url, params, callback);
            case POST:

        }

        return null;
    }


    public static String rebuildUrl(String url, Map<String, String> params) {
        if (params == null) {
            return url;
        }
        StringBuilder sb = new StringBuilder("?");
        for (String key : params.keySet()) {
            sb.append(key).append("=").append(params.get(key)).append("&");
        }
//        sb.delete(sb.length() - 1, sb.length());
        return url + sb.toString();
    }


}
