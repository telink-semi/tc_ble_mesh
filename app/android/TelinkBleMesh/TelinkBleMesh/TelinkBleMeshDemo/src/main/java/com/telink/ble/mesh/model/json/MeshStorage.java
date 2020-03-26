package com.telink.ble.mesh.model.json;


import com.telink.ble.mesh.entity.Scheduler;

import java.util.List;

/**
 * Created by kee on 2018/9/10.
 */

public class MeshStorage {

    interface Defaults {
        String Schema = "telink-semi.com";
        String Version = "V0.0.1";
        String MeshName = "Telink-Sig-Mesh";

        int IV_INDEX = 0;
    }

    public String $schema = Defaults.Schema;

    public String version = Defaults.Version;

    public String meshName = Defaults.MeshName;

    public String meshUUID;

    public String timestamp;

    public List<Provisioner> provisioners;

    public List<NetworkKey> netKeys;

    public List<ApplicationKey> appKeys;

    /**
     * 默认会存在一个本地的node， 即手机节点， UUID与provisioner UUID 一致
     */
    public List<Node> nodes;

    public List<Group> groups;

    public List<Scene> scenes;

    /**
     * 自定义数据
     */
    public String ivIndex = String.format("%08X", Defaults.IV_INDEX);

    public static class Provisioner {
        public String provisionerName;

        public String UUID;

        public List<AddressRange> allocatedUnicastRange;
        public List<AddressRange> allocatedGroupRange;
        public List<SceneRange> allocatedSceneRange;

        public static class AddressRange {
            public AddressRange(String lowAddress, String highAddress) {
                this.lowAddress = lowAddress;
                this.highAddress = highAddress;
            }

            public String lowAddress;
            public String highAddress;
        }

        public static class SceneRange {
            public SceneRange(int firstScene, int lastScene) {
                this.firstScene = firstScene;
                this.lastScene = lastScene;
            }

            public int firstScene;
            public int lastScene;
        }
    }

    public static class NetworkKey {
        public String name;

        // 0 -- 4095
        public int index;

        // 0,1,2
        public int phase;
        public String key;
        public String minSecurity;
        public String oldKey;
        public String timestamp;
    }

    public static class ApplicationKey {
        public String name;
        public int index;
        public int boundNetKey;
        public String key;
        public String oldKey;
    }

    /**
     * 节点信息目前只包含一个netKey和一个appKey
     */
    public static class Node {

        // custom: not in doc
        public String macAddress;

        /**
         * sequence number
         * custom value
         * big endian string convert by mesh.sno
         * valued only when node is provisioner
         *
         * @see com.telink.ble.mesh.model.MeshInfo#sequenceNumber
         */
        public String sno;

        public String UUID;
        public String unicastAddress;
        public String deviceKey;
        public String security;
        public List<NodeKey> netKeys;
        public boolean configComplete;
        public String name;
        public String cid;
        public String pid;
        public String vid;
        public String crpl;
        public Features features;
        public boolean secureNetworkBeacon;
        public int defaultTTL;
        public Transmit networkTransmit;
        public Transmit relayRetransmit;
        public List<NodeKey> appKeys;
        public List<Element> elements;
        public boolean blacklisted;

        // custom data for scheduler
        public List<NodeScheduler> schedulers;
    }

    public static class NodeScheduler {
        public byte index;

        public long year;
        public long month;
        public long day;
        public long hour;
        public long minute;
        public long second;
        public long week;
        public long action;
        public long transTime;
        public int sceneId;

        public static NodeScheduler fromScheduler(Scheduler scheduler) {
            NodeScheduler nodeScheduler = new NodeScheduler();
            nodeScheduler.index = scheduler.getIndex();

            Scheduler.Register register = scheduler.getRegister();
            nodeScheduler.year = register.getYear();
            nodeScheduler.month = register.getMonth();
            nodeScheduler.day = register.getDay();
            nodeScheduler.hour = register.getHour();
            nodeScheduler.minute = register.getMinute();
            nodeScheduler.second = register.getSecond();
            nodeScheduler.week = register.getWeek();
            nodeScheduler.action = register.getAction();
            nodeScheduler.transTime = register.getTransTime();
            nodeScheduler.sceneId = register.getSceneId();
            return nodeScheduler;
        }
    }

    public static class Features {
        public int relay;
        public int proxy;
        public int friend;
        public int lowPower;

        public Features(int relay, int proxy, int friend, int lowPower) {
            this.relay = relay;
            this.proxy = proxy;
            this.friend = friend;
            this.lowPower = lowPower;
        }
    }

    //Network transmit && Relay retransmit
    public static class Transmit {
        // 0--7
        public int count;
        // 10--120
        public int interval;

        public Transmit(int count, int interval) {
            this.count = count;
            this.interval = interval;
        }
    }

    // node network key && node application key
    public static class NodeKey {
        public int index;
        public boolean updated;

        public NodeKey(int index, boolean updated) {
            this.index = index;
            this.updated = updated;
        }
    }

    public static class Element {
        public String name;
        public int index;
        public String location;
        public List<Model> models;
    }

    public static class Model {
        public String modelId;
        public List<String> subscribe;
        public Publish publish;
        public List<Integer> bind;
    }

    public static class Publish {
        public String address;
        public int index;
        public int ttl;
        public int period;
        public int credentials;
        public Transmit retransmit;
    }

    public static class Group {
        public String name;
        public String address;
        public String parentAddress;
    }

    public static class Scene {
        public String name;
        public List<String> addresses;
        public int number;
    }

}
