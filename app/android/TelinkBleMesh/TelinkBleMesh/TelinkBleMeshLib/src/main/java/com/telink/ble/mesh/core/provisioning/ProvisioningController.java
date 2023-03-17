/********************************************************************************************************
 * @file ProvisioningController.java
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
package com.telink.ble.mesh.core.provisioning;

import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.SparseArray;

import androidx.annotation.NonNull;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningCapabilityPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningConfirmPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningDataPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningInvitePDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningPubKeyPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRandomPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRecordRequestPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRecordResponsePDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRecordsGetPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningRecordsListPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningStartPDU;
import com.telink.ble.mesh.core.provisioning.pdu.ProvisioningStatePDU;
import com.telink.ble.mesh.core.proxy.ProxyPDU;
import com.telink.ble.mesh.entity.ProvisioningDevice;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;

import org.spongycastle.jcajce.provider.asymmetric.ec.BCECPublicKey;

import java.nio.ByteBuffer;
import java.security.KeyPair;
import java.security.cert.X509Certificate;

/**
 * provisioning
 * Auth Method inputOOB outputOOB is not supported
 * Created by kee on 2019/7/31.
 */

public class ProvisioningController {
    private final String LOG_TAG = "Provisioning";

    /**
     * provisioning state
     * indicates current state in provisioning flow
     * init state {@link #STATE_IDLE} means not in provisioning flow
     * direction introduction
     * P: provisioner, D: device
     * invite(P->D)
     * =>
     * capability(D->P)
     * =>
     * start(P->D)
     * =>
     * pub_key(P->D)
     * =>
     * pub_key(D->P) (if use oob public key, skip)
     * =>
     * confirm(P->D)
     * =>
     * confirm(D->P)
     * =>
     * random(P->D)
     * =>
     * random(D->P)
     * =>
     * check confirm
     * =>
     * provisioning end
     */
    private int state = STATE_IDLE;

    /**
     * not in provisioning flow
     */
    public static final int STATE_IDLE = 0x1000;

    public static final int STATE_RECORDS_GET = 0x0010;

    public static final int STATE_RECORD_REQUEST = 0x0011;

    /**
     * sent provisioning invite pdu
     */
    public static final int STATE_INVITE = 0x1001;

    /**
     * received provisioning capability
     */
    public static final int STATE_CAPABILITY = 0x1002;

    /**
     * sent provisioning start
     */
    public static final int STATE_START = 0x1003;

    /**
     * sent provisioning pub key
     */
    public static final int STATE_PUB_KEY_SENT = 0x1004;

    /**
     * received provisioning pub key
     */
    public static final int STATE_PUB_KEY_RECEIVED = 0x1005;


//    public static final int STATE_INPUT_COMPLETE = 0x1005;

    /**
     * sent provisioning confirm
     */
    public static final int STATE_CONFIRM_SENT = 0x1006;

    /**
     * received provisioning confirm
     */
    public static final int STATE_CONFIRM_RECEIVED = 0x1007;

    /**
     * sent provisioning random
     */
    public static final int STATE_RANDOM_SENT = 0x1008;

    /**
     * received provisioning random
     */
    public static final int STATE_RANDOM_RECEIVED = 0x1009;

    /**
     * sent provisioning data
     */
    public static final int STATE_DATA = 0x100A;

    /**
     * received provisioning complete, success!
     */
    public static final int STATE_COMPLETE = 0x100B;

    /**
     * received provisioning fail, or params check err!
     */
    public static final int STATE_FAILED = 0x100C;


    private static final byte[] AUTH_NO_OOB = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    private static final byte[] AUTH_NO_OOB_EPA = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    private static final long TIMEOUT_PROVISIONING = 60 * 1000;


    /**
     * Certificate-based Provisioning URI
     */
    private static final int RECORD_ID_URI = 0x0000;

    /**
     * Device Certificate
     */
    private static final int RECORD_ID_DEVICE = 0x0001;


    /**
     * Intermediate Certificate
     * 0x0002 or larger
     */
    private static final int RECORD_ID_INTERMEDIATE = 0x0002;

    /**
     * certificate record fragment size
     */
    private static final int MAX_FRAGMENT_SIZE = 20;

    private Handler delayHandler;

    private ProvisioningBridge mProvisioningBridge;

    private int recordId = -1;

    private int recordIndex = -1;

    private SparseArray<byte[]> recordContents = new SparseArray<>();

    private int fragmentOffset = 0;

    private byte[] recordData;

    /**
     * public key in certificate get by record response data
     */
    private byte[] recordPubKey;

    private ProvisioningRecordsListPDU recordsListPDU;

    private ProvisioningInvitePDU invitePDU;

    private ProvisioningStartPDU startPDU;

    private ProvisioningCapabilityPDU pvCapability;

    private ProvisioningPubKeyPDU provisionerPubKeyPDU;

    private ProvisioningPubKeyPDU devicePubKeyPDU;

    private KeyPair provisionerKeyPair;

    private byte[] deviceECDHSecret;

    private byte[] provisionerRandom;

    private byte[] provisionerConfirm;

    private byte[] deviceRandom;

    private byte[] deviceConfirm;


//    private byte[] deviceKey;


//    private ProvisioningParams mProvisioningParams;

    /**
     * target device
     */
    private ProvisioningDevice mProvisioningDevice;

    // deprecated
//    private boolean encAuth;

    //    private byte[] deviceCertData;

    public static final byte[] ROOT_CERT_IN_PTS = Arrays.hexToBytes("308202873082022EA003020102020101300A06082A8648CE3D04030230819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533365A170D3330313030343138353533365A30819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D3059301306072A8648CE3D020106082A8648CE3D03010703420004D183194D0257D2141D3C5566639B4F7AF0834945349B7207DDDA730693FD2B56B8A83AC49FD22517D28D0EED9AE3F1D43A221FE37919B66E9418FF9618C2081EA35D305B301D0603551D0E041604142556CB5D177EFA709C7E05CCB7418A3B714C0A77301F0603551D230418301680142556CB5D177EFA709C7E05CCB7418A3B714C0A77300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D040302034700304402207C9696D079CB866BEA5EAAC230FB52EB5BC8EFC72F46E25F7B1E7990401BC74202206B6FD9F0DBAC54D4121045FD0E4AC06D5F3306BF8DCAF32F2D701C1445A62EF8");

    // root cert in pts
    private byte[] rootCert = ROOT_CERT_IN_PTS;
    //HW_ROOT_CERT =  Arrays.hexToBytes("30820221308201C9A0030201020214738723BD08D202CB5C2FCB7F1B45CF5D14A292C6300A06082A8648CE3D0403023061310B3009060355040613026161310B300906035504080C026161310B300906035504070C026161310B3009060355040A0C026161310B3009060355040B0C026161310B300906035504030C0261613111300F06092A864886F70D010901160261613020170D3231303731343130343534335A180F32313231303632303130343534335A3061310B3009060355040613026161310B300906035504080C026161310B300906035504070C026161310B3009060355040A0C026161310B3009060355040B0C026161310B300906035504030C0261613111300F06092A864886F70D010901160261613059301306072A8648CE3D020106082A8648CE3D030107034200047CA0DF06B773604D9C41E68B89D375B39D8369B159D903AB41EA3744F61F0E6C1A35A19A1B618D36475E429635DDFEAEBD0E495DDF89B962124D52658E73AFC6A35D305B301D0603551D0E04160414584EBEF28F1459636393B7FE45C3EFBE317BD944301F0603551D23041830168014584EBEF28F1459636393B7FE45C3EFBE317BD944300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D0403020346003043021F5D623DB722C87AD2416906A5EBC031AB76C79B2C146AEF765AD309BDBF822002203DE73217AD01E72AF3425E6465E15952516040C346F195FD02D10DE54DA4F740");

    public ProvisioningController(HandlerThread handlerThread) {
        this.delayHandler = new Handler(handlerThread.getLooper());
    }

    public void setProvisioningBridge(ProvisioningBridge provisioningBridge) {
        this.mProvisioningBridge = provisioningBridge;
    }

    public ProvisioningDevice getProvisioningDevice() {
        return mProvisioningDevice;
    }

    public void begin(@NonNull ProvisioningDevice device) {
        log("begin -- " + Arrays.bytesToHexString(device.getDeviceUUID()));
        this.mProvisioningDevice = device;
        if (device.getRootCert() != null) {
            this.rootCert = device.getRootCert();
        } else {
            this.rootCert = ROOT_CERT_IN_PTS;
        }
        delayHandler.removeCallbacks(provisioningTimeoutTask);
        delayHandler.postDelayed(provisioningTimeoutTask, TIMEOUT_PROVISIONING);

//         draft feature
        provisionInvite();
    }

    public void clear() {
        if (delayHandler != null) {
            delayHandler.removeCallbacks(provisioningTimeoutTask);
        }
        this.state = STATE_IDLE;
    }

    public void pushNotification(byte[] provisioningPdu) {
        if (state == STATE_IDLE) {
            log("received notification when idle", MeshLogger.LEVEL_WARN);
            return;
        }
        log("provisioning pdu received: " + Arrays.bytesToHexString(provisioningPdu, ""));
        int provisioningPduType = provisioningPdu[0];
        byte[] provisioningData = new byte[provisioningPdu.length - 1];
        System.arraycopy(provisioningPdu, 1, provisioningData, 0, provisioningData.length);
        switch (provisioningPduType) {
            case ProvisioningPDU.TYPE_CAPABILITIES:
                onCapabilityReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_PUBLIC_KEY:
                onPubKeyReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_CONFIRMATION:
                onConfirmReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_RANDOM:
                onRandomReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_COMPLETE:
                onProvisionSuccess();
                break;
            case ProvisioningPDU.TYPE_FAILED:
                onProvisionFail("failed notification received");
                break;

            case ProvisioningPDU.TYPE_RECORDS_LIST:
                onRecordListReceived(provisioningData);
                break;

            case ProvisioningPDU.TYPE_RECORD_RESPONSE:
                onRecordResponse(provisioningData);
                break;
        }
    }

    private void onProvisionSuccess() {
        updateProvisioningState(STATE_COMPLETE, "Provision Success");
        onProvisionComplete();
    }

    private void onProvisionComplete() {
        this.state = STATE_IDLE;
        delayHandler.removeCallbacks(provisioningTimeoutTask);
    }

    private byte[] getAuthValue() {
        if (pvCapability.isStaticOOBSupported() && mProvisioningDevice.getAuthValue() != null) {
            return mProvisioningDevice.getAuthValue();
        } else {
            if (pvCapability.isHMacAlgorithmSupported()) {
                return AUTH_NO_OOB_EPA;
            }
            return AUTH_NO_OOB;
        }
    }

    private void updateProvisioningState(int state, String desc) {
        log("provisioning state update: state -- " + state + " desc -- " + desc);
        this.state = state;
        if (mProvisioningBridge != null) {
            mProvisioningBridge.onProvisionStateChanged(state, desc);
        }
    }

    /**
     * @see #onRecordListReceived(byte[])
     */
    private void provisionRecordsGet() {
        updateProvisioningState(STATE_RECORDS_GET, "Records Get");
        ProvisioningRecordsGetPDU recordsGetPDU = new ProvisioningRecordsGetPDU();
        sendProvisionPDU(recordsGetPDU);
    }


    private void provisionRecordRequest() {
        log(String.format("Record Request recordID=%04X offset=%04X", recordId, fragmentOffset));
        ProvisioningRecordRequestPDU recordRequestPDU =
                new ProvisioningRecordRequestPDU(recordId, fragmentOffset, MAX_FRAGMENT_SIZE);
        sendProvisionPDU(recordRequestPDU);
    }

    /**
     * invite
     */
    private void provisionInvite() {
        byte attention = 0;
        invitePDU = new ProvisioningInvitePDU(attention);
        updateProvisioningState(STATE_INVITE, "Invite");
        sendProvisionPDU(invitePDU);
    }


    private void provisionStart(boolean isStaticOOB) {
        final boolean useOOBPublicKey = recordPubKey != null;
        startPDU = ProvisioningStartPDU.getSimple(useOOBPublicKey, isStaticOOB);
        startPDU.setPublicKey(pvCapability.publicKeyType == 1 && recordPubKey != null);
        byte algo = (byte) (pvCapability.isHMacAlgorithmSupported() ? 1 : 0);
        startPDU.setAlgorithm(algo);
        updateProvisioningState(STATE_START, "Start - use static oob?" + isStaticOOB + " - algo:" + algo);
        sendProvisionPDU(startPDU);
    }

    private void provisionSendPubKey() {
        ProvisioningPubKeyPDU pubKeyPDU = getPublicKey();
        updateProvisioningState(STATE_PUB_KEY_SENT, "Send Public Key");
        sendProvisionPDU(pubKeyPDU);
    }

    public void onCapabilityReceived(byte[] capData) {
        if (this.state != STATE_INVITE) {
            log(" capability received when not inviting", MeshLogger.LEVEL_WARN);
            return;
        }

        pvCapability = ProvisioningCapabilityPDU.fromBytes(capData);
        updateProvisioningState(STATE_CAPABILITY, "Capability Received: " + pvCapability.toString());
        mProvisioningDevice.setDeviceCapability(pvCapability);
        boolean useStaticOOB = pvCapability.isStaticOOBSupported();
        if (useStaticOOB && mProvisioningDevice.getAuthValue() == null) {
            if (mProvisioningDevice.isAutoUseNoOOB()) {
                // use no oob
                useStaticOOB = false;
            } else {
                onProvisionFail("authValue not found when device static oob supported!");
                return;
            }
        }
        provisionStart(useStaticOOB);
        provisionSendPubKey();
        if (pvCapability.publicKeyType == 1 && recordPubKey != null) {
            onPubKeyReceived(recordPubKey);
        }
    }

    private void onPubKeyReceived(byte[] pubKeyData) {
        if (this.state != STATE_PUB_KEY_SENT) {
            log(" pub key received when not pub key sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_PUB_KEY_RECEIVED, "Public Key received");
        log("pub key received: " + Arrays.bytesToHexString(pubKeyData, ""));
        devicePubKeyPDU = ProvisioningPubKeyPDU.fromBytes(pubKeyData);
        deviceECDHSecret = Encipher.generateECDH(pubKeyData, provisionerKeyPair.getPrivate());
        if (deviceECDHSecret == null) {
            onProvisionFail("invalid public key");
            return;
        }
        log("get secret: " + Arrays.bytesToHexString(deviceECDHSecret, ""));
        sendConfirm();
    }


    private void sendConfirm() {
        final byte[] confirm = getConfirm();
        log("provisioner confirm - " + Arrays.bytesToHexString(confirm));
        provisionerConfirm = confirm;
        ProvisioningConfirmPDU confirmPDU = new ProvisioningConfirmPDU(provisionerConfirm);
        updateProvisioningState(STATE_CONFIRM_SENT, "Send confirm");
        sendProvisionPDU(confirmPDU);
    }

    /*
    If the Algorithm field is BTM_ECDH_P256_HMAC_SHA256_AES_CCM, the confirmation value of the Provisioner is a 256-bit value, the confirmation value of the device is a 256-bit value, and they are computed using:
ConfirmationProvisioner = HMAC-SHA-256ConfirmationKey (RandomProvisioner)
ConfirmationDevice = HMAC-SHA-256ConfirmationKey (RandomDevice)
Where:
ConfirmationKey = k5(ECDHSecret || AuthValue, ConfirmationSalt, “prck256”)
ConfirmationSalt = s2(ConfirmationInputs)
ConfirmationInputs = ProvisioningInvitePDUValue || ProvisioningCapabilitiesPDUValue || ProvisioningStartPDUValue || PublicKeyProvisioner || PublicKeyDevice

     */

    private byte[] getConfirm() {
        byte[] confirmInput = confirmAssembly();
        byte[] salt;
        byte[] confirmationKey;
        byte[] authValue = getAuthValue();

        // check alg
        if (isHmac()) {
            salt = Encipher.s2(confirmInput);
            // ConfirmationKey = k5(ECDHSecret || AuthValue, ConfirmationSalt, “prck256”)
            byte[] secAuth = ByteBuffer.allocate(deviceECDHSecret.length + authValue.length).put(deviceECDHSecret)
                    .put(authValue).array();
//            confirmationKey = Encipher.k5(deviceECDHSecret, salt, Encipher.PRCK256);
            confirmationKey = Encipher.k5(secAuth, salt, Encipher.PRCK256);
            provisionerRandom = Arrays.generateRandom(32);
            return Encipher.hMacSha256(provisionerRandom, confirmationKey);
        } else {
            salt = Encipher.generateSalt(confirmInput);
            confirmationKey = Encipher.k1(deviceECDHSecret, salt, Encipher.PRCK);
            provisionerRandom = Arrays.generateRandom(16);
            byte[] confirmData = new byte[provisionerRandom.length + authValue.length];
            System.arraycopy(provisionerRandom, 0, confirmData, 0, provisionerRandom.length);
            System.arraycopy(authValue, 0, confirmData, provisionerRandom.length, authValue.length);
            return Encipher.aesCmac(confirmData, confirmationKey);
        }
    }

    private boolean isHmac() {
        return pvCapability != null && pvCapability.isHMacAlgorithmSupported();
    }

    private void onConfirmReceived(byte[] confirm) {
        if (this.state != STATE_CONFIRM_SENT) {
            log(" confirm received when not confirm sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_CONFIRM_RECEIVED, "Confirm received");

        deviceConfirm = confirm;

        if (Arrays.equals(provisionerConfirm, deviceConfirm)) {
            onProvisionFail("received the same confirm as local");
            return;
        }

        sendRandom();
    }


    private void onRandomReceived(byte[] random) {
        if (this.state != STATE_RANDOM_SENT) {
            log(" random received when not random sent", MeshLogger.LEVEL_WARN);
            return;
        }

        updateProvisioningState(STATE_RANDOM_RECEIVED, "Random received");
        deviceRandom = random;


        if (Arrays.equals(provisionerRandom, deviceRandom)) {
            onProvisionFail("received the same random as local");
            return;
        }

        boolean pass = checkDeviceConfirm(random);
        if (pass) {
            sendProvisionData();
        } else {
            onProvisionFail("device confirm check err!");
        }
    }

    private void onRecordListReceived(byte[] recordsData) {
        if (state != STATE_RECORDS_GET) {
            log("record list received when not record list get", MeshLogger.LEVEL_WARN);
            return;
        }
        // 0D 0000 0000 0001
        this.recordsListPDU = ProvisioningRecordsListPDU.fromBytes(recordsData);
        if (recordsListPDU.recordsList.size() == 0) {
            onProvisionFail("Device Certificate not found");
            return;
        }

        this.recordIndex = 0;
        this.recordContents.clear();
        getNextRecord();
    }

    /**
     * @see #onAllRecordsComplete
     */
    private void getNextRecord() {
        log("get next record");
        if (recordIndex >= recordsListPDU.recordsList.size()) {
            onAllRecordsComplete();
            return;
        }
        this.recordId = recordsListPDU.recordsList.get(this.recordIndex);
        this.fragmentOffset = 0;
        this.recordData = null;
        this.recordPubKey = null;
        updateProvisioningState(STATE_RECORD_REQUEST, "Record Request");
        provisionRecordRequest();
    }

    private void onRecordResponse(byte[] recordResponseData) {
        if (state != STATE_RECORD_REQUEST) {
            log("record response received when not record request", MeshLogger.LEVEL_WARN);
            return;
        }
        ProvisioningRecordResponsePDU responsePDU = ProvisioningRecordResponsePDU.fromBytes(recordResponseData);
        log(responsePDU.toString());
        if (responsePDU.status != ProvisioningRecordResponsePDU.STATUS_SUCCESS || responsePDU.data == null) {
            onProvisionFail("record response error");
            return;
        }
        compositeResponseData(responsePDU.data);

        if (recordData.length >= responsePDU.totalLength) {
            onRecordResponseComplete();
        } else {
            fragmentOffset = fragmentOffset + responsePDU.data.length;
            provisionRecordRequest();
        }
    }


    private void compositeResponseData(byte[] newRecordData) {
        if (recordData == null) {
            recordData = newRecordData;
        } else {
//            recordData = ByteBuffer.allocate(recordData.length + newRecordData.length).put(recordData).put(newRecordData).array();
            byte[] re = new byte[recordData.length + newRecordData.length];
            System.arraycopy(recordData, 0, re, 0, recordData.length);
            System.arraycopy(newRecordData, 0, re, recordData.length, newRecordData.length);
            recordData = re;
        }
    }


    private void onRecordResponseComplete() {
        log("complete record: " + recordId + " -- " + Arrays.bytesToHexString(recordData));
        this.recordContents.put(recordId, recordData.clone());
        recordIndex++;
        getNextRecord();
    }

    private void onAllRecordsComplete() {
        if (recordContents.size() == 0) {
            onProvisionFail("no records found");
            return;
        }
        int firstRecordId = recordContents.keyAt(0);
//        firstRecordId = -1;
        if (firstRecordId == RECORD_ID_URI) {
            // contains Certificate-based Provisioning URI
            String uriStr = new String(recordContents.get(0));
            log("Certificate-based Provisioning URI found: " + uriStr);
            Uri uri = Uri.parse(uriStr);
            // skip HTTP retrieve
            // ptswebapiprod.azurewebsites.net/api/meshcert/v/a  for no-oob
            // ptswebapiprod.azurewebsites.net/api/meshcert/v/b  for static-oob
            byte[] ROOT_CERT = Arrays.hexToBytes("308202873082022EA003020102020101300A06082A8648CE3D04030230819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533365A170D3330313030343138353533365A30819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D3059301306072A8648CE3D020106082A8648CE3D03010703420004D183194D0257D2141D3C5566639B4F7AF0834945349B7207DDDA730693FD2B56B8A83AC49FD22517D28D0EED9AE3F1D43A221FE37919B66E9418FF9618C2081EA35D305B301D0603551D0E041604142556CB5D177EFA709C7E05CCB7418A3B714C0A77301F0603551D230418301680142556CB5D177EFA709C7E05CCB7418A3B714C0A77300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D040302034700304402207C9696D079CB866BEA5EAAC230FB52EB5BC8EFC72F46E25F7B1E7990401BC74202206B6FD9F0DBAC54D4121045FD0E4AC06D5F3306BF8DCAF32F2D701C1445A62EF8");
            byte[] DEVICE_CERT_NOOOB = Arrays.hexToBytes("30820250308201F6A003020102020104300A06082A8648CE3D04030230818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533375A170D3330313030343138353533375A3077310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303059301306072A8648CE3D020106082A8648CE3D03010703420004F465E43FF23D3F1B9DC7DFC04DA8758184DBC966204796ECCF0D6CF5E16500CC0201D048BCBBD899EEEFC424164E33C201C2B010CA6B4D43A8A155CAD8ECB279A35A305830090603551D1304023000300B0603551D0F040403020308301D0603551D0E04160414E262F3584AB688EC882EA528ED8E5C442A71369F301F0603551D230418301680144ABE293903A8BB49FF1D327CFEB80985F4109C21300A06082A8648CE3D0403020348003045022100E029814E909DD3D81C1DC4843FD27A56209212818765356699912382EFF9F46102201BE5E9E8801C868F7C44574F3AB7056C42B8083D6A39D519FD92FA4C17D81D52");
            byte[] DEVICE_CERT_STATICOOB = Arrays.hexToBytes("3082027F30820224A003020102020103300A06082A8648CE3D04030230818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533365A170D3330313030343138353533365A3077310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303059301306072A8648CE3D020106082A8648CE3D03010703420004F465E43FF23D3F1B9DC7DFC04DA8758184DBC966204796ECCF0D6CF5E16500CC0201D048BCBBD899EEEFC424164E33C201C2B010CA6B4D43A8A155CAD8ECB279A3818730818430090603551D1304023000300B0603551D0F040403020308301D0603551D0E04160414E262F3584AB688EC882EA528ED8E5C442A71369F301F0603551D230418301680144ABE293903A8BB49FF1D327CFEB80985F4109C21302A06146982E19DE491EAC0C283999CAA83FD8CC3D0D3670412041000000000000000000102030405060708300A06082A8648CE3D0403020349003046022100F7B504477EC2E5796644A0C5A95D864BF001CF96A5A180E243432CCE28FC5F9E0221008D816BEE11C36CDC1890189EDB85DF9A26998063EAC8EA55330B7F75003FEB98");
            byte[] WEBSITE_INTERMEDIATE_CERT = Arrays.hexToBytes("3082027A30820220A003020102020102300A06082A8648CE3D04030230819D310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453312D302B06035504030C2430303142444330382D313032312D304230452D304130432D3030304230453041304330303124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D301E170D3139303731383138353533365A170D3330313030343138353533365A30818F310B30090603550406130255533113301106035504080C0A57617368696E67746F6E31163014060355040A0C0D426C7565746F6F746820534947310C300A060355040B0C03505453311F301D06035504030C16496E7465726D65646961746520417574686F726974793124302206092A864886F70D0109011615737570706F727440626C7565746F6F74682E636F6D3059301306072A8648CE3D020106082A8648CE3D030107034200045463155F21C616953E8FAE69A9BCE445BE4FC8A3EB3412E7B2E7539D1CDBD86C64465F8F40A0A770A63C2AD1A51717DDE536F17865663252AB06B629E6BAE17EA35D305B301D0603551D0E041604144ABE293903A8BB49FF1D327CFEB80985F4109C21301F0603551D230418301680142556CB5D177EFA709C7E05CCB7418A3B714C0A77300C0603551D13040530030101FF300B0603551D0F040403020106300A06082A8648CE3D040302034800304502205C224FE15E7EE0FD2E499EC019D930950FF190E0E384D9B2FAB6259522E3EDF0022100D8846CC82D72EE1387875BB2B95C347776A2950F29A17E7863117203EEF0F5FE");
            validateDeviceCert(uriStr.endsWith("a") ? DEVICE_CERT_NOOOB : DEVICE_CERT_STATICOOB, WEBSITE_INTERMEDIATE_CERT, ROOT_CERT);
        } else {
            final byte[] deviceCertData = recordContents.get(RECORD_ID_DEVICE);
            final byte[] intermediateCertData = recordContents.get(RECORD_ID_INTERMEDIATE);
            if (deviceCertData == null) {
                onProvisionFail("device cert not found");
                return;
            }
            if (intermediateCertData == null) {
//                onProvisionFail("intermediate cert not found");
                log("check single device cert");
                validateDeviceCert(deviceCertData);
                return;
            }
            if (rootCert == null) {
                onProvisionFail("root cert not found");
                return;
            }
            validateDeviceCert(deviceCertData, intermediateCertData, rootCert);
        }
    }


    private void validateDeviceCert(byte[] deviceCertData) {
        // check intermediate  by root
        X509Certificate devCert = Encipher.checkCertificate(deviceCertData);
        if (devCert == null) {
            onProvisionFail("device cert verify fail");
        } else {
            recordPubKey = Encipher.getPublicKeyInCert(devCert);
            byte[] staticOOB = Encipher.getStaticOOBInCert(devCert);
            mProvisioningDevice.setAuthValue(staticOOB);
            log("public key in record: " + Arrays.bytesToHexString(recordPubKey));
            provisionInvite();
        }
    }


    private void validateDeviceCert(byte[] deviceCertData, byte[] intermediateCertData, byte[] rootCertData) {
        // check intermediate  by root
        X509Certificate devCert = Encipher.checkCertificateByCa(intermediateCertData, rootCertData);
        if (devCert == null) {
            onProvisionFail("intermediate cert verify fail");
            return;
        }

        X509Certificate certificate = Encipher.checkCertificateByCa(deviceCertData, intermediateCertData);
        if (certificate == null) {
            onProvisionFail("device cert verify fail");
        } else {
            recordPubKey = Encipher.getPublicKeyInCert(certificate);
            byte[] staticOOB = Encipher.getStaticOOBInCert(certificate);
            mProvisioningDevice.setAuthValue(staticOOB);
            log("public key in record - 1 : " + Arrays.bytesToHexString(recordPubKey));
            provisionInvite();
        }
    }

    private void onProvisionFail(String desc) {
        updateProvisioningState(STATE_FAILED, desc);
        onProvisionComplete();
    }

    private void sendProvisionData() {
        byte[] pvData = createProvisioningData();
        ProvisioningDataPDU provisioningDataPDU = new ProvisioningDataPDU(pvData);
        updateProvisioningState(STATE_DATA, "Send provisioning data");
        sendProvisionPDU(provisioningDataPDU);
    }

    private void sendRandom() {
        ProvisioningRandomPDU randomPDU = new ProvisioningRandomPDU(provisionerRandom);
        updateProvisioningState(STATE_RANDOM_SENT, "Send random");
        sendProvisionPDU(randomPDU);
    }


    private ProvisioningPubKeyPDU getPublicKey() {

        provisionerKeyPair = Encipher.generateKeyPair();
        if (provisionerKeyPair != null) {
            BCECPublicKey publicKey = (BCECPublicKey) provisionerKeyPair.getPublic();
            byte[] x = publicKey.getQ().getXCoord().getEncoded();
            byte[] y = publicKey.getQ().getYCoord().getEncoded();
            provisionerPubKeyPDU = new ProvisioningPubKeyPDU();
            provisionerPubKeyPDU.x = x;
            provisionerPubKeyPDU.y = y;
            log("get key x: " + Arrays.bytesToHexString(x, ":"));
            log("get key y: " + Arrays.bytesToHexString(y, ":"));
            return provisionerPubKeyPDU;
        } else {
            throw new RuntimeException("key pair generate err");
        }
    }

    // assemble random value
    public byte[] confirmAssembly() {
        byte[] inviteData = invitePDU.toBytes();
        byte[] capData = pvCapability.toBytes();
        byte[] startData = startPDU.toBytes();

        byte[] provisionerPubKey = provisionerPubKeyPDU.toBytes();

        byte[] devicePubKey = devicePubKeyPDU.toBytes();

        final int len = inviteData.length
                + capData.length
                + startData.length
                + provisionerPubKey.length
                + devicePubKey.length;
        ByteBuffer buffer = ByteBuffer.allocate(len);
        buffer.put(inviteData)
                .put(capData)
                .put(startData)
                .put(provisionerPubKey)
                .put(devicePubKey);

        return buffer.array();
    }

    private boolean checkDeviceConfirm(byte[] random) {

        byte[] confirmInput = confirmAssembly();
        byte[] salt;
        byte[] confirmationKey;
        byte[] authValue = getAuthValue();

        byte[] confirmationValue;
        if (isHmac()) {
            salt = Encipher.s2(confirmInput);
            byte[] secAuth = ByteBuffer.allocate(deviceECDHSecret.length + authValue.length).put(deviceECDHSecret)
                    .put(authValue).array();
            confirmationKey = Encipher.k5(secAuth, salt, Encipher.PRCK256);
            confirmationValue = Encipher.hMacSha256(deviceRandom, confirmationKey);
        } else {
            salt = Encipher.generateSalt(confirmInput);
            confirmationKey = Encipher.k1(deviceECDHSecret, salt, Encipher.PRCK);

            ByteBuffer buffer = ByteBuffer.allocate(random.length + authValue.length);
            buffer.put(random);
            buffer.put(authValue);
            byte[] confirmationData = buffer.array();
            confirmationValue = Encipher.aesCmac(confirmationData, confirmationKey);
        }

        if (java.util.Arrays.equals(confirmationValue, deviceConfirm)) {
            log("Confirmation values check pass");
            return true;
        } else {
            log("Confirmation values check err", MeshLogger.LEVEL_WARN);
        }

        return false;
    }

    private byte[] createProvisioningData() {

        byte[] confirmationInputs = confirmAssembly();

        byte[] confirmationSalt;
        if (isHmac()) {
            confirmationSalt = Encipher.s2(confirmationInputs);
        } else {
            confirmationSalt = Encipher.generateSalt(confirmationInputs);
        }

        // 96 bytes in epa
        ByteBuffer saltBuffer = ByteBuffer.allocate(confirmationSalt.length + provisionerRandom.length + deviceRandom.length);
        saltBuffer.put(confirmationSalt);
        saltBuffer.put(provisionerRandom);
        saltBuffer.put(deviceRandom);
        byte[] provisioningSaltInput = saltBuffer.array();
        byte[] provisioningSalt = Encipher.generateSalt(provisioningSaltInput);


        byte[] sessionKey = Encipher.k1(deviceECDHSecret, provisioningSalt, Encipher.PRSK);

        byte[] nonce = Encipher.k1(deviceECDHSecret, provisioningSalt, Encipher.PRSN);
        ByteBuffer nonceBuffer = ByteBuffer.allocate(nonce.length - 3);
        nonceBuffer.put(nonce, 3, nonceBuffer.limit());
        byte[] sessionNonce = nonceBuffer.array();

        byte[] deviceKey = Encipher.k1(deviceECDHSecret, provisioningSalt, Encipher.PRDK);
        mProvisioningDevice.setDeviceKey(deviceKey);

        log("device key: " + Arrays.bytesToHexString(deviceKey, ""));
        log("provisioning data prepare: " + mProvisioningDevice.toString());
        byte[] provisioningData = mProvisioningDevice.generateProvisioningData();

        log("unencrypted provision data: " + Arrays.bytesToHexString(provisioningData, ""));
        byte[] enData = Encipher.ccm(provisioningData, sessionKey, sessionNonce, 8, true);
        log("encrypted provision data: " + Arrays.bytesToHexString(enData, ""));
        return enData;
    }

    private void sendProvisionPDU(ProvisioningStatePDU pdu) {
        byte[] data = pdu.toBytes();

        byte[] re;
        if (data == null || data.length == 0) {
            re = new byte[]{pdu.getState()};
        } else {
            re = new byte[data.length + 1];
            re[0] = pdu.getState();
            System.arraycopy(data, 0, re, 1, data.length);
        }
        if (mProvisioningBridge != null) {
            log("pdu prepared: " + Arrays.bytesToHexString(data, ":"));
            mProvisioningBridge.onCommandPrepared(ProxyPDU.TYPE_PROVISIONING_PDU, re);
        }
    }


    private Runnable provisioningTimeoutTask = new Runnable() {
        @Override
        public void run() {
            onProvisionFail("provisioning timeout");
        }
    };

    private void log(String logMessage) {
        log(logMessage, MeshLogger.LEVEL_DEBUG);
    }

    private void log(String logMessage, int level) {
        MeshLogger.log(logMessage, LOG_TAG, level);
    }
}
