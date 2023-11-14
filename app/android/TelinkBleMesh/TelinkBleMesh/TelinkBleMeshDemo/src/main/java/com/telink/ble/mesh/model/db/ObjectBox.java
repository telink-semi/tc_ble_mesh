/********************************************************************************************************
 * @file ObjectBox.java
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
package com.telink.ble.mesh.model.db;

import android.content.Context;

import com.telink.ble.mesh.demo.BuildConfig;
import com.telink.ble.mesh.model.MyObjectBox;
import com.telink.ble.mesh.util.MeshLogger;

import io.objectbox.BoxStore;
import io.objectbox.BoxStoreBuilder;
import io.objectbox.android.Admin;
import io.objectbox.exception.FileCorruptException;
import io.objectbox.model.ValidateOnOpenMode;

public class ObjectBox {

    private static BoxStore boxStore;

    public static void init(Context context) {
        BoxStoreBuilder storeBuilder = MyObjectBox.builder()
                .validateOnOpen(ValidateOnOpenMode.WithLeaves)  // Additional DB page validation
                .validateOnOpenPageLimit(20)
                .androidContext(context.getApplicationContext());
        try {
            boxStore = storeBuilder.build();
        } catch (FileCorruptException e) { // Demonstrate handling issues caused by devices with a broken file system
            MeshLogger.d("File corrupt, trying previous data snapshot...");
            storeBuilder.usePreviousCommit();
            boxStore = storeBuilder.build();
        }

        if (BuildConfig.DEBUG) {
            MeshLogger.d(String.format("Using ObjectBox %s (%s)", BoxStore.getVersion(), BoxStore.getVersionNative()));
            // Enable ObjectBox Admin on debug builds.
            // https://docs.objectbox.io/data-browser
            new Admin(boxStore).start(context.getApplicationContext());
        }
    }

    public static BoxStore get() {
        return boxStore;
    }
}
