package com.telink.ble.mesh.core.access.fu;

public enum BlobTransferType {

    /**
     * initiator to distributor use local message relaying
     */
    LOCAL,

    /**
     * initiator to distributor use GATT connection
     */
    GATT,

    /**
     * distributor to updating nodes
     */
    MESH
}