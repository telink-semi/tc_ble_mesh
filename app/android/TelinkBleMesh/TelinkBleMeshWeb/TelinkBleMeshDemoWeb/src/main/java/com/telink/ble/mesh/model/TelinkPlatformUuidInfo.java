package com.telink.ble.mesh.model;

import com.telink.ble.mesh.core.MeshUtils;
import com.telink.ble.mesh.util.MeshLogger;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class TelinkPlatformUuidInfo {

    public static final int TC_FLAG_TELINK = 0x0211;
    // fixed 0x0211, means Telink

    /**
     * 2 bytes
     * Telink Cloud Flag，fixed 0x0211
     */
    public int tcFlag;

    /**
     * 1 byte
     * bit0~1:	UUID version: initial version is 0 \newline
     * bit2: 	static OOB: 0 “no static OOB”; 1 "static OOB" \newline
     * bit3: 	key_bind_needed: 0: no need to send key bind from APP. 1: need. \newline
     * bit4~7:	reserved for future
     */
    public int protocolFeature;

    /**
     * 2 bytes
     * Telink Cloud Vendor ID, Telink assign. 0x0000 for reserve, 0x0001 ~ 0x000f for telink, 0x0010 ~ 0xfffe will be assigned to coustomer.
     */
    public int tcVendorId;

    /**
     * 2 bytes
     * Telink Cloud Product ID
     */
    public int pid;

    /**
     * 6 bytes
     * MAC address, little endianess
     */
    public byte[] mac;


    /**
     * 2 bytes
     * Reserved for future use
     */
    public int rfu;

    /**
     * summary from "TC Flag" to "CHECKSUM"(not include)
     * 1 byte
     */
    public int checksum;


    /**
     * 02:01:06:03:03:27:18:15:16:27:18:60:73:EA:EC:10:51:49:3C:A6:7C:0E:BF:13:E3:6E:05:00:00:1E:FF:11:02:83:DD:CC:BB:FF:FF:83:5D:00:00:00:00:00:00:00:00:01:02:03:04:05:06:07:08:09:0A:0B:00:00
     * 60:73:EA:EC:10:51:49:3C:A6:7C:0E:BF:13:E3:6E:05
     * 6073EAEC1051493CA67C0EBF13E36E05
     * sample : 1102 0100 0100 51DDCCBBFFFF 000000C8
     * cidProtocol
     * cidPlatform
     * pid
     * mac
     */
    public static TelinkPlatformUuidInfo parseFromUuid(byte[] deviceUuid) {
        if (deviceUuid.length != 16) return null;
        MeshLogger.d("parseFromUuid - " + com.telink.ble.mesh.util.Arrays.bytesToHexString(deviceUuid));
        int checksum = getChecksum(deviceUuid);
        if ((deviceUuid[15] & 0xFF) != checksum) {
            MeshLogger.e(String.format("device uuid checksum error : %02X - %02X", checksum, deviceUuid[15]));
            return null;
        }

        int offset = 0;
        int tcFlag = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        if (tcFlag != TC_FLAG_TELINK) {
            MeshLogger.e("protocol error: " + tcFlag);
            return null;
        }

        offset += 2;
        TelinkPlatformUuidInfo info = new TelinkPlatformUuidInfo();
        info.tcFlag = tcFlag;
        info.protocolFeature = deviceUuid[offset++] & 0xFF;

        info.tcVendorId = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        offset += 2;

        info.pid = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);
        offset += 2;

        info.mac = ByteBuffer.allocate(6).put(deviceUuid, offset, 6).array();
        offset += 6;

        info.rfu = MeshUtils.bytes2Integer(deviceUuid, offset, 2, ByteOrder.LITTLE_ENDIAN);

        return info;
    }

    public int getUuidVersion() {
        return protocolFeature & 0x03;
    }

    public boolean isStaticOobEnable() {
        return (protocolFeature & 0x04) != 0;
    }

    public boolean isKeyBindNeeded() {
        return (protocolFeature & 0x08) != 0;
    }


    private static int getChecksum(byte[] deviceUuid) {
        int sum = 0;
        for (int i = 0; i < deviceUuid.length - 2; i++) {
            sum += deviceUuid[i] & 0xFF;
        }
        return sum & 0xFF;
    }

    @Override
    public String toString() {
        return "TelinkPlatformUuidInfo{" +
                "tcFlag=" + tcFlag +
                ", protocolFeature=" + protocolFeature +
                ", tcVendorId=" + tcVendorId +
                ", pid=" + pid +
                ", mac=" + Arrays.toString(mac) +
                ", rfu=" + rfu +
                ", checksum=" + checksum +
                '}';
    }
}
