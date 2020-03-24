/********************************************************************************************************
 * @file     FlashOperation.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.sig.mesh.util;

import android.content.Context;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

@Deprecated
/**
 * telink.flash file deprecated in version 2.8.3
 * data Store/Recovery operation see
 * @see com.telink.sig.mesh.lib.MeshLib#onMeshStorageUpdate(byte[], int) (int, int)
 * and
 * @see com.telink.sig.mesh.lib.MeshLib#onMeshStorageRetrieve(int, int)
 */
public abstract class FlashOperation {

    public static final String FILE_NAME = "telink.flash";

    public static final int DATA_LEN = 1024 * 1024;


    public static byte[] readFile(Context context) {
        byte[] flashData = null;

        byte[] result = null;
        File dir = context.getFilesDir();
        File targetFile = new File(dir.getAbsolutePath() + File.separator + FILE_NAME);

        try {
            if (!targetFile.exists()) {
                flashData = new byte[DATA_LEN];
                for (int i = 0; i < flashData.length; i++) {
                    flashData[i] = (byte) 0xFF;
                }
                targetFile.createNewFile();
                FileOutputStream fos = new FileOutputStream(targetFile);
//                fos.
                // 0x1000 len  0x6b000 start
                fos.write(flashData);
                fos.flush();
                fos.close();
            } else {
                InputStream stream = new FileInputStream(targetFile);
                int length = stream.available();
                flashData = new byte[length];
//                stream.skip(0x6b000);


                stream.read(flashData);
                stream.close();
            }
        } catch (IOException e) {
        }

        result = new byte[0x1000];
        System.arraycopy(flashData, 0x60000, result, 0, 0x1000);
//        TelinkLog.d("Flash:" + Arrays.bytesToHexString(result, ":"));
        return flashData;

    }

    public synchronized static void readFlash(Context context, ByteBuffer buffer, int addr, int len) {
//        TelinkLog.d("FlashOperation#readFlash -- buffer capacity:" + buffer.capacity() + " -- addr:" + addr + " -- len:" + len);
        byte[] flashData = null;

        File dir = context.getFilesDir();
        File targetFile = new File(dir.getAbsolutePath() + File.separator + FILE_NAME);

        try {
            if (!targetFile.exists()) {
                flashData = new byte[DATA_LEN];
                for (int i = 0; i < flashData.length; i++) {
                    flashData[i] = (byte) 0xFF;
                }
                targetFile.createNewFile();
                FileOutputStream fos = new FileOutputStream(targetFile);
                fos.write(flashData);
                fos.flush();
                fos.close();
            } else {
                InputStream stream = new FileInputStream(targetFile);
                int length = stream.available();
                flashData = new byte[length];
                stream.read(flashData);
                stream.close();
            }
        } catch (IOException e) {
        }
//        TelinkLog.e("readFlash -- flashData capacity:" + flashData.length + " -- addr:" + addr + " -- len:" + len);
        byte[] result = new byte[len];
        System.arraycopy(flashData, addr, result, 0, len);
        buffer.position(0);
//        buffer.
        buffer.put(result);
//        TelinkLog.e("########## buffer.capacity: " + buffer.capacity());
        byte data0 = buffer.get(0);
//        TelinkLog.e("########## buffer.get(0): " + buffer.get(0));

//        return flashData;
    }

    public synchronized static boolean writeFlash(Context context, byte[] buffer, int addr, int len) {
//        TelinkLog.e("writeFlash -- buffer capacity:" + buffer.length + " -- addr:" + addr + " -- len:" + len);
        File dir = context.getFilesDir();
        File targetFile = new File(dir.getAbsolutePath() + File.separator + FILE_NAME);
        byte[] flashData = null;
        FileOutputStream fos;
        try {

            if (!targetFile.exists()) {

                targetFile.createNewFile();
                flashData = new byte[DATA_LEN];
                for (int i = 0; i < flashData.length; i++) {
                    flashData[i] = (byte) 0xFF;
                }

            } else {
                InputStream stream = new FileInputStream(targetFile);
                int length = stream.available();
                flashData = new byte[length];
                stream.read(flashData);
                stream.close();
            }

            System.arraycopy(buffer, 0, flashData, addr, len);

            fos = new FileOutputStream(targetFile);
            fos.write(flashData);
            fos.flush();
            fos.close();
            return true;
        } catch (IOException e) {
        }

//        TelinkLog.e("########## buffer.length: " + buffer.length);
        return false;

    }

}
