package com.telink.ble.mesh.core.proxy;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.networking.NetworkLayerPDU;
import com.telink.ble.mesh.core.networking.NonceGenerator;

import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/26.
 */

public class ProxyConfigurationPDU extends NetworkLayerPDU {

    public static final byte ctl = 1;

    public static final byte ttl = 0;

    public static final byte dst = 0x00;

    public ProxyConfigurationPDU(NetworkEncryptionSuite encryptionSuite) {
        super(encryptionSuite);
    }

    @Override
    protected byte[] generateNonce() {
        byte[] seqNo = MeshUtils.integer2Bytes(getSeq(), 3, ByteOrder.BIG_ENDIAN);
        return NonceGenerator.generateProxyNonce(seqNo, getSrc(), this.encryptionSuite.ivIndex);
    }
}
