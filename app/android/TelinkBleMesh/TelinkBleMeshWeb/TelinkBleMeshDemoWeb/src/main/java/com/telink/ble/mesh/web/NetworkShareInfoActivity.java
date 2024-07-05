/********************************************************************************************************
 * @file NetworkShareInfoActivity.java
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
import android.os.Handler;
import android.os.Message;

import androidx.annotation.NonNull;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.viewpager.widget.ViewPager;

import com.google.android.material.tabs.TabLayout;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.activity.BaseActivity;
import com.telink.ble.mesh.ui.fragment.ShareInfoListFragment;
import com.telink.ble.mesh.ui.fragment.ShareLinkListFragment;

/**
 * 显示网络分享列表
 * 分栏显示 ： 用户创建的网络分享（已过期的分享放在最后面， 已过期的只能查看列表， 且内容显示未灰色）和 网络分享请求（已批准， 已拒绝）
 */
public class NetworkShareInfoActivity extends BaseActivity {

    ShareLinkListFragment linkListFragment;

    ShareInfoListFragment infoListFragment;

    TabLayout tabLayout;
    @SuppressWarnings("HandlerLeak")
    private Handler handler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_info);
        initView();
    }

    private void initView() {
        setTitle("Share");
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.menu_single);
        enableBackNav(true);
        toolbar.setOnMenuItemClickListener(item -> {
            if (item.getItemId() == R.id.item_single) {
                if (tabLayout.getSelectedTabPosition() == 0) {
                    linkListFragment.getLinkList();
                } else {
                    infoListFragment.getApplicants();
                }
            }
            return false;
        });

        int networkId = getIntent().getIntExtra("networkId", 0);
        String networkName = getIntent().getStringExtra("networkName");
        linkListFragment = new ShareLinkListFragment();
        linkListFragment.setMeshNetworkInfo(networkId, networkName);
        infoListFragment = new ShareInfoListFragment();
        infoListFragment.setMeshNetworkId(networkId);

        tabLayout = findViewById(R.id.tab_share);

        ViewPager viewPager = findViewById(R.id.vp_share);
        viewPager.setAdapter(new FragmentPagerAdapter(getSupportFragmentManager()) {
            @NonNull
            @Override
            public Fragment getItem(int position) {
                return position == 0 ? linkListFragment : infoListFragment;
            }

            @Override
            public int getCount() {
                return 2;
            }

            @Override
            public CharSequence getPageTitle(int position) {
                return position == 0 ? "Links" : "Applicants";
            }

        });
        tabLayout.setupWithViewPager(viewPager);
    }


}
