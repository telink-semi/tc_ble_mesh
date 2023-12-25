package com.telink.ble.mesh.web;

/**
 * response code used in
 */

public enum ResponseCode {
    SUCCESS(200, "success"),
    FAILED(500, "operation failed"),
    PLATFORM_NOT_SUPPORT(400, "operation failed"),
    UNAUTHORIZED(401, "not login or token timeout"),
    FORBIDDEN(403, "no permissions"),
    RESOURCE_NOT_FOUND(404, "resource not found"),
    UNKNOWN(505, "unknown error"),
    ;

    public int code;
    public String message;

    ResponseCode(int code, String message) {
        this.code = code;
        this.message = message;
    }
}
