/********************************************************************************************************
 * @file FileSystem.java
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
package com.telink.ble.mesh.util;

import android.content.Context;
import android.os.Environment;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

public abstract class FileSystem {

    public static boolean writeAsObject(Context context, String fileName, Object obj) {

        File dir = context.getFilesDir();
        File file = new File(dir, fileName);

        FileOutputStream fos = null;
        ObjectOutputStream ops = null;

        boolean success = false;
        try {

            if (!file.exists())
                file.createNewFile();

            fos = new FileOutputStream(file);
            ops = new ObjectOutputStream(fos);

            ops.writeObject(obj);
            ops.flush();

            success = true;

        } catch (IOException e) {

        } finally {
            try {
                if (ops != null)
                    ops.close();
                if (ops != null)
                    fos.close();
            } catch (Exception e) {
            }
        }

        return success;
    }

    public static Object readAsObject(Context context, String fileName) {

        File dir = context.getFilesDir();
        File file = new File(dir, fileName);

        if (!file.exists())
            return null;
        MeshLogger.d("obj file size: " + file.length());
        FileInputStream fis = null;
        ObjectInputStream ois = null;

        Object result = null;
        try {

            fis = new FileInputStream(file);
            ois = new ObjectInputStream(fis);

            result = ois.readObject();
        } catch (IOException | ClassNotFoundException e) {
            MeshLogger.w("read object error : " + e.toString());
        } finally {
            try {
                if (ois != null)
                    ois.close();
            } catch (Exception e) {
            }
        }

        return result;
    }

    /**
     * delete file
     *
     * @param fileName file in app file dir
     */
    public static void deleteFile(Context context, String fileName) {
        File dir = context.getFilesDir();
        File file = new File(dir, fileName);
        if (file.delete()) {
            MeshLogger.d("file delete success");
        } else {
            MeshLogger.d("file delete fail");
        }
    }

    public static File getSettingPath() {
        File root = Environment.getExternalStorageDirectory();
        return new File(root.getAbsolutePath() + File.separator + "TelinkBleMesh");
    }

    //
    public static File writeString(File dir, String filename, String content) {

        if (!dir.exists()) {
            dir.mkdirs();
        }
        File file = new File(dir, filename);

        FileOutputStream fos = null;
        try {
            if (!file.exists())
                file.createNewFile();
            fos = new FileOutputStream(file);
            fos.write(content.getBytes());
            fos.flush();
            fos.getFD().sync();
            return file;
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }

        return null;
    }


    public static String readString(File file) {
        if (!file.exists())
            return "";
        try {
            FileReader fr = new FileReader(file);
            BufferedReader br = new BufferedReader(fr);
            String line = null;

            StringBuilder sb = new StringBuilder();

            while ((line = br.readLine()) != null) {
                sb.append(line);
            }

            br.close();
            fr.close();
            return sb.toString();

        } catch (IOException e) {

        }

        return "";
    }


    public static File writeByteArray(File dir, String filename, byte[] array) {

        if (!dir.exists()) {
            dir.mkdirs();
        }
        File file = new File(dir, filename);

        FileOutputStream fos = null;
        try {
            if (!file.exists())
                file.createNewFile();
            fos = new FileOutputStream(file);
            fos.write(array);
            fos.flush();
            fos.getFD().sync();
            return file;
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

        }

        return null;
    }

    public static byte[] readByteArray(File file) {
        if (!file.exists())
            return null;
        FileInputStream fis = null;
        byte[] certData;
        try {
            fis = new FileInputStream(file);
            certData = new byte[fis.available()];
            fis.read(certData);
            return certData;
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (fis != null) {
                try {
                    fis.close();
                    fis = null;
                } catch (IOException e) {
                    e.printStackTrace();
                }

            }
        }

        return null;
    }
}