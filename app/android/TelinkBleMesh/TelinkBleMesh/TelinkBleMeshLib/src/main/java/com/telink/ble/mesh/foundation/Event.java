/********************************************************************************************************
 * @file Event.java
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

package com.telink.ble.mesh.foundation;

import android.os.Parcelable;

/**
 * This is an abstract class representing an event.
 * It implements the Parcelable interface to allow for easy serialization and deserialization.
 * The event class is generic, allowing for different types of events to be created.
 * The sender and type properties represent the sender object and the type of the event, respectively.
 * The threadMode property represents the thread mode in which the event should be handled.
 * The default thread mode is set to Default.
 */
public abstract class Event<T> implements Parcelable {

    protected Object sender;
    protected T type;
    protected ThreadMode threadMode = ThreadMode.Default;
    
    /**
     * Default constructor for the Event class.
     */
    public Event() {
    }

    /**
     * Constructor for the Event class.
     *
     * @param sender The sender object of the event.
     * @param type   The type of the event.
     */
    public Event(Object sender, T type) {
        this(sender, type, ThreadMode.Default);
    }

    /**
     * Constructor for the Event class.
     *
     * @param sender     The sender object of the event.
     * @param type       The type of the event.
     * @param threadMode The thread mode in which the event should be handled.
     */
    public Event(Object sender, T type, ThreadMode threadMode) {
        this.sender = sender;
        this.type = type;
        this.threadMode = threadMode;
    }

    /**
     * Get the sender object of the event.
     *
     * @return The sender object.
     */
    public Object getSender() {
        return sender;
    }

    /**
     * Get the type of the event.
     *
     * @return The type of the event.
     */
    public T getType() {
        return type;
    }

    /**
     * Get the thread mode in which the event should be handled.
     *
     * @return The thread mode.
     */
    public ThreadMode getThreadMode() {
        return this.threadMode;
    }

    /**
     * Set the thread mode in which the event should be handled.
     *
     * @param mode The thread mode.
     * @return The event object with the updated thread mode.
     */
    public Event<T> setThreadMode(ThreadMode mode) {
        this.threadMode = mode;
        return this;
    }

    /**
     * Enum representing the different thread modes in which an event can be handled.
     * Background - The event should be handled in a background thread.
     * Main - The event should be handled in the main thread.
     * Default - The event should be handled in the default thread mode.
     */
    public enum ThreadMode {
        Background, Main, Default,
        ;
    }
}