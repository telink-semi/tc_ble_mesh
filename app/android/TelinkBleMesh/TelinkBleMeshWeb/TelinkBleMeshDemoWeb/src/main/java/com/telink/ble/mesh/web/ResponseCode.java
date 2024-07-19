/********************************************************************************************************
 * @file ResponseCode.java
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
