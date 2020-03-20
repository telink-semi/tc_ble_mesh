package com.telink.ble.mesh.core.networking;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Created by kee on 2019/8/12.
 */

public class NonceGenerator {

    private static final int NONCE_LENGTH = 13;

    /**
     * Used with an encryption key for network authentication and encryption
     */
    private static final byte NONCE_TYPE_NETWORK = 0x00;

    /**
     * Used with an application key for upper transport authentication and encryption
     */
    private static final byte NONCE_TYPE_APPLICATION = 0x01;

    /**
     * Used with a device key for upper transport authentication and encryption
     */
    private static final byte NONCE_TYPE_DEVICE = 0x02;

    /**
     * Used with an encryption key for proxy authentication and encryption
     */
    private static final byte NONCE_TYPE_PROXY = 0x03;

    private static final byte NONCE_PADDING = 0x00;

    public static byte[] generateNetworkNonce(byte ctlTTL, byte[] sequenceNumber, int src, int ivIndex) {
        final ByteBuffer networkNonce = ByteBuffer.allocate(13).order(ByteOrder.BIG_ENDIAN);
        networkNonce.put(NONCE_TYPE_NETWORK); //Nonce typeValue
        networkNonce.put(ctlTTL); // CTL and TTL
        networkNonce.put(sequenceNumber);
        networkNonce.putShort((short) src);
        networkNonce.put(new byte[]{0x00, 0x00}); //PADDING
        networkNonce.putInt(ivIndex);
        return networkNonce.array();
    }

    /**
     * output application or device nonce
     *
     * @param aszmic     SZMIC if a Segmented Access message or 0 for all other message formats
     * @param accessType if Application or Device command
     */
    public static byte[] generateAccessNonce(byte aszmic, byte[] sequenceNumber, int src, int dst, int ivIndex, AccessType accessType) {
        ByteBuffer accessNonceBuf = ByteBuffer.allocate(NONCE_LENGTH).order(ByteOrder.BIG_ENDIAN);
        byte type = accessType == AccessType.APPLICATION ? NONCE_TYPE_APPLICATION : NONCE_TYPE_DEVICE;
        accessNonceBuf.put(type); //Nonce typeValue
        accessNonceBuf.put((byte) ((aszmic << 7) | NONCE_PADDING)); //ASZMIC (SZMIC if a segmented access message) and PAD
        accessNonceBuf.put(sequenceNumber);
        accessNonceBuf.putShort((short) src);
        accessNonceBuf.putShort((short) dst);
        accessNonceBuf.putInt(ivIndex);
        return accessNonceBuf.array();
    }

    /**
     * output proxy nonce
     */
    public static byte[] generateProxyNonce(byte[] sequenceNumber, int src, int ivIndex) {
        ByteBuffer applicationNonceBuffer = ByteBuffer.allocate(NONCE_LENGTH);
        applicationNonceBuffer.put(NONCE_TYPE_PROXY); //Nonce typeValue
        applicationNonceBuffer.put(NONCE_PADDING); //PAD
        applicationNonceBuffer.put(sequenceNumber);
        applicationNonceBuffer.putShort((short) src);
        applicationNonceBuffer.put(new byte[]{NONCE_PADDING, NONCE_PADDING});
        applicationNonceBuffer.putInt(ivIndex);
        return applicationNonceBuffer.array();
    }
}
