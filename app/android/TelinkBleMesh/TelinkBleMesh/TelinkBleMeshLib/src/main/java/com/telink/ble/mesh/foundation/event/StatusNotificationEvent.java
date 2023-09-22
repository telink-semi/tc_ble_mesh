/********************************************************************************************************
 * @file StatusNotificationEvent.java
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
package com.telink.ble.mesh.foundation.event;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.message.NotificationMessage;
import com.telink.ble.mesh.foundation.Event;

/**
 * Mesh Status Message Event
 * event will dispatched when received mesh status
 * For known messages, registering in MeshStatus.Container is suggested
 * If StatusMessage is registered in {@link com.telink.ble.mesh.core.message.MeshStatus.Container}
 * the eventType of the status event will be valued by the className (Class.getName),
 * otherwise, it will be valued by {@link #EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN}
 * <p>
 * Created by kee on 2019/9/12.
 */
public class StatusNotificationEvent extends Event<String> implements Parcelable {

    /**
     * all unrecognized notification
     */
    public static final String EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN = "com.telink.ble.mesh.EVENT_TYPE_NOTIFICATION_MESSAGE_UNKNOWN";

    /**
     * event message
     */
    private NotificationMessage notificationMessage;

    /**
     * Constructor for the StatusNotificationEvent class.
     *
     * @param sender              the object that sends the event
     * @param type                the type of the event
     * @param notificationMessage the notification message associated with the event
     */
    public StatusNotificationEvent(Object sender, String type, NotificationMessage notificationMessage) {
        super(sender, type);
        this.notificationMessage = notificationMessage;
    }

    /**
     * Constructor for the StatusNotificationEvent class used for parceling.
     *
     * @param in the parcel to read from
     */
    protected StatusNotificationEvent(Parcel in) {
        notificationMessage = in.readParcelable(NotificationMessage.class.getClassLoader());
    }

    /**
     * Creator for the StatusNotificationEvent class used for parceling.
     */
    public static final Creator<StatusNotificationEvent> CREATOR = new Creator<StatusNotificationEvent>() {
        @Override
        public StatusNotificationEvent createFromParcel(Parcel in) {
            return new StatusNotificationEvent(in);
        }

        @Override
        public StatusNotificationEvent[] newArray(int size) {
            return new StatusNotificationEvent[size];
        }
    };

    /**
     * Getter for the notification message associated with the event.
     *
     * @return the notification message
     */
    public NotificationMessage getNotificationMessage() {
        return notificationMessage;
    }

    /**
     * Implementing the describeContents() method of the Parcelable interface.
     *
     * @return 0 as there are no special objects in the parcel
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Implementing the writeToParcel() method of the Parcelable interface.
     *
     * @param dest  the parcel to write to
     * @param flags additional flags for writing
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeParcelable(notificationMessage, flags);
    }
}