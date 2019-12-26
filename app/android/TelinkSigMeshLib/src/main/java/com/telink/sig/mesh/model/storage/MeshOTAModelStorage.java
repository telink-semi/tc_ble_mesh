package com.telink.sig.mesh.model.storage;

/**
 * Created by kee on 2019/5/29.
 */

import com.telink.sig.mesh.lib.MeshLib;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

/*
 model_mesh_ota_t;
 */
public class MeshOTAModelStorage implements MeshStorageBridge {
    //    private static final int LEN = 1132; // 1156
    public CommonModelStorage fwDistServer;
    public CommonModelStorage fwUpdateServer;
    public CommonModelStorage objTransServer;

    public CommonModelStorage fwUpdateClient;
    public CommonModelStorage objTransClient;
    public CommonModelStorage fwDistClient;

    // 4 bytes
    int rsv;

    public static MeshOTAModelStorage getDefault(int eleAdr, int keyIndex) {

        CommonModelStorage commonModelStorage = CommonModelStorage.getDefault(eleAdr, keyIndex);


        MeshOTAModelStorage meshOTAModelStorage = new MeshOTAModelStorage();
        meshOTAModelStorage.fwDistServer = commonModelStorage;
        meshOTAModelStorage.fwUpdateServer = commonModelStorage;
        meshOTAModelStorage.objTransServer = commonModelStorage;
        meshOTAModelStorage.fwUpdateClient = commonModelStorage;
        meshOTAModelStorage.objTransClient = commonModelStorage;
        meshOTAModelStorage.fwDistClient = commonModelStorage;

        meshOTAModelStorage.rsv = 0;
        return meshOTAModelStorage;
    }


    @Override
    public byte[] toBytes() {
        int len = CommonModelStorage.getLen() * 6 + 4;
        ByteBuffer byteBuffer = ByteBuffer.allocate(len).order(ByteOrder.LITTLE_ENDIAN);
        byteBuffer.put(fwDistServer.toBytes())
                .put(fwUpdateServer.toBytes())
                .put(objTransServer.toBytes())
                .put(fwUpdateClient.toBytes())
                .put(objTransClient.toBytes())
                .put(fwDistClient.toBytes())
                .putInt(rsv);
        return byteBuffer.array();
    }


}
