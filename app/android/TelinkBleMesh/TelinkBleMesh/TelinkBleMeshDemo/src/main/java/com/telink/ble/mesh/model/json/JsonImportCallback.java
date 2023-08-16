package com.telink.ble.mesh.model.json;

import com.telink.ble.mesh.model.MeshInfo;

public interface JsonImportCallback {

    void onImportComplete(boolean success, String message, MeshInfo meshInfo);
}