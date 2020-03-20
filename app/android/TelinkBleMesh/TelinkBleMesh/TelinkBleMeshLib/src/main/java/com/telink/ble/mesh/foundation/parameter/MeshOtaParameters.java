package com.telink.ble.mesh.foundation.parameter;

import com.telink.ble.mesh.entity.MeshUpdatingConfiguration;

/**
 * Created by kee on 2017/11/23.
 */

public class MeshOtaParameters extends Parameters {

    public MeshOtaParameters(MeshUpdatingConfiguration meshUpdatingConfiguration) {
        this.set(COMMON_PROXY_FILTER_INIT_NEEDED, true);
        this.set(ACTION_MESH_OTA_CONFIG, meshUpdatingConfiguration);
    }

}
