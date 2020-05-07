package com.telink.ble.mesh.ui;

import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;
import android.widget.EditText;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.OOBPair;
import com.telink.ble.mesh.ui.widget.HexFormatTextWatcher;
import com.telink.ble.mesh.util.Arrays;

import androidx.appcompat.widget.Toolbar;

/**
 * show device composition data
 */

public class OOBEditActivity extends BaseActivity {

    public static final String EXTRA_OOB = "com.telink.ble.mesh.EXTRA_OOB";

    public static final String EXTRA_POSITION = "com.telink.ble.mesh.EXTRA_POSITION";

    private boolean isAddMode = false;
    // oobPair in edit mode
    private OOBPair editingOOBPair;
    private EditText et_uuid, et_oob;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_oob_edit);
        isAddMode = !getIntent().hasExtra(EXTRA_OOB);
        if (isAddMode) {
            setTitle("Add OOB");
        } else {
            setTitle("Edit OOB");
            editingOOBPair = (OOBPair) getIntent().getSerializableExtra(EXTRA_OOB);
        }
        enableBackNav(true);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.check);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_check) {
                    OOBPair pair = validateInput();
                    if (pair == null) {
                        return false;
                    }
                    Intent intent = new Intent();
                    intent.putExtra(EXTRA_OOB, pair);
                    if (!isAddMode) {
                        int rawPosition = getIntent().getIntExtra(EXTRA_POSITION, 0);
                        intent.putExtra(EXTRA_POSITION, rawPosition);
                    }
                    setResult(RESULT_OK, intent);
                    finish();
                }
                return false;
            }
        });

        et_uuid = findViewById(R.id.et_uuid);
        et_oob = findViewById(R.id.et_oob);
        TextView tv_uuid_preview = findViewById(R.id.tv_uuid_preview);
        TextView tv_oob_preview = findViewById(R.id.tv_oob_preview);
        et_uuid.addTextChangedListener(new HexFormatTextWatcher(tv_uuid_preview));
        et_oob.addTextChangedListener(new HexFormatTextWatcher(tv_oob_preview));

        if (!isAddMode) {
            et_uuid.setText(Arrays.bytesToHexString(editingOOBPair.deviceUUID));
            et_oob.setText(Arrays.bytesToHexString(editingOOBPair.oob));
        }
    }

    private OOBPair validateInput() {
        String uuidInput = et_uuid.getText().toString();
        if (uuidInput.equals("")) {
            toastMsg("uuid cannot be null");
            return null;
        }

        byte[] uuid = Arrays.hexToBytes(uuidInput);
        if (uuid == null || uuid.length != 16) {
            toastMsg("uuid format error");
            return null;
        }

        String oobInput = et_oob.getText().toString();
        if (oobInput.equals("")) {
            toastMsg("oob cannot be null");
            return null;
        }

        byte[] oob = Arrays.hexToBytes(oobInput);
        if (oob == null || oob.length != 16) {
            toastMsg("oob format error");
            return null;
        }

        if (isAddMode) {
            OOBPair oobPair = new OOBPair();
            oobPair.timestamp = System.currentTimeMillis();
            oobPair.oob = oob;
            oobPair.deviceUUID = uuid;
            return oobPair;
        } else {
            editingOOBPair.timestamp = System.currentTimeMillis();
            editingOOBPair.oob = oob;
            editingOOBPair.deviceUUID = uuid;
            return editingOOBPair;
        }
    }

}
