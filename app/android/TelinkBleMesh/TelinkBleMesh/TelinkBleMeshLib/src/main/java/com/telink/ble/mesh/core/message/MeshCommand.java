package com.telink.ble.mesh.core.message;

import com.telink.ble.mesh.core.networking.AccessType;

/**
 * Created by kee on 2019/8/12.
 */

public abstract class MeshCommand {

    /**
     * The AKF and AID fields of the Lower Transport PDU shall be set according to the application key or device key used to encrypt and authenticate the Upper Transport PDU.
     * If an application key is used, then the AKF field shall be set to 1 and the AID field shall be set to the application key identifier (AID). If the device key is used, then the AKF field shall be set to 0 and the AID field shall be set to 0b000000.
     */

    private byte szmic = 0;

    /**
     * application key flag
     * 0 device key
     * 1 application key
     */

//    private int akf;
    private AccessType accessType;

    private int destinationAddress;

    private int sourceAddress;

    // application key identity
    private int aid;

    private int ctl;

    private int ttl;

    public abstract int getOpcode();

    public abstract byte[] getParams();

    public int getSourceAddress() {
        return sourceAddress;
    }

    public void setSourceAddress(int sourceAddress) {
        this.sourceAddress = sourceAddress;
    }

    public int getDestinationAddress() {
        return destinationAddress;
    }

    public void setDestinationAddress(int destinationAddress) {
        this.destinationAddress = destinationAddress;
    }

    public AccessType getAccessType() {
        return accessType;
    }

    public void setAccessType(AccessType accessType) {
        this.accessType = accessType;
    }

    public byte getSzmic() {
        return szmic;
    }

    public void setSzmic(byte szmic) {
        this.szmic = szmic;
    }


    public int getAid() {
        return aid;
    }

    public void saveAid(int aid) {
        this.aid = aid;
    }

    public int getCtl() {
        return ctl;
    }

    public void setCtl(int ctl) {
        this.ctl = ctl;
    }

    public int getTtl() {
        return ttl;
    }

    public void setTtl(int ttl) {
        this.ttl = ttl;
    }
}
