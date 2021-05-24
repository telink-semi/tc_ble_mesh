package com.telink.ble.mesh.core.access.fu;

public enum BlobTransferType {

    /**
     * initiator to distributor use local message relaying
     */
    LOCAL_INIT,

    /**
     * initiator to distributor use GATT connection
     */
    GATT_INIT,


    /**
     * distributor to updating node use GATT connection ,
     * only one updating node, meanwhile this node is direct connected
     */
    GATT_DIST,


    /**
     * distributor to updating nodes
     */
    MESH_DIST
}