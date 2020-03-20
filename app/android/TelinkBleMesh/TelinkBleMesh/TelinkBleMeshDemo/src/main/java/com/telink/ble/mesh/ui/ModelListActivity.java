
package com.telink.ble.mesh.ui;

/**
 * model setting
 */

@Deprecated
public class ModelListActivity extends BaseActivity {

    /*private ModelListAdapter mAdapter;
    private List<SigMeshModel> modelList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_model_list);
        initTitle();
        enableBackNav(true);
        RecyclerView recyclerView = findViewById(R.id.rv_models);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        modelList = TelinkMeshApplication.getInstance().getMeshInfo().selectedModels;
        mAdapter = new ModelListAdapter(this, modelList);
        recyclerView.setAdapter(mAdapter);
    }

    private void initTitle() {
        Toolbar toolbar = findViewById(R.id.title_bar);
        setTitle("Model List");
        toolbar.inflateMenu(R.menu.setting);
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_setting) {
                    startActivity(new Intent(ModelListActivity.this, ModelSettingActivity.class));
                }
                return false;
            }
        });
    }

    @Override
    public void onResume() {
        super.onResume();
        mAdapter.notifyDataSetChanged();
    }*/
}
