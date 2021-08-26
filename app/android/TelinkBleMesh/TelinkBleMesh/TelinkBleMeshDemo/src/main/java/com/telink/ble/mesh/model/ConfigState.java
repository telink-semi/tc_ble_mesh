package com.telink.ble.mesh.model;

import java.io.Serializable;

/**
 * #ref spec mesh_v1.0 # 4.2
 * Foundation models#state definition
 */
public enum ConfigState implements Serializable {
    DEFAULT_TTL("Default TTL", "The Default TTL state determines the TTL value used when sending messages."),
    RELAY("Relay & RelayRetransmit",
            "The Relay state indicates support for the Relay feature; The Relay Retransmit state is a composite state that controls parameters of retransmission of the Network PDU relayed by the node."),
    SECURE_NETWORK_BEACON("Secure Network Beacon",
            "The Secure Network Beacon state determines if a node is periodically broadcasting Secure Network beacon messages."),
    PROXY("GATT Proxy", "The GATT Proxy state indicates if the Mesh Proxy Service is supported, and if supported, it indicates and controls the status of the Mesh Proxy Service. "),
    NODE_IDENTITY("Node Identity", "The Node Identity state determines if a node that supports the Mesh Proxy Service is advertising on a subnet using Node Identity messages. "),
    FRIEND("Friend", "The Friend state indicates support for the Friend feature. If Friend feature is supported, then this also indicates and controls whether Friend feature is enabled or disabled."),
    KEY_REFRESH_PHASE("Key Refresh Phase", "The Key Refresh Phase state indicates and controls the Key Refresh procedure for each NetKey in the NetKey List. ", false),
    NETWORK_TRANSMIT("Network Transmit", "The Network Transmit state is a composite state that controls the number and timing of the transmissions of Network PDU originating from a node."),

    // in RELAY
//    RELAY_RETRANSMIT("Relay Retransmit", "The Relay Retransmit state is a composite state that controls parameters of retransmission of the Network PDU relayed by the node."),
    ;


    public final String name;
    public final String definition;
    /**
     * getting message supported
     */
    public final boolean isGetSupported;

    /**
     * setting message supported
     */
    public final boolean isSetSupported;

    ConfigState(String name, String definition) {
        this.name = name;
        this.definition = definition;
        this.isGetSupported = true;
        this.isSetSupported = true;
    }

    ConfigState(String name, String definition, boolean isSetSupported) {
        this.name = name;
        this.definition = definition;
        this.isGetSupported = true;
        this.isSetSupported = isSetSupported;
    }

    ConfigState(String name, String definition, boolean isGetSupported, boolean isSetSupported) {
        this.name = name;
        this.definition = definition;
        this.isGetSupported = isGetSupported;
        this.isSetSupported = isSetSupported;
    }
}
