package com.telink.ble.mesh.foundation.parameter;

/**
 * Created by kee on 2019/9/18.
 */

public enum AutoConnectFilterType {
    /**
     * node identity only
     */
    NODE_IDENTITY(true, false),
    /**
     * network id only
     */
    NETWORK_ID(false, true),
    /**
     * both supported
     */
    AUTO(true, true);

    public final boolean isNodeIdentitySupport;

    public final boolean isNetworkIdSupport;

    AutoConnectFilterType(boolean isNodeIdentitySupport, boolean isNetworkIdSupport) {
        this.isNodeIdentitySupport = isNodeIdentitySupport;
        this.isNetworkIdSupport = isNetworkIdSupport;
    }
}
