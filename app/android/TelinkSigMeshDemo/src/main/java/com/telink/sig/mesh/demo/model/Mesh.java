/********************************************************************************************************
 * @file Mesh.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.demo.model;


import android.content.Context;

import com.telink.sig.mesh.demo.FileSystem;
import com.telink.sig.mesh.demo.model.json.ProvisionerNode;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Group;
import com.telink.sig.mesh.model.SigMeshModel;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class Mesh implements Serializable, Cloneable {

    private static final long serialVersionUID = 1L;

    public static final String STORAGE_NAME = "telink_sig_mesh";

    public byte[] networkKey;

    public int netKeyIndex = 0x00;

    public byte[] appKey;

    public int appKeyIndex = 0;

    public byte ivUpdateFlag = 0;

    // provisioner sequence number
    public int sno = 0;

    public int ivIndex = 0;

    public int localAddress;

    public List<Group> groups = new ArrayList<>();

    public List<DeviceInfo> devices = new ArrayList<>();

    public int pvIndex = 1;

    public List<SigMeshModel> selectedModels = new ArrayList<>();

    public List<Scene> scenes = new ArrayList<>();

    /**
     * mesh uuid, should be compared with MeshStorage.meshUUID when importing
     */
    public String uuid;

    public String provisionerUUID;

    public AddressRange unicastRange;

    /**
     * provisioner nodes
     */
    public List<ProvisionerNode> provisionerNodes = new ArrayList<>();

//    public AddressRange groupRange;

//    public AddressRange sceneRange;

    public void saveScene(Scene scene) {
        for (Scene local : scenes) {
            if (local.id == scene.id) {
                local.states = scene.states;
                return;
            }
        }
        scenes.add(scene);
    }

    public Scene getSceneById(int id) {
        for (Scene scene : scenes) {
            if (id == scene.id) {
                return scene;
            }
        }
        return null;
    }

    /**
     * 1-0xFFFF
     *
     * @return -1 invalid id
     */
    public int allocSceneId() {
        if (scenes.size() == 0) {
            return 1;
        }
        int id = scenes.get(scenes.size() - 1).id;
        if (id == 0xFFFF) {
            return -1;
        }
        return id + 1;
    }

    public DeviceInfo getDeviceByMeshAddress(int meshAddress) {
        if (this.devices == null)
            return null;

        for (DeviceInfo info : devices) {
            if (info.meshAddress == meshAddress)
                return info;
        }
        return null;
    }

    public DeviceInfo getDeviceByMacAddress(String address) {
        for (DeviceInfo info : devices) {
            if (info.macAddress.equals(address))
                return info;
        }
        return null;
    }

    public void insertDevice(DeviceInfo deviceInfo) {
        DeviceInfo local = getDeviceByMacAddress(deviceInfo.macAddress);
        if (local != null) {
            this.removeDeviceByMac(deviceInfo.macAddress);
        }
        devices.add(deviceInfo);
    }


    public boolean removeDeviceByMeshAddress(int address) {

        if (this.devices == null || this.devices.size() == 0) return false;

        for (Scene scene : scenes) {
            scene.removeByAddress(address);
        }

        Iterator<DeviceInfo> iterator = devices.iterator();
        while (iterator.hasNext()) {
            DeviceInfo deviceInfo = iterator.next();

            if (deviceInfo.meshAddress == address) {
                iterator.remove();
                return true;
            }
        }

        return false;
    }

    public boolean removeDeviceByMac(String address) {

        if (this.devices == null || this.devices.size() == 0) return false;
        Iterator<DeviceInfo> iterator = devices.iterator();
        while (iterator.hasNext()) {
            DeviceInfo deviceInfo = iterator.next();
            if (deviceInfo.macAddress.endsWith(address)) {
                iterator.remove();
                return true;
            }
        }

        return false;
    }

    public void saveOrUpdate(Context context) {
//        MeshStorageService.getInstance().saveFromLocalMesh(this, context);
        FileSystem.writeAsObject(context, STORAGE_NAME, this);
    }

    // 获取所有在线的设备个数
    public int getOnlineCountInAll() {
        if (devices == null || devices.size() == 0) {
            return 0;
        }

        int result = 0;
        for (DeviceInfo device : devices) {
            if (device.getOnOff() != -1) {
                result++;
            }
        }

        return result;
    }

    // 获取指定分组的在线设备个数
    public int getOnlineCountInGroup(int groupAddr) {
        if (devices == null || devices.size() == 0) {
            return 0;
        }
        int result = 0;
        for (DeviceInfo device : devices) {
            if (device.getOnOff() != -1) {
                for (int addr : device.subList) {
                    if (addr == groupAddr) {
                        result++;
                        break;
                    }
                }
            }
        }

        return result;
    }

    @Override
    public Object clone() throws CloneNotSupportedException {
        return super.clone();
    }
}
