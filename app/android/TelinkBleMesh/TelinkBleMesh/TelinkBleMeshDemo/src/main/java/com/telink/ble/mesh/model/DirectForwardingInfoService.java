/********************************************************************************************************
 * @file GroupInfo.java
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

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.util.FileSystem;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by kee on 2017/8/18.
 */

public class DirectForwardingInfoService implements Serializable {

    private static final String DF_CACHE = "tlk_df";
    private static DirectForwardingInfoService instance = new DirectForwardingInfoService();

    public static DirectForwardingInfoService getInstance() {
        return instance;
    }

    List<DirectForwardingInfo> directForwardingInfoList;

    public void load(Context context) {
        Object obj = FileSystem.readAsObject(context, DF_CACHE);
        if (obj != null) {
            directForwardingInfoList = (List<DirectForwardingInfo>) obj;
        } else {
            directForwardingInfoList = new ArrayList<>();
        }
    }

    public List<DirectForwardingInfo> get() {
        return this.directForwardingInfoList;
    }

    public void addItem(DirectForwardingInfo item) {
        this.directForwardingInfoList.add(item);
        save();
    }

    public void removeItem(DirectForwardingInfo item) {
        this.directForwardingInfoList.remove(item);
    }

    public void clear() {
        this.directForwardingInfoList.clear();
    }

    public void save() {
        FileSystem.writeAsObject(TelinkMeshApplication.getInstance(), DF_CACHE, this.directForwardingInfoList);
    }
}
