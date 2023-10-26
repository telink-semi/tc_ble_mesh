/********************************************************************************************************
 * @file ShareExportFragment.java
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

package com.telink.ble.mesh.ui;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.foundation.MeshService;
import com.telink.ble.mesh.model.MeshInfo;
import com.telink.ble.mesh.model.MeshNetKey;
import com.telink.ble.mesh.model.db.MeshInfoService;
import com.telink.ble.mesh.model.json.MeshStorageService;
import com.telink.ble.mesh.ui.adapter.MeshKeySelectAdapter;
import com.telink.ble.mesh.ui.cdtp.CdtpExportToGatewayActivity;
import com.telink.ble.mesh.ui.cdtp.CdtpExportToPhoneActivity;
import com.telink.ble.mesh.ui.qrcode.QRCodeShareActivity;
import com.telink.ble.mesh.util.FileSystem;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

/**
 * share export
 */

public class ShareExportActivity extends BaseActivity implements View.OnClickListener {

    private TextView tv_log;
    private RadioGroup rg_share_type;
    private RadioButton rb_file, rb_qrcode, rb_cdtp_to_phone, rb_cdtp_to_gw;
    private File exportDir;
    private Button btn_open;
    private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    private String savedPath;
    private static final String SAVED_NAME = "mesh.json";
    private MeshKeySelectAdapter adapter;
    private MeshInfo meshInfo;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (!validateNormalStart(savedInstanceState)) {
            return;
        }
        setContentView(R.layout.activity_share_export);
        findViewById(R.id.btn_export).setOnClickListener(this);
        btn_open = findViewById(R.id.btn_open);
        btn_open.setOnClickListener(this);
        btn_open.setVisibility(View.GONE);
        tv_log = findViewById(R.id.tv_log);
        exportDir = FileSystem.getSettingPath();
        initView();
        MeshService.getInstance().idle(false);
    }

    @SuppressLint("ClickableViewAccessibility")
    private void initView() {

        enableBackNav(true);
        rg_share_type = findViewById(R.id.rg_share_type);
        rb_file = findViewById(R.id.rb_file);
        rb_qrcode = findViewById(R.id.rb_qrcode);
        rb_cdtp_to_phone = findViewById(R.id.rb_cdtp_to_phone);
        rb_cdtp_to_gw = findViewById(R.id.rb_cdtp_to_gw);

        rb_file.setOnTouchListener(TOUCH_LISTENER);
        rb_qrcode.setOnTouchListener(TOUCH_LISTENER);
        rb_cdtp_to_phone.setOnTouchListener(TOUCH_LISTENER);
        rb_cdtp_to_gw.setOnTouchListener(TOUCH_LISTENER);

        RecyclerView rv_net_key_select = findViewById(R.id.rv_net_key_select);
        rv_net_key_select.setLayoutManager(new LinearLayoutManager(this));
        long meshId = getIntent().getLongExtra("MeshInfoId", 0);
        meshInfo = MeshInfoService.getInstance().getById(meshId);
        setTitle("Share Export", meshInfo.meshName);
        adapter = new MeshKeySelectAdapter<>(this, meshInfo.meshNetKeyList);
        rv_net_key_select.setAdapter(adapter);
    }

    @SuppressLint("ClickableViewAccessibility")
    private final View.OnTouchListener TOUCH_LISTENER = new View.OnTouchListener() {
        @Override
        public boolean onTouch(View v, MotionEvent event) {
            Drawable drawableRight = ((TextView) v).getCompoundDrawables()[2];
            if (event.getAction() == MotionEvent.ACTION_UP && event.getRawX() >= (v.getRight() - drawableRight.getBounds().width())) {
                toastMsg("phone");
                String resId = "";
                if (v == rb_file) {
                    resId = "1. Internal testing use.\\n2. Implementation process:\\n① After exporting JSON data from mobile phone A, generate a JSON file with a *. JSON suffix.\\n\\n② The user then transfers it to mobile phone B through file copying.\\n\\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (similar to Disadvantage 1 of `QRCode + BLE Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt explanation of 'QRCode + BLE Transfer JSON'.\\n\\n4. Disadvantage 2- Provisioner address conflict issue (similar to Disadvantage 2 of `QRCode + Cloud Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt description of 'QRCode + Cloud Transfer JSON'.\\n\\n5. Disadvantage 3- *. JSON file transfer problem with JSON suffix: Users need to transfer JSON files through chat tools such as QQ and WeChat, or hardware such as computer USB drives, which is not user-friendly in the operation process.\\n\\n6. If the customer needs to use this solution as a landing solution, the following restrictions need to be imposed:\\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\\n\\n\\n\\n1.内部测试使用。\\n\\n2.实现流程：\\n①手机A导出JSON数据后生成一份*.json为后缀的JSON文件。\\n②用户再通过文件拷贝的方式传输到手机B。\\n\\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+CDTP Transfer JSON`的提示说明。\\n\\n4.缺点2-Provisioner地址冲突问题（和模式2的缺点2相同，目前APP没有针对性解决该问题）：详情请看`QRCode+Cloud Transfer JSON`的提示说明。\\n\\n5.缺点3-*.json为后缀的JSON文件的传输问题：需要用户通过QQ、微信等聊天工具或者电脑U盘等硬件传输JSON文件，操作流程不友好。\\n\\n6.如果客户需要使用这种方案作为落地解决方案，需要做以下限制：\\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。";
                } else if (v == rb_qrcode) {
                    resId = "1. For internal testing use, the Telink cloud source code is not open. If existing cloud functions need to be modified, customers need to develop them themselves.\\n\\n2. Implementation process:\\n① Mobile phone A exports JSON data and uploads it to the cloud, which returns a UUID string. Mobile phone A then generates a QR code from the UUID string.\\n② Scan the QR code on mobile phone B to obtain the UUID string.\\n③ Mobile B uses UUID to request complete JSON data from the cloud.\\n\\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (similar to Disadvantage 1 of `QRCode + BLE Transfer JSON`, currently the APP does not specifically address this issue): For more details, please refer to the prompt explanation of 'QRCode + BLE Transfer JSON'.\\n\\n4. Disadvantage 2- Provisioner address conflict issue (currently the APP does not specifically address this issue): After mobile phone A shares JSON data with mobile phone B, mobile phone B will add a Provisioner to the JSON data, and mobile phone B needs to send the updated JSON data back to mobile phone A; If mobile phone B does not send the updated JSON data back to mobile phone A, but mobile phone A shares the JSON data with mobile phone C again, there will be an address conflict problem caused by mobile phone B and mobile phone C using the same Provisioner.\\n\\n5. If the customer uses this mode of the Demo app for testing, the following restrictions need to be imposed on the operation behavior:\\n① After mobile phone A shares it with mobile phone B, mobile phone B can only perform control operations, and mobile phone B cannot share it anymore. Mobile phone B also does not support operations that modify JSON data such as adding nodes or groups.\\n\\n6. If the customer wants to solve the two shortcomings described above, they need to implement more comprehensive cloud server functions themselves: such as adding devices, adding groups, and other operations to modify JSON data, and then upload Mesh data to the cloud in real-time, and notify other online mobile devices to update Mesh data in real-time.\\n\\n\\n\\n1.内部测试使用，Telink云端源代码不开放，如果需要修改现有云端功能则需要客户自行开发。\\n\\n2.实现流程：\\n①手机A导出JSON数据并上传云端，云端返回一个UUID字符串，手机A再将UUID字符串生成二维码。\\n②手机B扫描二维码获取到UUID字符串。\\n③手机B使用UUID从云端请求到完整的JSON数据。\\n\\n3.缺点1-不同手机之间JSON数据不同步的问题（和模式1的缺点1相同，目前APP没有针对性解决该问题）：详情请看`QRCode+CDTP Transfer JSON`的提示说明。\\n\\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。\\n\\n5.如果客户使用Demo APP的这种模式进行测试，需要对操作行为做以下限制：\\n①手机A分享给手机B后，手机B只能进行控制操作，手机B不能再分享出去，手机B也不支持加入节点、添加分组等会修改JSON数据的操作。\\n\\n6.客户如果要解决上面描述的两个缺点，则需要客户自己实现更加完善的云端服务器功能：如进行添加设备、添加分组等修改JSON数据的操作后将Mesh数据实时上传云端，并实时通知其它在线的手机端更新Mesh数据。";
                } else if (v == rb_cdtp_to_phone) {
                    resId = "1. Cloud based solutions are not required, recommended for customers to use.\\n\\n2. Implementation process:\\n① Phone A (CDTP Server) generates a virtual Bluetooth peripheral and assigns the value of Object Transfer Service UUID=0x1825 to the Service UUIDs field of the Bluetooth broadcast package.\\n② The CDTP Client scans the ServiceUUIDs field of the broadcast package for devices with Object Transfer ServiceUUID=0x1825, and displays it as a CDTP Server list for users to choose from.\\n③ Phone B (CDTP Client) selects a server from the CDTP Server list and clicks the Start button to start transmitting JSON data through the OTS protocol.\\n④ When JSON data transmission is completed, both phones will prompt for successful sharing.\\n⑤ In order to reduce the amount of data transmitted by CDTP, the APP performs zip compression on JSON data.\\n⑥ For security reasons, the generated virtual Bluetooth peripherals are encrypted using SMP.\\n\\n3. Disadvantage 1- Problem of asynchronous JSON data between different mobile phones (currently the app does not specifically address this issue): Mobile A adds a new device and Mobile B is unaware of it. Mobile A needs to share Mesh data with Mobile B again, but without the cloud, sharing cannot be done in a timely manner. If both phone A and phone B have added or deleted devices at the same time, it is also necessary to consider the issue of JSON data merging.\\nSolution suggestion 1: The customer stores JSON data in the cloud and pulls the JSON data from the cloud every time. After updating the JSON data locally on their mobile phone, they immediately upload it to the cloud.\\nSolution suggestion 2: Customers should add the roles of Mesh administrator and visitor themselves. Visitors only have device control permissions, and administrators have permission to modify Mesh data and device control.\\n\\n4. Disadvantage 2- Provisioner address conflict issue (currently the APP does not specifically address this issue): After Phone A shares JSON data with Phone B, Phone B will add a Provisioner to the JSON data, and Phone B needs to send the updated JSON data back to Phone A; If Phone B does not send the updated JSON data back to Phone A, but Phone A shares the JSON data with Phone C again, there will be an address conflict problem caused by Phone B and Phone C using the same Provisioner.\\n\\n\\n\\n1.不需要云端的解决方案，推荐客户使用。\\n\\n2.实现流程：\\n①手机A（CDTP Server）生成一个虚拟蓝牙外设并将ObjectTransferServiceUUID=0x1825赋值到蓝牙广播包的ServiceUUIDs字段。\\n②手机B（CDTP Client）扫描广播包的ServiceUUIDs字段存在ObjectTransferServiceUUID=0x1825的设备，并显示为一个CDTP Server列表供用户选择。\\n③手机B（CDTP Client）从CDTP Server列表里面选择一个Server，并点击Start按钮即可开始通过OTS协议进行JSON数据的传输。\\n④当JSON数据传输完成，两个手机都会提示分享成功。\\n⑤为了减少CDTP传输的数据量，APP对JSON数据进行zip压缩处理。\\n⑥为了安全性，生成的虚拟蓝牙外设使用了SMP加密。\\n\\n3.缺点1-不同手机之间JSON数据不同步的问题（目前APP没有针对性解决该问题）：手机A新添加设备，手机B是不知道的，需要手机A再次分享Mesh数据给手机B，但是没有云端的情况下，分享不能及时进行。如果手机A和手机B同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。\\n解决建议1：客户自己将JSON数据存储到云端，每次都拉取云端的JSON数据，手机本地更新JSON数据后立刻上传云端。\\n解决建议2：客户自己新增Mesh管理员与访客的角色，访客只有设备控制权限，管理员有修改Mesh数据和设备控制的权限。\\n\\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给手机B后，手机B会添加一个Provisioner到JSON数据里面，手机B需要将更新后的JSON数据回传给手机A；如果手机B未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于手机B和手机C使用相同的Provisioner而造成的地址冲突问题。";
                } else if (v == rb_cdtp_to_gw) {
                    resId = "1. Cloud based solutions are not required, recommended for customers to use.\\n\\n2. Implementation process:\\n① The Gateway (CDTP Server) assigns Object Transfer Service UUID=0x1825 to the Service UUIDs field of the Bluetooth broadcast packet and starts Bluetooth broadcasting.\\n② Phone A (CDTP Client) scans the ServiceUUIDs field of the broadcast package for devices with Object Transfer ServiceUUID=0x1825, and displays it as a CDTP Server list for users to choose from.\\n③ Phone A (CDTP Client) selects a server from the CDTP Server list and clicks the Start button to start transmitting JSON data through the OTS protocol.\\n④ When JSON data transmission is completed, phone A will prompt for successful sharing.\\n⑤ In order to reduce the amount of data transmitted by CDTP, the APP performs zip compression on JSON data.\\n⑥ For security reasons, the gateway Bluetooth peripheral uses SMP encryption.\\n\\n3. Disadvantage 1- Problem of asynchronous JSON data between different phones (currently the app does not specifically address this issue): Gateway has added a new device, and Phone A is unaware of it. Gateway needs to share Mesh data with Phone A again, but without the cloud, sharing cannot be done in a timely manner. If both Gateway and Mobile A add or delete devices at the same time, it is also necessary to consider the issue of JSON data merging.\\n4. Disadvantage 2- Provisioner address conflict issue (currently the app does not specifically address this issue): After Phone A shares JSON data with Gateway, Gateway will add a Provisioner to the JSON data, and Gateway needs to send the updated JSON data back to Phone A; If Gateway does not send the updated JSON data back to Phone A, but Phone A shares the JSON data with Phone C again, there will be an address conflict problem caused by Gateway and Phone C using the same Provisioner.\\n\\n\\n\\n1.不需要云端的解决方案，推荐客户使用。\\n\\n2.实现流程：\\n①Gateway（CDTP Server）将ObjectTransferServiceUUID=0x1825赋值到蓝牙广播包的ServiceUUIDs字段，并开始蓝牙广播。\\n②手机A（CDTP Client）扫描广播包的ServiceUUIDs字段存在ObjectTransferServiceUUID=0x1825的设备，并显示为一个CDTP Server列表供用户选择。\\n③手机A（CDTP Client）从CDTP Server列表里面选择一个Server，并点击Start按钮即可开始通过OTS协议进行JSON数据的传输。\\n④当JSON数据传输完成，手机A会提示分享成功。\\n⑤为了减少CDTP传输的数据量，APP对JSON数据进行zip压缩处理。\\n⑥为了安全性，gateway蓝牙外设使用了SMP加密。\\n\\n3.缺点1-不同手机之间JSON数据不同步的问题（目前APP没有针对性解决该问题）：Gateway新添加设备，手机A是不知道的，需要Gateway再次分享Mesh数据给手机A，但是没有云端的情况下，分享不能及时进行。如果Gateway和手机A同时添加了设备或者删除了设备，还要需要考虑JSON数据合并的问题。\\n4.缺点2-Provisioner地址冲突问题（目前APP没有针对性解决该问题）：手机A分享JSON数据给Gateway后，Gateway会添加一个Provisioner到JSON数据里面，Gateway需要将更新后的JSON数据回传给手机A；如果Gateway未将更新后的JSON数据回传给手机A，手机A却再次分享JSON数据给手机C，就会出现由于Gateway和手机C使用相同的Provisioner而造成的地址冲突问题。";
                }
                startActivity(new Intent(ShareExportActivity.this, TipsActivity.class)
                        .putExtra(TipsActivity.INTENT_KEY_TIP_STRING, resId)
                );
                return true;
            }
            return false;
        }
    };

    /**
     * check if file exists
     */
    private void checkFile() {
        String filename = MeshStorageService.JSON_FILE;
        File file = new File(exportDir, filename);
        if (file.exists()) {
//            btn_open.setVisibility(View.VISIBLE);
            btn_open.setVisibility(View.GONE);
            savedPath = file.getAbsolutePath();
            String desc = "File already exists: " + file.getAbsolutePath() + " -- " + sdf.format(new Date(file.lastModified())) + "\n";
            tv_log.append(desc);
        } else {
            btn_open.setVisibility(View.GONE);
        }
    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.btn_export:

                /*if(meshInfo.nodes.size() == 0){
                    toastMsg("not allow to share empty network");
                    return;
                }*/
                List<MeshNetKey> selectedKeys = getSelectedNetKeys();
                if (selectedKeys.size() == 0) {
                    toastMsg("select at least one net key");
                    return;
                }

                switch (rg_share_type.getCheckedRadioButtonId()) {
                    case R.id.rb_file:
                        exportToFile(selectedKeys);
                        break;
                    case R.id.rb_qrcode:
                        exportToQrcode(selectedKeys);
                        break;
                    case R.id.rb_cdtp_to_phone:
                        exportToPhone(selectedKeys);
                        break;
                    case R.id.rb_cdtp_to_gw:
                        exportToGw(selectedKeys);
                        break;
                }
                break;
            case R.id.btn_open:
                startActivity(
                        new Intent(this, JsonPreviewActivity.class)
                                .putExtra(JsonPreviewActivity.FILE_PATH, savedPath)
                );
                break;
        }
    }

    private void exportToFile(List<MeshNetKey> selectedKeys) {
        MeshInfo meshInfo = TelinkMeshApplication.getInstance().getMeshInfo();
        File file = MeshStorageService.getInstance().exportMeshToJson(
                exportDir,
                MeshStorageService.JSON_FILE,
                meshInfo,
                selectedKeys);

        savedPath = file.getAbsolutePath();
        String desc = "File exported: " + file.getAbsolutePath() + " -- " + sdf.format(new Date(file.lastModified())) + "\n";
//                tv_log.append(desc);
        tv_log.setText(desc);
        toastMsg("Export Success!");
//                btn_open.setVisibility(View.VISIBLE);
        btn_open.setVisibility(View.GONE);
    }

    private void exportToQrcode(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(this, QRCodeShareActivity.class)
                .putExtra("MeshInfoId", meshInfo.id)
                .putExtra("selectedIndexes", getIndexes(selectedKeys))
        );
    }

    private void exportToPhone(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(this, CdtpExportToPhoneActivity.class)
                .putExtra("MeshInfoId", meshInfo.id)
                .putExtra("selectedIndexes", getIndexes(selectedKeys)));
    }


    private void exportToGw(List<MeshNetKey> selectedKeys) {
        startActivity(new Intent(this, CdtpExportToGatewayActivity.class)
                .putExtra("MeshInfoId", meshInfo.id)
                .putExtra("selectedIndexes", getIndexes(selectedKeys)));
    }

    private int[] getIndexes(List<MeshNetKey> selectedKeys) {
        int[] keyIndexes = new int[selectedKeys.size()];
        for (int i = 0; i < keyIndexes.length; i++) {
            keyIndexes[i] = selectedKeys.get(i).index;
        }
        return keyIndexes;
    }

    private List<MeshNetKey> getSelectedNetKeys() {
        List<MeshNetKey> meshNetKeyList = new ArrayList<>();
        final List<MeshNetKey> netKeyList = TelinkMeshApplication.getInstance().getMeshInfo().meshNetKeyList;
        final List<Boolean> selectList = adapter.getSelectList();
        for (int i = 0; i < selectList.size(); i++) {
            if (selectList.get(i)) {
                meshNetKeyList.add(netKeyList.get(i));
            }
        }
        return meshNetKeyList;
    }

}
