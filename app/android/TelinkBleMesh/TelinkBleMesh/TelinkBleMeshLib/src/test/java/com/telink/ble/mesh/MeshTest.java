package com.telink.ble.mesh;

import com.telink.ble.mesh.core.Encipher;
import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.core.access.MeshFirmwareParser;
import com.telink.ble.mesh.util.Arrays;

import org.junit.Assert;
import org.junit.Test;

import java.util.zip.CRC32;

/**
 * Created by kee on 2019/10/14.
 */

public class MeshTest {

    /**
     * test 9K firmware
     * ||
     * test chunk at
     */
    @Test
    public void testFirmwareParser() {
        MeshFirmwareParser firmwareParser = new MeshFirmwareParser();
//        byte[] testData = new byte[109 * 1024];
//        byte[] testData = new byte[57];

        byte[] testData = new byte[66464];
        for (int i = 0; i < testData.length; i++) {
            testData[i] = (byte) (i + 1);
        }

        int blockSize = 21;
        int chunkSize = 5;


        firmwareParser.reset(testData);
//        firmwareParser.reset(testData, blockSize, chunkSize);

        int blockNumber = 0;
        int blockIndex;
        while (firmwareParser.hasNextBlock()) {
            blockNumber++;
            blockIndex = firmwareParser.nextBlock();
            System.out.println("block : " + blockIndex);
            for (byte[] chunkData = firmwareParser.nextChunk(); chunkData != null; chunkData = firmwareParser.nextChunk()) {
                System.out.println("chunkData: " + Arrays.bytesToHexString(chunkData, ""));
                if (firmwareParser.validateProgress()){
                    if (firmwareParser.getProgress() > 90){
                        firmwareParser.getProgress();
                    }
                    System.out.println("progress: %" + firmwareParser.getProgress());
                }
            }
        }


//        Assert.assertArrayEquals(firmwareParser.chunkAt(2), Arrays.hexToBytes("3536373839"));
//
        Assert.assertEquals(2, 2);
    }

    @Test
    public void testLog2() {
        int logRst = (int) MeshUtils.mathLog2(8 * 1024);
        Assert.assertEquals(logRst, 13);
    }

    @Test
    public void testCrc32() {
        CRC32 crc32 = new CRC32();

        crc32.update(new byte[]{1});
//        Assert.assertEquals(crc32.getValue(), 0xE401A57BL);
        System.out.println(String.format("chunkData: %08x", crc32.getValue()));
    }


    @Test
    public void testK2() {//
        // ivi or nid invalid: ivi -- 0 nid -- 112 encryptSuit : ivi -- 540300920 nid -- 125
        byte[] netkey = Arrays.hexToBytes("F309F5CF8CCF499984856D13E83CAB7A");
        byte[][] k2Output = Encipher.calculateNetKeyK2(netkey);
        byte nid = (byte) (k2Output[0][15] & 0x7F);
        System.out.println("nid: " + nid);

    }


    @Test
    public void testOnlineStatus() {
        byte[] data = Arrays.hexToBytes("62066A121A5E975F76F4D5A5ABDA0A58CA94");
        byte[] key = Arrays.hexToBytes("3E9DAC96E9BFF60FB435CB79F06D99B5");
        byte[] re = Encipher.decryptOnlineStatus(data, key);

        System.out.println("re: " + Arrays.bytesToHexString(re, ":"));
//        Assert.assertEquals(re, expectedRe);
        //

    }


    @Test
    public void testAes() {
//        byte[] data = Arrays.hexToBytes("0102030405060708090a0b0c0d0e0f00");
        byte[] data = new byte[]{
                '0', '1', '2', '3',
                '0', '1', '2', '3',
                '0', '1', '2', '3',
                '0', '1', '2', '3'
        };

//        byte[] key = Arrays.hexToBytes("3E9DAC96E9BFF60FB435CB79F06D99B5");
        byte[] key = {
                'a', 'b', 'c', 'd',
                'a', 'b', 'c', 'd',
                'a', 'b', 'c', 'd',
                'a', 'b', 'c', 'd'
        };


        byte[] re = Encipher.aes(data, key);

        // aes 1BCEA700A0889443AD3515B5EB4F0E92
        // reverse A1882E0A23E6FCBC8AF95CE3F7F3FED4

        byte[] expectedRe = Arrays.hexToBytes("C7A28CA9F54838A1D8757F6F56597AFF");
        System.out.println("re: " + Arrays.bytesToHexString(re, ""));
//        Assert.assertEquals(re, expectedRe);
        //

    }


}
