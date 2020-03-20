package com.telink.ble.mesh.core.networking;

/**
 * Created by kee on 2019/8/14.
 */

public interface NetworkingPDU<T> {

    /**
     * parse PDU byte data to instance
     *
     * @return instance
     */
//    T parse(byte[] data);
    byte[] toByteArray();
}
