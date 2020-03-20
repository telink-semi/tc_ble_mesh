package com.telink.ble.mesh.core.provisioning.pdu;

import com.telink.ble.mesh.core.provisioning.AuthenticationMethod;

/**
 * Created by kee on 2019/7/18.
 */

public class ProvisioningStartPDU implements ProvisioningStatePDU {
    private static final int LEN = 5;

    /**
     * The algorithm used for provisioning
     * 0x00: FIPS P-256 Elliptic Curve
     * 0x01–0xFF: Reserved for Future Use
     */
    public byte algorithm;

    /**
     * Public Key used
     * 0x00: No OOB Public Key is used
     * 0x01: OOB Public Key is used
     * 0x02–0xFF: Prohibited
     */
    public byte publicKey;

    /**
     * Authentication Method used
     * 0x00: No OOB authentication is used
     * 0x01: Static OOB authentication is used
     * 0x02: Output OOB authentication is used
     * 0x03: Input OOB authentication is used
     * 0x04–0xFF: Prohibited
     */
    public byte authenticationMethod;

    /**
     * Selected Output OOB Action or Input OOB Action or 0x00
     */
    public byte authenticationAction;

    /**
     * Size of the Output OOB used or size of the Input OOB used or 0x00
     */
    public byte authenticationSize;

    public static ProvisioningStartPDU getSimple(boolean staticOOBSupported) {
        ProvisioningStartPDU startPDU = new ProvisioningStartPDU();
        startPDU.algorithm = 0;
        startPDU.publicKey = 0;
        startPDU.authenticationMethod = staticOOBSupported ? AuthenticationMethod.StaticOOB.value :
                AuthenticationMethod.NoOOB.value;
        startPDU.authenticationAction = 0;
        startPDU.authenticationSize = 0;
        return startPDU;
    }

    @Override
    public byte[] toBytes() {
        return new byte[]{
                algorithm,
                publicKey,
                authenticationMethod,
                authenticationAction,
                authenticationSize
        };
    }

    @Override
    public byte getState() {
        return ProvisioningPDU.TYPE_START;
    }
}
