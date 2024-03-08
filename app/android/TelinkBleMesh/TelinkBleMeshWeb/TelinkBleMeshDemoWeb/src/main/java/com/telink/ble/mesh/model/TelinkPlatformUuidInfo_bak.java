package com.telink.ble.mesh.model;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * 历史版本
 */
public class TelinkPlatformUuidInfo_bak {

    public static final int CID_PROTOCOL_TELINK = 0x0211;
    // fixed 0x0211, means Telink
    public int cidProtocol;

    // Telink assign. 0x0000 for reserve, 0x0001 ~ 0x000f for telink, 0x0010 ~ 0xfffe will be assigned to coustomer.
    public int cidPlatform;

    // defined by customer. used to represent composition data, so it must be different if composition data(exclude version ID) is diffrent.
    public int pid;

    // 6 bytes , little endian
    public byte[] mac;

    // 3 bits UUID version, default is 0
    public int uuidVersion;

    //	1 bit
    public boolean staticOobEnable;

    // 1 bit 0 means no need to send key bind for APP. 1 means need.
    public boolean keyBindNeeded;

    // 3 bits;
    public int featureRsv1;

    //  2 bytes
    public byte[] featureRsv2;

    public byte checksum;

    /**
     * sample : 1102 0100 0100 51DDCCBBFFFF 000000C8
     *          cidProtocol
     *               cidPlatform
     *                    pid
     *                         mac
     */
    public static TelinkPlatformUuidInfo_bak parseFromUuid(byte[] deviceUuid) {
        if (deviceUuid.length != 16) return null;
        int checksum = getChecksum(deviceUuid);
        if ((deviceUuid[15] & 0xFF) != checksum) {
            MeshLogger.e(String.format("device uuid checksum error : %02X - %02X" , checksum, deviceUuid[15]));
            return null;
        }

        int offset = 0;
        int cidProtocol = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        if (cidProtocol != CID_PROTOCOL_TELINK) {
            MeshLogger.e("protocol error: " + cidProtocol);
            return null;
        }

        offset += 2;
        TelinkPlatformUuidInfo_bak info = new TelinkPlatformUuidInfo_bak();
        info.cidProtocol = cidProtocol;
        info.cidPlatform = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        offset += 2;
        info.pid = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        offset += 2;
        info.mac = ByteBuffer.allocate(6).put(deviceUuid, offset, 6).array();
        offset += 6;
        int i = deviceUuid[offset++] & 0xFF;
        info.uuidVersion = i & 0x07; // 3 bits
        info.staticOobEnable = (i & 0x08) != 0; // 1 bit
        info.keyBindNeeded = (i & 0x10) != 0; // 1 bit
        info.featureRsv1 = (i >> 5) & 0x07;
        info.featureRsv2 = ByteBuffer.allocate(2).put(deviceUuid, offset, 2).array();
        return info;
    }

    private static int getChecksum(byte[] deviceUuid) {
        int sum = 0;
        for (int i = 0; i < deviceUuid.length - 2; i++) {
            sum += deviceUuid[i] & 0xFF;
        }
        return sum & 0xFF;
    }

}
