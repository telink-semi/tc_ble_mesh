/********************************************************************************************************
 * @file SceneRegisterStatusMessage.java
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
 * This class represents a scene register status message.
 * It extends the StatusMessage class and implements the Parcelable interface.
 * It contains information about the status code, current scene, and scenes.
 */
public class SceneRegisterStatusMessage extends StatusMessage implements Parcelable {
    private byte statusCode; // The status code of the message
    private int currentScene; // The current scene
    private int[] scenes; // An array of scenes

    /**
     * Default constructor for the SceneRegisterStatusMessage class.
     */
    public SceneRegisterStatusMessage() {
    }

    /**
     * Constructor for the SceneRegisterStatusMessage class that initializes the object from a Parcel.
     *
     * @param in The Parcel object to read the data from.
     */
    protected SceneRegisterStatusMessage(Parcel in) {
        statusCode = in.readByte();
        currentScene = in.readInt();
        scenes = in.createIntArray();
    }

    /**
     * Creator constant for the SceneRegisterStatusMessage class.
     */
    public static final Creator<SceneRegisterStatusMessage> CREATOR = new Creator<SceneRegisterStatusMessage>() {
        @Override
        public SceneRegisterStatusMessage createFromParcel(Parcel in) {
            return new SceneRegisterStatusMessage(in);
        }

        @Override
        public SceneRegisterStatusMessage[] newArray(int size) {
            return new SceneRegisterStatusMessage[size];
        }
    };

    /**
     * Parses the byte array and sets the values of the status code, current scene, and scenes.
     *
     * @param params The byte array to parse.
     */
    @Override
    public void parse(byte[] params) {
        int index = 0;
        this.statusCode = params[index++];
        this.currentScene = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
        index += 2;
        int rst = params.length - index;
        if (rst > 0 && rst % 2 == 0) {
            scenes = new int[rst / 2];
            for (int i = 0; i < scenes.length; i++) {
                scenes[i] = MeshUtils.bytes2Integer(params, index, 2, ByteOrder.LITTLE_ENDIAN);
                index += 2;
            }
        }
    }

    /**
     * Returns a bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     *
     * @return A bitmask indicating the set of special object types marshaled by this Parcelable object instance.
     */
    @Override
    public int describeContents() {
        return 0;
    }

    /**
     * Writes the object's data to the given Parcel.
     *
     * @param dest  The Parcel object to write the data to.
     * @param flags Additional flags about how the object should be written. May be 0 or Parcelable.PARCELABLE_WRITE_RETURN_VALUE.
     */
    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeByte(statusCode);
        dest.writeInt(currentScene);
        dest.writeIntArray(scenes);
    }

    /**
     * Returns the status code of the message.
     *
     * @return The status code of the message.
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
     * Returns the array of scenes.
     *
     * @return The array of scenes.
     */
    public int[] getScenes() {
        return scenes;
    }
}
