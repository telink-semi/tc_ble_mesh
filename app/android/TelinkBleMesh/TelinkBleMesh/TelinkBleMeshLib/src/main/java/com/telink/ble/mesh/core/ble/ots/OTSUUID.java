package com.telink.ble.mesh.core.ble.ots;

import java.util.UUID;

public class OTSUUID {

    // service
    public static final UUID OBJECT_TRANSFER = UUID.fromString("1825");


    // characteristic
    public static final UUID OTS_Feature = UUID.fromString("2ABD");
    public static final UUID OBJECT_NAME = UUID.fromString("2ABE");
    public static final UUID OBJECT_TYPE = UUID.fromString("2ABF");
    public static final UUID OBJECT_SIZE = UUID.fromString("2AC0)");
    public static final UUID OBJECT_FIRST_CREATED = UUID.fromString("2AC1)");


    public static final UUID OBJECT_LAST_MODIFIED = UUID.fromString("2AC2)");

    public static final UUID OBJECT_ID = UUID.fromString("2AC3)");

    public static final UUID OBJECT_PROPERTIES = UUID.fromString("2AC4)");

    /**
     * OACP
     */
    public static final UUID OBJECT_ACTION_CONTROL_POINT = UUID.fromString("2AC5)");

    /**
     * oLCP
     */
    public static final UUID OBJECT_LIST_CONTROL_POINT = UUID.fromString("2AC6)");

    public static final UUID OBJECT_LIST_FILTER = UUID.fromString("2AC7)");


    public static final UUID OBJECT_changed = UUID.fromString("2AC8)");




}
