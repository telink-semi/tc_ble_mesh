package com.telink.ble.mesh.core.ble;


import java.util.UUID;

/**
 * Created by kee on 2018/7/19.
 */

public class LeScanFilter {

    // null means not in filter

    public UUID[] uuidInclude = null;

    public String[] macInclude = null;

    public String[] macExclude = null;

//    public int[] companyIdInclude = null;

//    private String[] nameInclude = null;
}
