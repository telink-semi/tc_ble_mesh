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

    public final static String URL_BASE = "http://192.168.18.59:8080/";

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
