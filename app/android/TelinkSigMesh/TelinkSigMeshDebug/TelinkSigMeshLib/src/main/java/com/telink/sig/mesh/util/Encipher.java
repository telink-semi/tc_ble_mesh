package com.telink.sig.mesh.util;

import org.spongycastle.crypto.BlockCipher;
import org.spongycastle.crypto.CipherParameters;
import org.spongycastle.crypto.InvalidCipherTextException;
import org.spongycastle.crypto.engines.AESEngine;
import org.spongycastle.crypto.engines.AESLightEngine;
import org.spongycastle.crypto.macs.CMac;
import org.spongycastle.crypto.modes.CCMBlockCipher;
import org.spongycastle.crypto.params.AEADParameters;
import org.spongycastle.crypto.params.KeyParameter;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * Mesh Encipher
 * Created by kee on 2019/8/19.
 */
public final class Encipher {

    private static final byte[] SALT_INPUT_K2 = "smk2".getBytes();

    private static final byte[] SALT_INPUT_K3 = "smk3".getBytes();

    //  “id6” || 0x01
    public static final byte[] SALT_K3_M = new byte[]{0x69, 0x64, 0x36, 0x34, 0x01};

    private static final byte[] SALT_NKIK = "nkik".getBytes();

    private static final byte[] SALT_ID128 = "id128".getBytes();

    // 48 bit
    private static final byte[] NODE_IDENTITY_HASH_PADDING = new byte[]{0, 0, 0, 0, 0, 0};

    private static final byte[] SALT_KEY_ZERO =
            {
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
            };

    /**
     * aes-cmac
     * <p>
     * k is the 128-bit key
     * m is the variable length data to be authenticated
     */
    public static byte[] aesCmac(byte[] content, byte[] key) {
        CipherParameters cipherParameters = new KeyParameter(key);
        BlockCipher blockCipher = new AESEngine();
        CMac mac = new CMac(blockCipher);

        mac.init(cipherParameters);
        mac.update(content, 0, content.length);
        byte[] re = new byte[16];
        mac.doFinal(re, 0);
        return re;
    }


    /**
     * S1
     * s1(M) = AES-CMAC ZERO (M)
     *
     * @param m input
     * @return re
     */
    public static byte[] generateSalt(byte[] m) {
        return aesCmac(m, SALT_KEY_ZERO);
    }

    /*
    The inputs to function k1 are:
N is 0 or more octets SALT is 128 bits
P is 0 or more octets
The key (T) is computed as follows: T = AES-CMAC SALT (N)
The output of the key generation function k1 is as follows: k1(N, SALT, P) = AES-CMACT (P)
     */


    /**
     * @param data    target data
     * @param k       key
     * @param n       nonce
     * @param micSize mic
     * @param encrypt true: encryption, false: decryption
     * @return result
     */
    public static byte[] ccm(byte[] data, byte[] k, byte[] n, int micSize, boolean encrypt) {
        byte[] result = new byte[data.length + (encrypt ? micSize : (-micSize))];
        CCMBlockCipher ccmBlockCipher = new CCMBlockCipher(new AESEngine());
        AEADParameters aeadParameters = new AEADParameters(new KeyParameter(k), micSize * 8, n);
        ccmBlockCipher.init(encrypt, aeadParameters);
        ccmBlockCipher.processBytes(data, 0, data.length, result, data.length);
        try {
            ccmBlockCipher.doFinal(result, 0);
            return result;
        } catch (InvalidCipherTextException e) {
            return null;
        }
    }


    public static byte[] aes(final byte[] data, final byte[] key) {
        final byte[] encrypted = new byte[data.length];
        final CipherParameters cipherParameters = new KeyParameter(key);
        final AESLightEngine engine = new AESLightEngine();
        engine.init(true, cipherParameters);
        engine.processBlock(data, 0, encrypted, 0);

        return encrypted;
    }


    public static byte[] k1(byte[] ecdh, byte[] salt, byte[] text) {
        byte[] t = aesCmac(ecdh, salt);
        return aesCmac(text, t);
    }


    public static byte[][] k2(byte[] data, byte[] p) {

        byte[] salt = generateSalt(SALT_INPUT_K2);
        byte[] t = aesCmac(data, salt);

        byte[] t0 = {};
        final ByteBuffer inputBufferT0 = ByteBuffer.allocate(t0.length + p.length + 1);
        inputBufferT0.put(t0);
        inputBufferT0.put(p);
        inputBufferT0.put((byte) 0x01);
        final byte[] t1 = aesCmac(inputBufferT0.array(), t);
//        final byte nid = (byte) (t1[15] & 0x7F);

        final ByteBuffer inputBufferT1 = ByteBuffer.allocate(t1.length + p.length + 1);
        inputBufferT1.put(t1);
        inputBufferT1.put(p);
        inputBufferT1.put((byte) 0x02);
        // encryptionKey
        final byte[] t2 = aesCmac(inputBufferT1.array(), t);

        final ByteBuffer inputBufferT2 = ByteBuffer.allocate(t2.length + p.length + 1);
        inputBufferT2.put(t2);
        inputBufferT2.put(p);
        inputBufferT2.put((byte) 0x03);
        // privacyKey
        final byte[] t3 = aesCmac(inputBufferT2.array(), t);

        return new byte[][]{t1, t2, t3};
//        return new K2Output(nid, encryptionKey, privacyKey);
    }

    public static byte[] k3(final byte[] netKey) {

        final byte[] salt = generateSalt(SALT_INPUT_K3);

        final byte[] t = aesCmac(netKey, salt);

        final byte[] result = aesCmac(SALT_K3_M, t);

        //Only the least significant 8 bytes are returned
        final byte[] networkId = new byte[8];
        final int srcOffset = result.length - networkId.length;

        System.arraycopy(result, srcOffset, networkId, 0, networkId.length);
        //bBuffer.
        return networkId;
    }


    /**
     * Hash = e(IdentityKey, Padding | Random | Address) mod 264
     * Where:
     * Padding – 48 bits of padding, all bits set to 0.
     * Random – 64 bit random value.
     * Address – The unicast address of the node.
     */

    public static byte[] generateNodeIdentityHash(final byte[] identityKey, final byte[] random, int src) {
        int length = NODE_IDENTITY_HASH_PADDING.length + random.length + 2;
        ByteBuffer bufferHashInput = ByteBuffer.allocate(length).order(ByteOrder.BIG_ENDIAN);
        bufferHashInput.put(NODE_IDENTITY_HASH_PADDING);
        bufferHashInput.put(random);
        bufferHashInput.putShort((short) src);
        byte[] hashInput = bufferHashInput.array();
        byte[] hash = aes(hashInput, identityKey);

        ByteBuffer buffer = ByteBuffer.allocate(8);
        buffer.put(hash, 8, 8);

        return buffer.array();
    }

    public static byte[] generateIdentityKey(final byte[] networkKey) {
        final byte[] salt = generateSalt(SALT_NKIK);
        ByteBuffer buffer = ByteBuffer.allocate(SALT_ID128.length + 1);
        buffer.put(SALT_ID128);
        buffer.put((byte) 0x01);
        final byte[] p = buffer.array();
        return k1(networkKey, salt, p);
    }

}
