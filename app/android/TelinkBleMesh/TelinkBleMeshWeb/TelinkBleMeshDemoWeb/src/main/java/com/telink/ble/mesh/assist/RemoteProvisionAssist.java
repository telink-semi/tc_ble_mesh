/********************************************************************************************************
 * @file DeviceProvisionActivity.java
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
package com.telink.ble.mesh.assist;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.JSONException;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.BindingBearer;
import com.telink.ble.mesh.core.message.config.ModelPublicationStatusMessage;
import com.telink.ble.mesh.entity.BindingDevice;
import com.telink.ble.mesh.entity.RemoteProvisioningDevice;
import com.telink.ble.mesh.foundation.Event;
import com.telink.ble.mesh.foundation.EventListener;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.foundation.event.BindingEvent;
import com.telink.ble.mesh.foundation.event.RemoteProvisioningEvent;
import com.telink.ble.mesh.foundation.parameter.BindingParameters;
import com.telink.ble.mesh.model.NetworkingDevice;
import com.telink.ble.mesh.model.NetworkingState;
import com.telink.ble.mesh.model.RemoteNetworkingDevice;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.util.Arrays;
import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;
import com.telink.ble.mesh.web.entity.MeshNetworkDetail;
import com.telink.ble.mesh.web.entity.MeshNode;

import java.io.IOException;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * Implement the provision and key bind processes
 */
public class RemoteProvisionAssist implements EventListener<String> {

    private static final String URL_APPLY_ADDRESS = TelinkHttpClient.URL_BASE + "mesh-node/applyNodeAddress";

    private static final String URL_ADD_NODE = TelinkHttpClient.URL_BASE + "mesh-node/addNode";

    /**
     * local mesh info
     */
    private MeshNetworkDetail mesh;

    private BaseActivity context;

    private RemoteNetworkingDevice processingDevice;

    private RemoteProvisionAssistCallback cb = null;

    public RemoteProvisionAssist(BaseActivity context) {
        this.context = context;
        this.mesh = TelinkMeshApplication.getInstance().getMeshInfo();
        addEventListeners();
    }

    public void clear() {
        TelinkMeshApplication.getInstance().removeEventListener(this);
        this.context = null;
        this.cb = null;
    }


    public void start(RemoteNetworkingDevice networkingDevice, RemoteProvisionAssistCallback cb) {
        this.processingDevice = networkingDevice;
        this.cb = cb;
        startRemoteProvision();
    }

    private void addEventListeners() {
        TelinkMeshApplication.getInstance().addEventListener(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_CAPABILITY_RECEIVED, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_SUCCESS, this);
        TelinkMeshApplication.getInstance().addEventListener(BindingEvent.EVENT_TYPE_BIND_FAIL, this);
        TelinkMeshApplication.getInstance().addEventListener(ModelPublicationStatusMessage.class.getName(), this);
    }

    // apply node address from cloud
    private void applyAddress(int elementCount) {
        int tcVendorId = processingDevice.uuidInfo.tcVendorId;
        int pid = processingDevice.uuidInfo.pid;
        FormBody formBody = new FormBody.Builder()
                .add("tcVendorId", tcVendorId + "")
                .add("pid", pid + "")
                .add("networkId", mesh.id + "")
                .add("elementCount", elementCount + "")
                .add("provisionerId", mesh.provisioner.id + "")
                .build();
        TelinkHttpClient.getInstance().post(URL_APPLY_ADDRESS, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                onApplyAddressError("call fail - " + URL_APPLY_ADDRESS);
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, context);
                if (responseInfo == null) {
                    onApplyAddressError("apply address: server response error");
                    return;
                }
                MeshLogger.d("responseInfo - " + responseInfo.toString());
                int address = Integer.parseInt(responseInfo.data);
                continueProvision(address);
            }
        });
    }

    private void onApplyAddressError(String desc) {
        processingDevice.state = NetworkingState.PROVISION_FAIL;
        processingDevice.addLog(NetworkingDevice.TAG_REMOTE_PV, desc);
        updateDeviceState();
        onComplete(false);
    }

    private void updateDeviceState() {
        if (cb != null) {
            cb.onDeviceStateUpdate(processingDevice);
        }
    }

    private void onError(String desc) {
        if (cb != null) {
            cb.onError(desc);
        }
    }

    private void onComplete(boolean success) {
        MeshLogger.d("remote prov - complete : - " + Arrays.bytesToHexString(processingDevice.remoteProvisioningDevice.getUuid()));
        if (cb != null) {
            cb.onComplete(success);
        }
    }

    private void continueProvision(int address) {
        if (!MeshUtils.validUnicastAddress(address)) {
            onError("invalid unicast address");
            return;
        }
        processingDevice.address = address;
        processingDevice.addLog(NetworkingDevice.TAG_REMOTE_PV, "alloc address: " + address);
        updateDeviceState();
        MeshService.getInstance().continueRemoteProvision(address);
    }

    private void startRemoteProvision() {
        processingDevice.state = NetworkingState.PROVISIONING;
        processingDevice.addLog(NetworkingDevice.TAG_REMOTE_PV, "action start -> address not alloc");
        updateDeviceState();
        MeshService.getInstance().startRemoteProvisioning(processingDevice.remoteProvisioningDevice);
    }

    @Override
    public void performed(final Event<String> event) {
        String eventType = event.getType();
        if (eventType.equals(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_FAIL)) {
            onRemoteProvisioningFail((RemoteProvisioningEvent) event);
            onComplete(false);
        } else if (eventType.equals(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_PROVISIONING_SUCCESS)) {
            onRemoteProvisioningSuccess((RemoteProvisioningEvent) event);
        } else if (eventType.equals(BindingEvent.EVENT_TYPE_BIND_SUCCESS)) {
            onKeyBindSuccess((BindingEvent) event);
            onComplete(true);
        } else if (eventType.equals(BindingEvent.EVENT_TYPE_BIND_FAIL)) {
            onKeyBindFail((BindingEvent) event);
            onComplete(false);
        } else if (event.getType().equals(RemoteProvisioningEvent.EVENT_TYPE_REMOTE_CAPABILITY_RECEIVED)) {
            onCapabilityReceived((RemoteProvisioningEvent) event);
        }
    }


    private void onRemoteProvisioningFail(RemoteProvisioningEvent event) {
        MeshLogger.log("remote act fail: " + Arrays.bytesToHexString(event.getRemoteProvisioningDevice().getUuid()));
        processingDevice.state = NetworkingState.PROVISION_FAIL;
        processingDevice.addLog("remote-provision", event.getDesc());
        updateDeviceState();
    }

    private void onRemoteProvisioningSuccess(RemoteProvisioningEvent event) {
        RemoteProvisioningDevice remote = event.getRemoteProvisioningDevice();
        processingDevice.state = NetworkingState.BINDING;
        processingDevice.addLog(NetworkingDevice.TAG_REMOTE_PV, "success");
        processingDevice.deviceKey = remote.getDeviceKey();
        updateDeviceState();
        addNodeToCloud();
    }


    private void onCapabilityReceived(RemoteProvisioningEvent event) {
        RemoteProvisioningDevice remote = event.getRemoteProvisioningDevice();
        int elementCount = remote.getDeviceCapability().eleNum & 0xFF;
        applyAddress(elementCount);
    }

    private void addNodeToCloud() {
        // add node
        int tcVendorId = processingDevice.uuidInfo.tcVendorId;
        int pid = processingDevice.uuidInfo.pid;

        MeshLogger.d("addNodeToCloud - " + processingDevice.address);
        FormBody formBody = new FormBody.Builder()
                .add("tcVendorId", tcVendorId + "")
                .add("pid", pid + "")
                .add("networkId", mesh.id + "")
                .add("deviceUUID", Arrays.bytesToHexString(processingDevice.deviceUUID))
                .add("name", "common node")
                .add("address", processingDevice.address + "")
                .add("provisionerId", mesh.provisioner.id + "")
                .add("deviceKey", Arrays.bytesToHexString(processingDevice.deviceKey))
                .build();
        TelinkHttpClient.getInstance().post(URL_ADD_NODE, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                context.showError("call fail - " + call.request().url());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, context);
                if (responseInfo == null) {
                    context.showError("fail: server response error");
                    return;
                }
                try {
                    MeshNode meshNode = JSON.parseObject(responseInfo.data, MeshNode.class);
                    mesh.insertDevice(meshNode);
                    processingDevice.nodeId = meshNode.id;
                    startKeyBind();
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private void startKeyBind() {
        // check if private mode opened
        processingDevice.addLog(NetworkingDevice.TAG_BIND, "action start");
        int appKeyIndex = mesh.getDefaultAppKeyIndex();
        byte[] deviceUUID = processingDevice.deviceUUID;
        // processingDevice.nodeId,
        BindingDevice bindingDevice = new BindingDevice(processingDevice.address, deviceUUID, appKeyIndex);
        boolean isKeyBindNeeded = processingDevice.uuidInfo.isKeyBindNeeded();
        bindingDevice.setDefaultBound(!isKeyBindNeeded);
        bindingDevice.setBearer(BindingBearer.Any);
        MeshService.getInstance().startBinding(new BindingParameters(bindingDevice));
    }

    private void onKeyBindFail(BindingEvent event) {
        processingDevice.state = NetworkingState.BIND_FAIL;
        processingDevice.addLog(NetworkingDevice.TAG_BIND, "failed - " + event.getDesc());
        updateDeviceState();
        onComplete(false);
    }

    private void onKeyBindSuccess(BindingEvent event) {
        BindingDevice remote = event.getBindingDevice();

        processingDevice.addLog(NetworkingDevice.TAG_BIND, "success");
        processingDevice.bindState = 1;
        // if is default bound, composition data has been valued ahead of binding action
        processingDevice.compositionData = remote.getCompositionData();
        // no need to set time publish
        processingDevice.state = NetworkingState.BIND_SUCCESS;
        mesh.saveBindState(processingDevice.nodeId, 1, remote.getCompositionData().vid);
        updateDeviceState();
        onComplete(true);
    }

    public interface RemoteProvisionAssistCallback {
        /**
         * new state is {@link NetworkingDevice#state}
         */
        void onDeviceStateUpdate(NetworkingDevice device);

        /**
         * If an error occurs, the current and upcoming networking actions need to be terminated
         *
         * @param desc description
         */
        void onError(String desc);

        void onComplete(boolean success);
    }
}
