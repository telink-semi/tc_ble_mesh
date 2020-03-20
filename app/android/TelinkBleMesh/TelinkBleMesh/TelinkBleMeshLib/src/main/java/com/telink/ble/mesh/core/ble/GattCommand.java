package com.telink.ble.mesh.core.ble;


import com.telink.ble.mesh.util.Arrays;

import java.util.UUID;

public class GattCommand {

    public UUID serviceUUID;
    public UUID characteristicUUID;
    public UUID descriptorUUID;
    public CommandType type;
    public byte[] data;
    public Object tag;
    public int flag;
    public int delay;

    public GattCommand() {
        this(null, null, CommandType.WRITE);
    }

    public GattCommand(UUID serviceUUID, UUID characteristicUUID, CommandType type) {
        this(serviceUUID, characteristicUUID, type, null);
    }

    public GattCommand(UUID serviceUUID, UUID characteristicUUID, CommandType type,
                       byte[] data) {
        this(serviceUUID, characteristicUUID, type, data, null);
    }

    public GattCommand(UUID serviceUUID, UUID characteristicUUID, CommandType type,
                       byte[] data, Object tag) {

        this.serviceUUID = serviceUUID;
        this.characteristicUUID = characteristicUUID;
        this.type = type;
        this.data = data;
        this.tag = tag;
    }

    public static GattCommand newInstance() {
        return new GattCommand();
    }

    public void clear() {
        this.serviceUUID = null;
        this.characteristicUUID = null;
        this.descriptorUUID = null;
        this.data = null;
    }

    @Override
    public String toString() {
        String d = "";

        if (data != null)
            d = Arrays.bytesToHexString(this.data, ",");

        return "{ tag : " + this.tag + ", typeValue : " + this.type
                + " CHARACTERISTIC_UUID_WRITE :" + characteristicUUID.toString() + " data: " + d + " delay :" + delay + "}";
    }

    public enum CommandType {
        READ, READ_DESCRIPTOR, WRITE, WRITE_NO_RESPONSE, WRITE_DESCRIPTOR, ENABLE_NOTIFY, DISABLE_NOTIFY
    }
}
