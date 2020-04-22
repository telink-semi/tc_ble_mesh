package com.telink.ble.mesh.core.message.config;

import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.core.networking.AccessType;

/**
 * configuration message
 * Created by kee on 2019/8/14.
 */
public abstract class ConfigMessage extends MeshMessage {


    public static final byte STATUS_SUCCESS = 0x00;

    /**
     * error code
     */
    public static final byte STATUS_INVALID_ADDRESS = 0x01;

    public static final byte STATUS_INVALID_MODEL = 0x02;

    public static final byte STATUS_INVALID_APPKEY_INDEX = 0x03;

    public static final byte STATUS_INVALID_NETKEY_INDEX = 0x04;

    public static final byte STATUS_INSUFFICIENT_RESOURCES = 0x05;

    public static final byte STATUS_KEY_INDEX_ALREADY_STORED = 0x06;

    public static final byte STATUS_INVALID_PUBLISH_PARAMETERS = 0x07;

    public static final byte STATUS_NOT_A_SUBSCRIBE_MODEL = 0x08;

    public static final byte STATUS_STORAGE_FAILURE = 0x09;

    public static final byte STATUS_FEATURE_NOT_SUPPORTED = 0x0A;

    public static final byte STATUS_CANNOT_UPDATE = 0x0B;

    public static final byte STATUS_CANNOT_REMOVE = 0x0C;

    public static final byte STATUS_CANNOT_BIND = 0x0D;

    public static final byte STATUS_TEMPORARILY_UNABLE_TO_CHANGE_STATE = 0x0E;

    public static final byte STATUS_CANNOT_SET = 0x0F;

    public static final byte STATUS_UNSPECIFIED_ERROR = 0x10;

    public static final byte STATUS_INVALID_BINDING = 0x11;


    public ConfigMessage(int destinationAddress) {
        this.destinationAddress = destinationAddress;
        // default rsp max
        this.responseMax = 1;
    }

    /**
     * for config message , AKF is 0
     *
     * @return application key flag
     */
    @Override
    public AccessType getAccessType() {
        return AccessType.DEVICE;
    }

}
