package com.telink.ble.mesh.model;

import java.io.Serializable;

public class OOBPair implements Serializable {
    public static final int IMPORT_MODE_MANUAL = 0;

    public static final int IMPORT_MODE_FILE = 1;

    public byte[] deviceUUID;

    public byte[] oob;

    public int importMode;

    public long timestamp;
}
