package com.telink.ble.mesh.model.web;

import com.telink.ble.mesh.util.MeshLogger;
import com.telink.ble.mesh.web.ResponseInfo;
import com.telink.ble.mesh.web.TelinkHttpClient;
import com.telink.ble.mesh.web.WebUtils;

import java.io.IOException;
import java.io.Serializable;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.Response;

/**
 * <p>
 *
 * </p>
 *
 * @author kee
 * @since 2023-12-19
 */
public class MeshNodeConfig implements Serializable {

    public final static String URL_UPDATE_CONFIGS = TelinkHttpClient.URL_BASE + "node-config/updateConfigs";

    private static final long serialVersionUID = 1L;

    public Integer id;

    public Integer nodeId;

    public Integer defaultTtl;

    public Integer relay;

    public Integer relayRetransmit;

    public Integer secureNetworkBeacon;

    public Integer gattProxy;

    public Integer privateBeacon;

    public Integer privateGattProxy;

    public Integer friend;

    public Integer networkRetransmit;

    public Integer onDemandPrivateGattProxy;

    public Integer directForwarding;

    public Integer directRelay;

    public Integer directProxy;

    public Integer directFriend;


    public void resetDefaultTtl(Integer defaultTtl) {
        this.defaultTtl = defaultTtl;
        updateConfigToCloud("ttl", defaultTtl + "");
    }


    public void resetRelay(Integer relay) {
        this.relay = relay;
        updateConfigToCloud("relay", relay + "");
    }


    public void resetRelayRetransmit(Integer relayRetransmit) {
        this.relayRetransmit = relayRetransmit;
        updateConfigToCloud("relayRetransmit", relayRetransmit + "");
    }

    public void resetSecureNetworkBeacon(Integer secureNetworkBeacon) {
        this.secureNetworkBeacon = secureNetworkBeacon;
        updateConfigToCloud("secureNetworkBeacon", secureNetworkBeacon + "");
    }


    public void resetGattProxy(Integer gattProxy) {
        this.gattProxy = gattProxy;
        updateConfigToCloud("gattProxy", gattProxy + "");
    }


    public void resetPrivateBeacon(Integer privateBeacon) {
        this.privateBeacon = privateBeacon;
        updateConfigToCloud("privateBeacon", privateBeacon + "");
    }


    public void resetPrivateGattProxy(Integer privateGattProxy) {
        this.privateGattProxy = privateGattProxy;
        updateConfigToCloud("privateGattProxy", privateGattProxy + "");
    }

    public void resetFriend(Integer friend) {
        this.friend = friend;
        updateConfigToCloud("friend", friend + "");
    }

    public void resetNetworkRetransmit(Integer networkRetransmit) {
        this.networkRetransmit = networkRetransmit;
        updateConfigToCloud("networkRetransmit", networkRetransmit + "");
    }

    public void resetOnDemandPrivateGattProxy(Integer onDemandPrivateGattProxy) {
        this.onDemandPrivateGattProxy = onDemandPrivateGattProxy;
        updateConfigToCloud("onDemandPrivateGattProxy", onDemandPrivateGattProxy + "");
    }

    public void resetDirectForwarding(Integer directForwarding) {
        this.directForwarding = directForwarding;
        updateConfigToCloud("directForwarding", directForwarding + "");
    }

    public void resetDirectRelay(Integer directRelay) {
        this.directRelay = directRelay;
        updateConfigToCloud("directRelay", directRelay + "");
    }

    public void resetDirectProxy(Integer directProxy) {
        this.directProxy = directProxy;
        updateConfigToCloud("directProxy", directProxy + "");
    }

    public void resetDirectFriend(Integer directFriend) {
        this.directFriend = directFriend;
        updateConfigToCloud("directFriend", directFriend + "");
    }

    private void updateConfigToCloud(String key, String newValue) {
        MeshLogger.d("updateConfigToCloud - " + key + " -- " + newValue);
        FormBody formBody = new FormBody.Builder()
                .add("configId", id + "")
                .add(key, newValue).build();
        TelinkHttpClient.getInstance().post(URL_UPDATE_CONFIGS, formBody, new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                MeshLogger.w("updateConfigToCloud error :" + e.getMessage());
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                ResponseInfo responseInfo = WebUtils.parseResponse(response, null);
                if (responseInfo == null) {
                    MeshLogger.w("fail: server response error - updateConfigToCloud");
                    return;
                }
                MeshLogger.d("updateC complete - " + key + " -- " + newValue);
            }
        });

    }


    @Override
    public String toString() {
        return "MeshNodeConfig{" +
                "id=" + id +
                ", nodeId=" + nodeId +
                ", defaultTtl=" + defaultTtl +
                ", relay=" + relay +
                ", relayRetransmit=" + relayRetransmit +
                ", secureNetworkBeacon=" + secureNetworkBeacon +
                ", gattProxy=" + gattProxy +
                ", privateBeacon=" + privateBeacon +
                ", privateGattProxy=" + privateGattProxy +
                ", friend=" + friend +
                ", networkRetransmit=" + networkRetransmit +
                ", onDemandPrivateGattProxy=" + onDemandPrivateGattProxy +
                ", directForwarding=" + directForwarding +
                ", directRelay=" + directRelay +
                ", directProxy=" + directProxy +
                ", directFriend=" + directFriend +
                "}";
    }
}
