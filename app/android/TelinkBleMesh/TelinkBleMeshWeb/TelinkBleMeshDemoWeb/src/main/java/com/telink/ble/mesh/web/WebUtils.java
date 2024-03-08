package com.telink.ble.mesh.web;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import okhttp3.Response;
import okhttp3.ResponseBody;

public class WebUtils {

    public static final int VERIFICATION_USAGE_REGISTER = 0x01;

    public static final int VERIFICATION_USAGE_RESET_PASSWORD = 0x02;

    public static ResponseInfo parseResponse(Response response, BaseActivity activity, boolean consumeCodeError) throws IOException {
        ResponseBody body = response.body();
        if (body == null) {
            MeshLogger.d("body null error");
            return null;
        }
        String rspStr = body.string();
        MeshLogger.d("body string - " + rspStr);
        try {
            ResponseInfo rspInfo = JSON.parseObject(rspStr, ResponseInfo.class);
            if (rspInfo == null) {
                return null;
            }
            if (!rspInfo.isSuccess()) {
                if (activity != null) {
                    activity.showError(rspInfo.message);
                    activity.validateWebAuth(rspInfo);
                }
                if (consumeCodeError) {
                    return null;
                }
            }
            if (activity != null) {
                activity.showSuccess(rspInfo.message);
            }
            return rspInfo;
        } catch (JSONException e) {
            e.getStackTrace();
            return null;
        }
    }


    public static ResponseInfo parseResponse(Response response, BaseActivity activity) throws IOException {
        return parseResponse(response, activity, true);
    }

    public static String sha256(String input) {
        try {
            MeshLogger.d("sha256 - " + input);
            MessageDigest md = MessageDigest.getInstance("SHA-256");
            byte[] hash = md.digest(input.getBytes("UTF-8"));
            // convert hash to hex string
            MeshLogger.d("sha256 out - " + Arrays.bytesToHexString(hash));
            return Arrays.bytesToHexString(hash);
        } catch (NoSuchAlgorithmException | UnsupportedEncodingException e) {
            e.printStackTrace();
            return input;
        }
    }
}
