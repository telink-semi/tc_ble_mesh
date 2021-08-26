package com.telink.ble.mesh.model;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

/**
 * device config values, such as TTL, relay
 */
public class DeviceConfig implements Serializable {

    public ConfigState configState;

    public List<Parameter> parameters = new ArrayList<>();

    public boolean expanded = false;

    public DeviceConfig(ConfigState configState) {
        this.configState = configState;
        switch (configState) {
            case DEFAULT_TTL:
                parameters.add(new Parameter("TTL", 8));
                break;

            case RELAY:
                parameters.add(new Parameter("Relay", 8));
                parameters.add(new Parameter("RelayRetransmitCount", 3));
                parameters.add(new Parameter("RelayRetransmitIntervalSteps", 5));
                break;

            case SECURE_NETWORK_BEACON:
                parameters.add(new Parameter("Beacon", 8));
                break;

            case PROXY:
                parameters.add(new Parameter("GATTProxy", 8));
                break;

            case NODE_IDENTITY:
                parameters.add(new Parameter("NetKeyIndex", 16));
                parameters.add(new Parameter("Identity", 8));
                break;

            case FRIEND:
                parameters.add(new Parameter("Friend", 8));
                break;

            case KEY_REFRESH_PHASE:
                parameters.add(new Parameter("NetKeyIndex", 16));
                parameters.add(new Parameter("Transition", 8));
                break;

            case NETWORK_TRANSMIT:
                parameters.add(new Parameter("NetworkTransmitCount", 3));
                parameters.add(new Parameter("NetworkTransmitIntervalSteps", 5));
                break;
        }
    }

    public void updateParamValue(String key, byte[] value) {
        for (Parameter parameter : parameters) {
            if (parameter.key.equals(key)) {
                parameter.value = value;
                break;
            }
        }
    }

    public byte[] getParamValue(String key) {
        for (Parameter parameter : parameters) {
            if (parameter.key.equals(key)) {
                return parameter.value;
            }
        }
        return null;
    }

    /**
     * all values
     */
    public byte[] getCompositeValues() {
        switch (configState) {
            case DEFAULT_TTL:
                byte[] ttl = getParamValue("TTL");
                if (ttl.length != 1) return null;
                return ttl;

            case RELAY:
                byte[] relay = getParamValue("Relay");

                byte[] relayRetransmitCount = getParamValue("RelayRetransmitCount");
                byte[] relayRetransmitIntervalSteps = getParamValue("RelayRetransmitIntervalSteps");

                if (relay.length != 1 || relayRetransmitCount.length != 1 || relayRetransmitIntervalSteps.length != 1)
                    return null;

                return new byte[]{relay[0], (byte) ((relayRetransmitCount[0] & 0b111) | (relayRetransmitIntervalSteps[0] << 3))};

            case SECURE_NETWORK_BEACON:
                byte[] beacon = getParamValue("Beacon");
                if (beacon.length != 1) return null;
                return beacon;

            case PROXY:
                byte[] proxy = getParamValue("GATTProxy");
                if (proxy.length != 1) return null;
                return proxy;

            case NODE_IDENTITY:
                byte[] netKeyIndex = getParamValue("NetKeyIndex");

                byte[] Identity = getParamValue("Identity");

//                if (netKeyIndex.length != )

                break;

            case FRIEND:
                parameters.add(new Parameter("Friend", 8));
                break;

            case KEY_REFRESH_PHASE:
                parameters.add(new Parameter("NetKeyIndex", 16));
                parameters.add(new Parameter("Transition", 8));
                break;

            case NETWORK_TRANSMIT:
                parameters.add(new Parameter("NetworkTransmitCount", 3));
                parameters.add(new Parameter("NetworkTransmitIntervalSteps", 5));
                break;
        }
        return null;
    }

    public static class Parameter implements Serializable{

        /**
         * key
         */
        public String key;

        /**
         * input value
         */
        public byte[] value;

        /**
         * bit length
         */
        public int bitLen;

        public Parameter(String key, int bitLen) {
            this.key = key;
            this.bitLen = bitLen;
        }
    }
}
