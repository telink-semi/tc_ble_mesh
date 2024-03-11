package com.telink.ble.mesh.ui.qrcode;

import java.io.Serializable;

/**
 * 二维码分享数据信息
 */
public class NetworkQrCodeInfo implements Serializable {
    /**
     * 分享链接
     */
    public Integer shareLinkId;

    /**
     * 创建者id
     */
    public String userName;

    /**
     * 网络名称
     */
    public String networkName;
}
