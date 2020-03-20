package com.telink.ble.mesh.model;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.foundation.Event;

/**
 * Created by kee on 2019/9/18.
 */

public class NodeStatusChangedEvent extends Event<String> implements Parcelable {
    public static final String EVENT_TYPE_NODE_STATUS_CHANGED = "com.telink.ble.mesh.EVENT_TYPE_NODE_STATUS_CHANGED";
    private NodeInfo nodeInfo;

    public NodeStatusChangedEvent(Object sender, String type, NodeInfo nodeInfo) {
        super(sender, type);
        this.nodeInfo = nodeInfo;
    }

    protected NodeStatusChangedEvent(Parcel in) {
    }

    public static final Creator<NodeStatusChangedEvent> CREATOR = new Creator<NodeStatusChangedEvent>() {
        @Override
        public NodeStatusChangedEvent createFromParcel(Parcel in) {
            return new NodeStatusChangedEvent(in);
        }

        @Override
        public NodeStatusChangedEvent[] newArray(int size) {
            return new NodeStatusChangedEvent[size];
        }
    };

    public NodeInfo getNodeInfo() {
        return nodeInfo;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
    }
}
