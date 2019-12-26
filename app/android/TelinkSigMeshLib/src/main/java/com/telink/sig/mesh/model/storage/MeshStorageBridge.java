package com.telink.sig.mesh.model.storage;

/**
 * Created by kee on 2019/5/29.
 */

public interface MeshStorageBridge<T> {

    byte[] toBytes();

//    T fromBytes(byte[] data);
}
