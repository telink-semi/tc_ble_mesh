package com.telink.ble.mesh.ui.qrcode;

import java.io.Serializable;

public class HttpResponse implements Serializable {

    public boolean isSuccess = false;
    public int resultCode = 200;
    public String msg;
    public Object data = null;

}
