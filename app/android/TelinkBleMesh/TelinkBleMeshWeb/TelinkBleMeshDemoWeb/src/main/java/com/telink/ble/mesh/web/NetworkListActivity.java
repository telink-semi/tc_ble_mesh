/********************************************************************************************************
 * @file NetworkListActivity.java
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
package com.telink.ble.mesh.web;

import android.os.Bundle;
import android.view.View;

import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;

/**
 * select network or create new network
 * show created networks and joined networks
 */
public class NetworkListActivity extends BaseActivity implements View.OnClickListener {
    public static final String EXTRA_FROM = "com.telink.ble.mesh.web.net_list_from";

    public static final int FROM_LOGIN = 0;

    public static final int FROM_MAIN = 1;

    private CreatedNetworkListFragment createdNetworkListFragment;

    private JoinedNetworkListFragment joinedNetworkListFragment;

    private TabLayout tabLayout;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_network_list);
        initView();
    }

    private void initView() {
        setTitle("Network List");
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        int from = getIntent().getIntExtra(EXTRA_FROM, FROM_LOGIN);
        enableBackNav(from != 0);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_single) {
                if (tabLayout.getSelectedTabPosition() == 0) {
                    createdNetworkListFragment.getNetworkList();
                } else {
                    joinedNetworkListFragment.getJoinedNetworkList();
                }
            }
            return false;
        });

        createdNetworkListFragment = new CreatedNetworkListFragment();
        joinedNetworkListFragment = new JoinedNetworkListFragment();

        tabLayout = findViewById(R.id.tab_network);

        ViewPager viewPager = findViewById(R.id.vp_network);
        viewPager.setAdapter(new FragmentPagerAdapter(getSupportFragmentManager()) {
            @Override
            public Fragment getItem(int position) {
                return position == 0 ? createdNetworkListFragment : joinedNetworkListFragment;
            }

            @Override
            public int getCount() {
                return 2;
            }

            @Override
            public CharSequence getPageTitle(int position) {
                return position == 0 ? "Created" : "Joined";
            }
        });
        tabLayout.setupWithViewPager(viewPager);
    }

    @Override
    public void onClick(View v) {
    }


}
