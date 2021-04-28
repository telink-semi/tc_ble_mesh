package com.telink.ble.mesh.core.access.fu;

import java.io.Serializable;

public enum UpdatePolicy implements Serializable {
    VerifyOnly(0,
            "The Firmware Distribution Server verifies that firmware image distribution completed successfully but does not apply the update. The Initiator (the Firmware Distribution Client) initiates firmware image application"
    ),
    VerifyAndApply(1,
            "The Firmware Distribution Server verifies that firmware image distribution completed successfully and then applies the firmware update"
    );

    final int value;
    final String desc;

    UpdatePolicy(int value, String desc) {
        this.value = value;
        this.desc = desc;
    }
}
