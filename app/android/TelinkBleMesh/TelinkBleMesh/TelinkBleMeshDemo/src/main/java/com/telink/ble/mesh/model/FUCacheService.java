package com.telink.ble.mesh.model;

import android.content.Context;

import com.telink.ble.mesh.core.access.fu.UpdatePolicy;
import com.telink.ble.mesh.entity.MeshUpdatingDevice;
import com.telink.ble.mesh.ui.qrcode.TelinkHttpClient;
import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;
import java.util.List;

/**
 * firmware update cache,
 * saved when distribution started by remote device,
 * retrieved when mesh reconnect success
 */
public class FUCacheService implements Serializable {

    private static final String FU_CACHE_NAME = "FU_CACHE";

    private static final FUCacheService service = new FUCacheService();

    FUCache fuCache;

    private FUCacheService() {
    }

    public static FUCacheService getInstance() {
        return service;
    }

    /**
     * load storage
     */
    public void load(Context context) {
        Object obj = FileSystem.readAsObject(context, FU_CACHE_NAME);
        fuCache = obj == null ? null : (FUCache) obj;
        MeshLogger.d("load FUCache : " + (fuCache == null ? "NULL" : fuCache.toString()));
    }


    /**
     * get firmware update cache in memory
     */
    public FUCache get() {
        return this.fuCache;
    }

    /**
     * save cache in disk
     */
    public void save(Context context, FUCache cache) {
        MeshLogger.d("FUCache save");
        this.fuCache = cache;
        FileSystem.writeAsObject(context, FU_CACHE_NAME, this.fuCache);
    }

    /**
     * clear memory and remove file in disk
     */
    public void clear(Context context) {
        MeshLogger.d("FUCache clear");
        this.fuCache = null;
        FileSystem.deleteFile(context, FU_CACHE_NAME);
    }
}
