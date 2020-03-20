package com.telink.ble.mesh.core.networking.beacon;

import com.telink.ble.mesh.core.provisioning.pdu.PDU;

/**
 * Created by kee on 2019/11/18.
 */

public abstract class MeshBeaconPDU implements PDU {

    public static final byte BEACON_TYPE_UNPROVISIONED_DEVICE = 0x00;

    public static final byte BEACON_TYPE_SECURE_NETWORK = 0x01;

    protected byte beaconType;

    protected byte[] beaconData;


}
