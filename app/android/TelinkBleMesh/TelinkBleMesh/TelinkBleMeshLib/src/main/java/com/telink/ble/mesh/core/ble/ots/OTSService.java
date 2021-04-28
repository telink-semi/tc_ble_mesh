package com.telink.ble.mesh.core.ble.ots;

abstract public class OTSService {


    /**
     * read metadata
     */
    abstract public void readObjectName();

    abstract public void readObjectType();

    abstract public void readObjectSize();

    abstract public void readObjectFirstCreated();

    abstract public void readObjectLastModified();
}
