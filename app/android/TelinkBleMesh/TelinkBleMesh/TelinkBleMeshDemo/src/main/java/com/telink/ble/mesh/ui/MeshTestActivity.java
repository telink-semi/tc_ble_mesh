package com.telink.ble.mesh.ui;


/**
 * Created by kee on 2017/8/17.
 */

@Deprecated
public class MeshTestActivity extends BaseActivity {


    /*@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_mesh_test);
        findViewById(R.id.tv_filter_init).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MeshService.getInstance().filterInit(TelinkMeshApplication.getInstance().getMeshInfo().localAddress);
            }
        });
        findViewById(R.id.tv_bv03).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv04).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv05).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv06).setOnClickListener(this.bvClick);
        findViewById(R.id.tv_bv07).setOnClickListener(this.bvClick);

    }

    public View.OnClickListener bvClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {

            List<DeviceInfo> devices = TelinkMeshApplication.getInstance().getMeshInfo().devices;
            String curDevice = MeshService.getInstance().getCurDeviceMac();
            if (TextUtils.isEmpty(curDevice)) {
                toastMsg("device not connected");
                return;
            }
            int addr = -1;
            for (DeviceInfo device : devices) {
                if (curDevice.equals(device.macAddress)) {
                    addr = device.meshAddress;
                    break;
                }
            }

            if (addr == -1) {
                toastMsg("device info not found");
                return;
            }

            switch (v.getId()) {

                case R.id.tv_bv03:
                    MeshService.getInstance().sendMeshTestCmd(addr, 3);
                    break;
                case R.id.tv_bv04:
                    MeshService.getInstance().sendMeshTestCmd(addr, 4);
                    break;

                case R.id.tv_bv05:
                    MeshService.getInstance().sendMeshTestCmd(addr, 5);
                    break;

                case R.id.tv_bv06:
                    MeshService.getInstance().sendMeshTestCmd(addr, 6);
                    break;

                case R.id.tv_bv07:
                    MeshService.getInstance().sendMeshTestCmd(addr, 7);
                    break;

            }
        }
    };*/
}
