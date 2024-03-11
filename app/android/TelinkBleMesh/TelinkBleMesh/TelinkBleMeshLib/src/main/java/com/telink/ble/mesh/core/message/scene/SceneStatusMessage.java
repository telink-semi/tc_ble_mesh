/********************************************************************************************************
 * @file SceneStatusMessage.java
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
package com.telink.ble.mesh.core.message.scene;

import android.os.Parcel;
import android.os.Parcelable;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.message.StatusMessage;

import java.nio.ByteOrder;

/**
 * This class represents a message that contains the status of a scene.
 * It extends the StatusMessage class and implements the Parcelable interface for serialization.
 */
public class SceneStatusMessage extends StatusMessage implements Parcelable {

    private static final int DATA_LEN_COMPLETE = 6;
    private byte statusCode;
    private int currentScene;
    private int targetScene;
    private byte remainingTime;
    private boolean isComplete = false;

    /**
     * Default constructor.
     */
    public SceneStatusMessage() {
    }

    /**
     * Constructor for creating a SceneStatusMessage from a Parcel.
     *
     * @param in The Parcel from which to read the SceneStatusMessage.
     */
    protected SceneStatusMessage(Parcel in) {
        statusCode = in.readByte();
        currentScene = in.readInt();
        targetScene = in.readInt();
        remainingTime = in.readByte();
        isComplete = in.readByte() != 0;
    }

    /**
     * Creator constant for generating instances of SceneStatusMessage from a Parcel.
     */
    public static final Creator<SceneStatusMessage> CREATOR = new Creator<SceneStatusMessage>() {
        @Override
        public SceneStatusMessage createFromParcel(Parcel in) {
            return new SceneStatusMessage(in);
        }

        @Override
        public SceneStatusMessage[] newArray(int size) {
            return new SceneStatusMessage[size];
        }
    };

    /**
     * Parses the byte array to extract the parameters and populate the SceneStatusMessage object.
     *
     * @param params The byte array containing the parameters.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.statusCode = params[index++];
        this.currentScene = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        if (params.length >= DATA_LEN_COMPLETE) {
            this.isComplete = true;
            this.targetScene = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
            index += 2;
            remainingTime = params[index];
        }
    }

    /**
     * Returns the description of the contents of this Parcelable object.
     *
     * @return The description of the contents.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the SceneStatusMessage object to a Parcel.
     *
     * @param dest  The Parcel in which to write the SceneStatusMessage.
     * @param flags Additional flags for writing the Parcel.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(statusCode);
        dest.writeInt(currentScene);
        dest.writeInt(targetScene);
        dest.writeByte(remainingTime);
        dest.writeByte((byte) (isComplete ? 1 : 0));
    }

    /**
     * Returns the status code of the scene.
     *
     * @return The status code.
     */
    public byte getStatusCode() {
        return statusCode;
    }

    /**
     * Returns the current scene.
     *
     * @return The current scene.
     */
    public int getCurrentScene() {
        return currentScene;
    }

    /**
     * Returns the target scene.
     *
     * @return The target scene.
     */
    public int getTargetScene() {
        return targetScene;
    }

    /**
     * Returns the remaining time for the scene.
     *
     * @return The remaining time.
     */
    public byte getRemainingTime() {
        return remainingTime;
    }

    /**
     * Returns whether the message is complete or not.
     *
     * @return True if the message is complete, false otherwise.
     */
    public boolean isComplete() {
        return isComplete;
    }
}