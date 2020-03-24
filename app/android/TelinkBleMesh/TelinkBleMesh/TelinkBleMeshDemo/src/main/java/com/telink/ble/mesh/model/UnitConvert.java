package com.telink.ble.mesh.model;



import com.telink.ble.mesh.util.MeshLogger;

import java.util.Calendar;

/**
 * Created by kee on 2017/11/28.
 */

public final class UnitConvert {
    public static int lum2level(int lum) {
        if (lum > 100) {
            lum = 100;
        }
        return (-32768 + (65535 * lum) / 100);
    }

    public static int level2lum(short level) {

        int re = (((level + 32768) * 100) / 65535);
        MeshLogger.log("level2lum: " + level + " re: " + re);
        return re;
    }

    /**
     * @param lum 0-100
     * @return 0-65535
     */
    public static int lum2lightness(int lum) {
        return 65535 * lum / 100;
    }

    public static int lightness2lum(int lightness) {
        return lightness * 100 / 65535;
    }

    /**
     * TEMP_MIN	 800
     * TEMP_MAX	 20000
     *
     * @param temp100 0-100
     * @return 800-2000
     */
    public static int temp100ToTemp(int temp100) {
        if (temp100 > 100) {
            temp100 = 100;
        }
        return (800 + ((20000 - 800) * temp100) / 100);
    }


    public static int getZoneOffset() {
        Calendar cal = Calendar.getInstance();
        // zone offset and daylight offset
        return (cal.get(Calendar.ZONE_OFFSET) + cal.get(Calendar.DST_OFFSET)) / 60 / 1000 / 15 + 64;
    }
}
