package com.telink.ble.mesh.core.provisioning.pdu;

/**
 * Created by kee on 2019/7/18.
 */
// big endian
public class ProvisioningPDU implements PDU {
    /**
     * including :
     * padding : 2 bits 0b00
     * typeValue : 6 bits
     * 0x00 - 0x09 indicates provisioning state
     * 0x0A–0xFF Reserved for Future Use
     */
    private byte type;

    /**
     * provisioning params
     */
    private byte[] params;

    public static ProvisioningPDU createGeneralInstance(byte type) {
        ProvisioningPDU pdu = new ProvisioningPDU();
        pdu.type = type;
        if (type == TYPE_INVITE) {
            pdu.params = new byte[]{0};
        }
        return pdu;
    }


    /**
     * Invites a device to join a mesh network
     */
    public static final byte TYPE_INVITE = 0x00;

    /**
     * Indicates the capabilities of the device
     */
    public static final byte TYPE_CAPABILITIES = 0x01;

    /**
     * Indicates the provisioning method selected by the Provisioner based on the capabilities of the device
     */
    public static final byte TYPE_START = 0x02;

    /**
     * Contains the Public Key of the device or the Provisioner
     */
    public static final byte TYPE_PUBLIC_KEY = 0x03;

    /**
     * Indicates that the user has completed inputting a value
     */
    public static final byte TYPE_INPUT_COMPLETE = 0x04;

    /**
     * Contains the provisioning confirmation value of the device or the Provisioner
     */
    public static final byte TYPE_CONFIRMATION = 0x05;

    /**
     * Contains the provisioning random value of the device or the Provisioner
     */
    public static final byte TYPE_RANDOM = 0x06;

    /**
     * Includes the assigned unicast address of the primary element, a network key, NetKey Index, Flags and the IV Index
     */
    public static final byte TYPE_DATA = 0x07;

    /**
     * Indicates that provisioning is complete
     */
    public static final byte TYPE_COMPLETE = 0x08;

    /**
     * Indicates that provisioning was unsuccessful
     */
    public static final byte TYPE_FAILED = 0x09;


    @Override
    public byte[] toBytes() {
        final int len = params == null ? 1 : 1 + params.length;
        byte[] re = new byte[len];
        re[0] = type;
        if (params != null) {
            System.arraycopy(params, 0, re, 1, params.length);
        }
        return re;
    }
}
