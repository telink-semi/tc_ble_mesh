package com.telink.ble.mesh.foundation;

import android.app.Application;

import com.telink.ble.mesh.foundation.event.MeshEvent;
import com.telink.ble.mesh.foundation.event.NetworkInfoUpdateEvent;
import com.telink.ble.mesh.foundation.event.OnlineStatusEvent;
import com.telink.ble.mesh.foundation.event.StatusNotificationEvent;

/**
 * Created by kee on 2019/9/4.
 */
public abstract class MeshApplication extends Application implements EventHandler {

    private EventBus<String> mEventBus;

    @Override
    public void onCreate() {
        super.onCreate();
        mEventBus = new EventBus<>();
    }

    /**
     * sequence-number or iv-index info update
     */
    protected abstract void onNetworkInfoUpdate(NetworkInfoUpdateEvent networkInfoUpdateEvent);

    /**
     * device status notification
     */
    protected abstract void onStatusNotificationEvent(StatusNotificationEvent statusNotificationEvent);

    /**
     * online status notification
     */
    protected abstract void onOnlineStatusEvent(OnlineStatusEvent onlineStatusEvent);

    /**
     * mesh event
     *
     * @see MeshEvent#EVENT_TYPE_MESH_EMPTY
     * @see MeshEvent#EVENT_TYPE_DISCONNECTED
     * @see MeshEvent#EVENT_TYPE_MESH_RESET
     */
    protected abstract void onMeshEvent(MeshEvent meshEvent);

    @Override
    public void onEventHandle(Event<String> event) {

        if (event instanceof NetworkInfoUpdateEvent) {
            // update network info: ivIndex , sequence number
            this.onNetworkInfoUpdate((NetworkInfoUpdateEvent) event);
        } else if (event instanceof StatusNotificationEvent) {
            onStatusNotificationEvent((StatusNotificationEvent) event);
        } else if (event instanceof OnlineStatusEvent) {
            onOnlineStatusEvent((OnlineStatusEvent) event);
        } else if (event instanceof MeshEvent) {
            onMeshEvent((MeshEvent) event);
        }
        dispatchEvent(event);
    }

    /********************************************************************************
     * Event API
     *******************************************************************************/

    /**
     * add event listener
     *
     * @param eventType event type
     * @param listener  listener
     */
    public void addEventListener(String eventType, EventListener<String> listener) {
        this.mEventBus.addEventListener(eventType, listener);
    }

    /**
     * remove listener
     */
    public void removeEventListener(EventListener<String> listener) {
        this.mEventBus.removeEventListener(listener);
    }

    /**
     * remove target event from listener
     *
     * @param eventType type
     * @param listener  ls
     */
    public void removeEventListener(String eventType, EventListener<String> listener) {
        this.mEventBus.removeEventListener(eventType, listener);
    }

    /**
     * remove all
     */
    public void removeEventListeners() {
        this.mEventBus.removeEventListeners();
    }

    /**
     * dispatch event from application
     */
    public void dispatchEvent(Event<String> event) {
        this.mEventBus.dispatchEvent(event);
    }

}
