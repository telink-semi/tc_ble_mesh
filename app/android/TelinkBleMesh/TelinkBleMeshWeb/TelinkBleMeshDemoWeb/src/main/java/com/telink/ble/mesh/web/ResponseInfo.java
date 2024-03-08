package com.telink.ble.mesh.web;


public class ResponseInfo {
    /**
     * response code
     */
    public int code;

    public String message;

    public String data = null;

    @Override
    public String toString() {
        return "ResponseInfo{" +
                "code=" + code +
                ", message='" + message + '\'' +
                ", data=" + data +
                '}';
    }

    public boolean isSuccess() {
        return code == ResponseCode.SUCCESS.code;
    }

    public boolean isAuthErr() {
        return code == ResponseCode.UNAUTHORIZED.code;
    }
}
