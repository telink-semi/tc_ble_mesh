package com.telink.ble.mesh.ui;

import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.view.MenuItem;

import com.google.android.material.tabs.TabLayout;
import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.fragment.ShareExportFragment;
import com.telink.ble.mesh.ui.fragment.ShareImportFragment;
import com.telink.ble.mesh.ui.qrcode.QRCodeScanActivity;
import com.telink.ble.mesh.ui.qrcode.QRCodeShareActivity;

import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentPagerAdapter;
import androidx.viewpager.widget.ViewPager;

/**
 * share
 */
public class ShareActivity extends BaseActivity {
    private final Fragment[] fragments = new Fragment[]{new ShareExportFragment(), new ShareImportFragment()};
    private final String[] TITLES = new String[]{"Export", "Import"};

    private final String[] ACTIONS = new String[]{"Export -> Generate QR-Code",
            "Import <- Scan QR-Code"};
    private AlertDialog.Builder actionSelectDialog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share);
        Toolbar toolbar = findViewById(R.id.title_bar);
        toolbar.inflateMenu(R.menu.share_tip);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_tip) {
                    startActivity(new Intent(ShareActivity.this, ShareTipActivity.class));
                } else if (item.getItemId() == R.id.item_qrcode) {
                    showActionSelectDialog();
                }
                return false;
            }
        });
        setTitle("Share");
        enableBackNav(true);
        ViewPager viewPager = findViewById(R.id.vp_share);

        viewPager.setAdapter(new SharePagerAdapter(getSupportFragmentManager()));
        TabLayout tabLayout = findViewById(R.id.tab_share);
        tabLayout.setupWithViewPager(viewPager);
    }

    private void showActionSelectDialog() {
        if (actionSelectDialog == null) {
            actionSelectDialog = new AlertDialog.Builder(this);
            actionSelectDialog.setItems(ACTIONS, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    if (which == 0) {
                        startActivity(new Intent(ShareActivity.this, QRCodeShareActivity.class));
                        finish();
                    } else if (which == 1) {
                        startActivity(new Intent(ShareActivity.this, QRCodeScanActivity.class));
                        finish();
                    }
                }
            });
            actionSelectDialog.setTitle("Select action");

        }
        actionSelectDialog.show();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }


    class SharePagerAdapter extends FragmentPagerAdapter {

        SharePagerAdapter(FragmentManager fm) {
            super(fm);
        }

        @Override
        public CharSequence getPageTitle(int position) {
            return TITLES[position];
        }

        @Override
        public Fragment getItem(int position) {
            return fragments[position];
        }

        @Override
        public int getCount() {
            return fragments.length;
        }
    }
}
