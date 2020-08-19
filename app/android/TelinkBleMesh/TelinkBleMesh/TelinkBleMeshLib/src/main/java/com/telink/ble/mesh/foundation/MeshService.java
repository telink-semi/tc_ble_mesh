/********************************************************************************************************
 * @file     MeshService.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *           
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *           
 *******************************************************************************************************/
package com.telink.ble.mesh.foundation;

import android.content.Context;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.message.MeshMessage;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.parameter.AutoConnectParameters;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.foundation.parameter.FastProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.GattOtaParameters;
import com.telink.ble.mesh.foundation.parameter.MeshOtaParameters;
import com.telink.ble.mesh.foundation.parameter.ProvisioningParameters;
import com.telink.ble.mesh.foundation.parameter.ScanParameters;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import org.spongycastle.jcajce.provider.asymmetric.ec.BCECPrivateKey;
import org.spongycastle.jcajce.provider.asymmetric.ec.BCECPublicKey;
import org.spongycastle.jce.spec.ECNamedCurveSpec;
import org.spongycastle.util.BigIntegers;

import java.math.BigInteger;
import java.security.KeyPair;
import java.security.PrivateKey;
import java.security.Security;
import java.security.spec.ECFieldFp;

import androidx.annotation.NonNull;

/**
 * Created by kee on 2019/8/26.
 */

public class MeshService implements MeshController.EventCallback {

    // mesh encipher
    static {
        Security.insertProviderAt(new org.spongycastle.jce.provider.BouncyCastleProvider(), 1);
    }

    private MeshController mController;

    private static MeshService mThis = new MeshService();

    public static MeshService getInstance() {
        return mThis;
    }

    private EventHandler mEventHandler;

    public void init(@NonNull Context context, EventHandler eventHandler) {
        MeshLogger.log("MeshService#init");
        if (mController == null) {
            mController = new MeshController();
        }
        mController.setEventCallback(this);
        mController.start(context);
        this.mEventHandler = eventHandler;
        testEcdh();
    }


    private void testEcdh() {
        byte[] content = Arrays.hexToBytes("2DE10AB359AA54B7D5CFFEB0262FE6AD");
        byte[] key = Arrays.hexToBytes("6EE32FB8FC878EC6E20725481E675242");
        byte[] aes = Encipher.aesCmac(content, key);
        MeshLogger.d("aes: " + Arrays.bytesToHexString(aes));

        byte[] salt = Encipher.generateSalt("smk2".getBytes());
        MeshLogger.d("salt: " + Arrays.bytesToHexString(salt));

        byte[] netkey = new byte[]{0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00};
//        byte[] k3 = Encipher.k3();
//        byte k4 = Encipher.k4(netkey);
//        MeshLogger.d("k4" + k4);
//        MeshLogger.d("k3: " + Arrays.bytesToHexString(k3));
        byte[] netId = Encipher.generateIdentityKey(netkey);
        MeshLogger.d("netId: " + Arrays.bytesToHexString(netId));


        byte[] random = Arrays.hexToBytes("1122334455667788");
        byte[] identity = Encipher.generateNodeIdentityHash(netId, random, 2);
        MeshLogger.d("identity: " + Arrays.bytesToHexString(identity));

        byte[] beaconKey = Encipher.generateBeaconKey(netkey);
        MeshLogger.d("beaconKey: " + Arrays.bytesToHexString(beaconKey));

        // aes: 314DE886AC5A48B2E897933FB4BF0D2D
        // aesCMAC: 7C1FD960385E14A2150F5C0697FFBE39
        /*KeyPair keyPair = Encipher.generateKeyPair();

        BCECPublicKey publicKey = (BCECPublicKey) keyPair.getPublic();
        byte[] pubX = publicKey.getQ().getXCoord().getEncoded();
        MeshLogger.d("public key x hex: " + Arrays.bytesToHexString(pubX));
        MeshLogger.d("public key x d: " + BigIntegers.fromUnsignedByteArray(pubX).toString());
        byte[] pubY = publicKey.getQ().getYCoord().getEncoded();
        MeshLogger.d("public key y hex: " + Arrays.bytesToHexString(pubY));
        MeshLogger.d("public key y d: " + BigIntegers.fromUnsignedByteArray(pubY).toString());

        BCECPrivateKey privateKey = (BCECPrivateKey) keyPair.getPrivate();
        BigInteger d = privateKey.getD();

        byte[] db = BigIntegers.asUnsignedByteArray(d);
        MeshLogger.d("private key d: " + d.toString());
        MeshLogger.d("private key db: " + Arrays.bytesToHexString(db));

        BigInteger a = privateKey.getParams().getCurve().getA();
        byte[] ab = BigIntegers.asUnsignedByteArray(a);
        MeshLogger.d("private key ab: " + Arrays.bytesToHexString(ab));

        BigInteger b = privateKey.getParams().getCurve().getB();
        byte[] bb = BigIntegers.asUnsignedByteArray(b);
        MeshLogger.d("private key bb: " + Arrays.bytesToHexString(bb));

        BigInteger gx = privateKey.getParams().getGenerator().getAffineX();
        byte[] gxb = BigIntegers.asUnsignedByteArray(gx);
        MeshLogger.d("private key gxb: " + Arrays.bytesToHexString(gxb));

        BigInteger gy = privateKey.getParams().getGenerator().getAffineY();
        byte[] gyb = BigIntegers.asUnsignedByteArray(gy);
        MeshLogger.d("private key gyb: " + Arrays.bytesToHexString(gyb));

        ECFieldFp fieldFp = (ECFieldFp) privateKey.getParams().getCurve().getField();
        BigInteger p = fieldFp.getP();
        byte[] pb = BigIntegers.asUnsignedByteArray(p);
        MeshLogger.d("private key pb: " + Arrays.bytesToHexString(pb));

        ECNamedCurveSpec namedCurveSpec = (ECNamedCurveSpec) privateKey.getParams();
        BigInteger order = namedCurveSpec.getOrder();
        byte[] orderB = BigIntegers.asUnsignedByteArray(order);
        MeshLogger.d("private key orderB: " + Arrays.bytesToHexString(orderB));

        byte[] xy = Arrays.hexToBytes("6EE32FB8FC878EC6E20725481E6752422DE10AB359AA54B7D5CFFEB0262FE6AD7EC9632BF7E3C9836370329E045C94F0CF73B7F0E356115E7C2B167E33EF403E");


        BigInteger x = BigIntegers.fromUnsignedByteArray(xy, 0, 32);
        BigInteger y = BigIntegers.fromUnsignedByteArray(xy, 32, 32);
        MeshLogger.d("x " + x.toString());
        MeshLogger.d("y " + y.toString());
        byte[] ecdhSec = Encipher.generateECDH(xy, privateKey);

        BigInteger ecdhSecV = BigIntegers.fromUnsignedByteArray(ecdhSec);
        MeshLogger.d("ecdh sec v: " + ecdhSecV.toString());
        MeshLogger.d("ecdh sec: " + Arrays.bytesToHexString(ecdhSec));*/
    }

    public void clear() {
        MeshLogger.log("MeshService#clear");
        if (this.mController != null) {
            this.mController.stop();
        }
    }


    public void setupMeshNetwork(MeshConfiguration configuration) {
        mController.setupMeshNetwork(configuration);
    }

    /**
     * @return is proxy connected && proxy set success (if proxy filter set needed)
     */
    public boolean isProxyLogin() {
        return mController.isProxyLogin();
    }

    /**
     * @return direct connected node address,
     * if 0 : invalid address
     */
    public int getDirectConnectedNodeAddress() {
        return mController.getDirectNodeAddress();
    }

    public void removeDevice(int meshAddress) {
        mController.removeDevice(meshAddress);
    }

    public MeshController.Mode getCurrentMode() {
        return mController.getMode();
    }

    /********************************************************************************
     * mesh api
     ********************************************************************************/

    /**
     * start scanning
     */
    public void startScan(ScanParameters scanParameters) {
        mController.startScan(scanParameters);
    }

    public void stopScan() {
        mController.stopScan();
    }

    /**
     * start provisioning if device found by {@link #startScan(ScanParameters)}
     */
    public boolean startProvisioning(ProvisioningParameters provisioningParameters) {
        return mController.startProvisioning(provisioningParameters);
    }

    /**
     * start binding application key for models in node if device provisioned
     */
    public void startBinding(BindingParameters bindingParameters) {
        mController.startBinding(bindingParameters);
    }

    /**
     * scanning an connecting proxy node for mesh control
     */
    public void autoConnect(AutoConnectParameters parameters) {
        mController.autoConnect(parameters);
    }

    /**
     * ota by gatt
     */
    public void startGattOta(GattOtaParameters otaParameters) {
        mController.startGattOta(otaParameters);
    }

    /**
     * ota by mesh
     * support multi node updating at the same time
     */
    public void startMeshOta(MeshOtaParameters meshOtaParameters) {
        mController.startMeshOTA(meshOtaParameters);
    }

    /**
     * stop mesh updating flow
     */
    public void stopMeshOta() {
        mController.stopMeshOta();
    }

    /**
     * remote provisioning when proxy node connected and RP-support device found
     */
    public void startRemoteProvisioning(RemoteProvisioningDevice remoteProvisioningDevice) {
        mController.startRemoteProvision(remoteProvisioningDevice);
    }

    public void startFastProvision(FastProvisioningParameters fastProvisioningConfiguration) {
        mController.startFastProvision(fastProvisioningConfiguration);
    }

    public void idle(boolean disconnect) {
        mController.idle(disconnect);
    }


    /**
     * send mesh message
     * 1. if message is reliable (with ack), message.responseOpcode should be valued by message ack opcode
     * {@link MeshMessage#getResponseOpcode()}
     * Besides, message.responseMax is suggested to be valued by expected response count,
     * for example, 3 nodes in group(0xC001), 3 is the best value for responseMax when get group status
     * 2. if message is with tid (for example: OnOffSetMessage {@link com.telink.ble.mesh.core.message.generic.OnOffSetMessage})
     * and App do not want to manage tid, valid message.tidPosition should be valued
     * otherwise tid in message will be sent,
     *
     * @param meshMessage message
     */
    public boolean sendMeshMessage(MeshMessage meshMessage) {
        return mController.sendMeshMessage(meshMessage);
    }

    /**
     * get all devices status
     *
     * @return if online_status supported
     */
    public boolean getOnlineStatus() {
        return mController.getOnlineStatus();
    }

    /********************************************************************************
     * bluetooth api
     ********************************************************************************/

    public boolean isBluetoothEnabled() {
        return mController.isBluetoothEnabled();
    }

    public void enableBluetooth() {
        mController.enableBluetooth();
    }

    /**
     * get current connected device macAddress
     */
    public String getCurDeviceMac() {
        return mController.getCurDeviceMac();
    }


    @Override
    public void onEventPrepared(Event<String> event) {
        if (mEventHandler != null) {
            mEventHandler.onEventHandle(event);
        }
    }

}
