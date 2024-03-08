/********************************************************************************************************
 * @file ConfigState.java
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

import java.io.Serializable;

/**
 * #ref spec mesh_v1.0 # 4.2
 * Foundation models#state definition
 */
public enum ConfigState implements Serializable {
    /**
     * default: 10
     */
    DEFAULT_TTL("Default TTL", "The Default TTL state determines the TTL value used when sending messages."),

    /**
     * default
     * relay: 0x01
     * relay retransmit: 0x15
     */
    RELAY("Relay & RelayRetransmit",
            "The Relay state indicates support for the Relay feature; The Relay Retransmit state is a composite state that controls parameters of retransmission of the Network PDU relayed by the node."),
    /**
     * default:
     * 0x01
     */
    SECURE_NETWORK_BEACON("Secure Network Beacon",
            "The Secure Network Beacon state determines if a node is periodically broadcasting Secure Network beacon messages."),

    /**
     * default:
     */
    PROXY("GATT Proxy", "The GATT Proxy state indicates if the Mesh Proxy Service is supported, and if supported, it indicates and controls the status of the Mesh Proxy Service. "),

    /**
     *
     */
    NODE_IDENTITY("Node Identity", "The Node Identity state determines if a node that supports the Mesh Proxy Service is advertising on a subnet using Node Identity messages. "),

    /**
     * default: 0x01
     */
    FRIEND("Friend", "The Friend state indicates support for the Friend feature. If Friend feature is supported, then this also indicates and controls whether Friend feature is enabled or disabled."),

    /**
     * default: 00000000
     */
    KEY_REFRESH_PHASE("Key Refresh Phase", "The Key Refresh Phase state indicates and controls the Key Refresh procedure for each NetKey in the NetKey List. ", false),

    /**
     * Network Transmit
     */
    NETWORK_TRANSMIT("Network Transmit", "The Network Transmit state is a composite state that controls the number and timing of the transmissions of Network PDU originating from a node."),

    /**
     * On-Demand Private GATT Proxy
     */
    ON_DEMAND_PRIVATE_GATT_PROXY("On-Demand Private GATT Proxy", "The On-Demand Private GATT Proxy state indicates whether advertising with Private Network Identity type can be enabled on demand and can be triggered upon reception of a Solicitation PDU."),

    /**
     * Proxy Solicitation
     */
//    PROXY_SOLICITATION("Proxy Solicitation", "A Proxy Client may use the GAP General Discoverable mode with non-connectable non-scannable undirected advertising events to send Solicitation PDUs to solicit a Proxy Server that supports the Private Proxy functionality to start advertising with Private Network Identity type"),

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
