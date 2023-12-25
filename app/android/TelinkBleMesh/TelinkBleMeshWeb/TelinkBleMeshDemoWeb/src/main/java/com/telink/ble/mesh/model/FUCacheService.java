/********************************************************************************************************
 * @file FUCacheService.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2017
 *
 * @par Copyright (c) 2017, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/
package com.telink.ble.mesh.model;

import android.content.Context;

import com.telink.ble.mesh.util.FileSystem;
import com.telink.ble.mesh.util.MeshLogger;

import java.io.Serializable;

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
