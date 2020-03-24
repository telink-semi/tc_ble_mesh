/********************************************************************************************************
 * @file SchedulerSettingActivity.java
 *
 * @brief for TLSR chips
 *
 * @author telink
 * @date Sep. 30, 2010
 *
 * @par Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
 *           All rights reserved.
 *
 *			 The information contained herein is confidential and proprietary property of Telink 
 * 		     Semiconductor (Shanghai) Co., Ltd. and is available under the terms 
 *			 of Commercial License Agreement between Telink Semiconductor (Shanghai) 
 *			 Co., Ltd. and the licensee in separate contract or the terms described here-in. 
 *           This heading MUST NOT be removed from this file.
 *
 * 			 Licensees are granted free, non-transferable use of the information in this 
 *			 file under Mutual Non-Disclosure Agreement. NO WARRENTY of ANY KIND is provided. 
 *
 *******************************************************************************************************/
package com.telink.sig.mesh.demo.ui;

import android.os.Bundle;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import com.telink.sig.mesh.demo.R;
import com.telink.sig.mesh.demo.TelinkMeshApplication;
import com.telink.sig.mesh.demo.ui.adapter.BaseSelectableListAdapter;
import com.telink.sig.mesh.demo.ui.adapter.SelectableListAdapter;
import com.telink.sig.mesh.event.Event;
import com.telink.sig.mesh.event.EventListener;
import com.telink.sig.mesh.event.NotificationEvent;
import com.telink.sig.mesh.lib.MeshLib;
import com.telink.sig.mesh.light.MeshService;
import com.telink.sig.mesh.light.Opcode;
import com.telink.sig.mesh.light.SchedulerNotificationParser;
import com.telink.sig.mesh.model.DeviceInfo;
import com.telink.sig.mesh.model.Scheduler;
import com.telink.sig.mesh.model.SigMeshModel;
import com.telink.sig.mesh.util.Arrays;
import com.telink.sig.mesh.util.MeshUtils;
import com.telink.sig.mesh.util.TelinkLog;
import com.telink.sig.mesh.util.UnitConvert;

/**
 * 配置闹钟
 * Created by kee on 2018/9/18.
 */

public class SchedulerSettingActivity extends BaseActivity implements View.OnClickListener, BaseSelectableListAdapter.SelectStatusChangedListener, CompoundButton.OnCheckedChangeListener, EventListener<String> {

    private RadioGroup rg_year, rg_day, rg_hour, rg_minute, rg_second, rg_action;
    private EditText et_year, et_day, et_hour, et_minute, et_second, et_scene;
    private RecyclerView rv_months, rv_weeks;
    private SelectableListAdapter monthsAdapter, weeksAdapter;
    private CheckBox cb_month_all, cb_week_all;
    private DeviceInfo mDevice;
    private Scheduler scheduler;
    private byte index;

    // January/February/March/April/May/June/July/August/September/October/November/December
    private SelectableListAdapter.SelectableBean[] monthList = {
            new SelectableListAdapter.SelectableBean("Jan"),
            new SelectableListAdapter.SelectableBean("Feb"),
            new SelectableListAdapter.SelectableBean("Mar"),
            new SelectableListAdapter.SelectableBean("Apr"),
            new SelectableListAdapter.SelectableBean("May"),
            new SelectableListAdapter.SelectableBean("June"),
            new SelectableListAdapter.SelectableBean("July"),
            new SelectableListAdapter.SelectableBean("Aug"),
            new SelectableListAdapter.SelectableBean("Sep"),
            new SelectableListAdapter.SelectableBean("Oct"),
            new SelectableListAdapter.SelectableBean("Nov"),
            new SelectableListAdapter.SelectableBean("Dec"),
    };

    private SelectableListAdapter.SelectableBean[] weekList = {
            new SelectableListAdapter.SelectableBean("Monday"),
            new SelectableListAdapter.SelectableBean("Tuesday"),
            new SelectableListAdapter.SelectableBean("Wednesday"),
            new SelectableListAdapter.SelectableBean("Thursday"),
            new SelectableListAdapter.SelectableBean("Friday"),
            new SelectableListAdapter.SelectableBean("Saturday"),
            new SelectableListAdapter.SelectableBean("Sunday")
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        int address = getIntent().getIntExtra("address", -1);
        mDevice = TelinkMeshApplication.getInstance().getMesh().getDeviceByMeshAddress(address);
        scheduler = (Scheduler) getIntent().getSerializableExtra("scheduler");
        if (scheduler == null) {
            index = mDevice.allocSchedulerIndex();
            if (index == -1) {
                Toast.makeText(getApplicationContext(), "no available index", Toast.LENGTH_SHORT).show();
                finish();
                return;
            }
        } else {
            index = scheduler.getIndex();
        }
        setContentView(R.layout.activity_scheduler_setting);

        Toolbar toolbar = findViewById(R.id.title_bar);
        setTitle("Scheduler Setting");

        toolbar.inflateMenu(R.menu.scheduler);
        toolbar.setOnMenuItemClickListener(new Toolbar.OnMenuItemClickListener() {
            @Override
            public boolean onMenuItemClick(MenuItem item) {
                if (item.getItemId() == R.id.item_check) {
                    saveScheduler();
                } else if (item.getItemId() == R.id.item_time) {
                    setTime();
                }
                return false;
            }
        });
        enableBackNav(true);
        initView();
        fillSchedulerInfo();
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_SCHEDULER_STATUS, this);
        TelinkMeshApplication.getInstance().addEventListener(NotificationEvent.EVENT_TYPE_TIME_STATUS, this);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        TelinkMeshApplication.getInstance().removeEventListener(this);
    }

    @Override
    public void onClick(View v) {
        if (v == cb_month_all) {
            monthsAdapter.setAll(!monthsAdapter.allSelected());
        } else if (v == cb_week_all) {
            weeksAdapter.setAll(!weeksAdapter.allSelected());
        } /* else if (v.getId() == R.id.btn_set_time) {
            MeshService.getInstance().cmdSetTime(mDevice.meshAddress, 1, System.currentTimeMillis() / 1000 - MeshLib.Constant.TAI_OFFSET_SECOND, 8);
        }*/
    }


    private void setTime() {

        long time = MeshUtils.getTaiTime();
        int offset = UnitConvert.getZoneOffset();
        final int eleAdr = mDevice.getTargetEleAdr(SigMeshModel.SIG_MD_TIME_S.modelId);
        if (eleAdr == -1) {
            toastMsg("Time Model Not Found !");
            return;
        }
        boolean re = MeshService.getInstance().setTime(eleAdr, 1, time, offset, null);
        if (re) {
            TelinkLog.d("setTime time: " + time + " zone " + offset);

        } else {
            TelinkLog.d("setTime fail");
        }
    }


    private void saveScheduler() {

        int year;
        switch (rg_year.getCheckedRadioButtonId()) {
            case R.id.rb_year_any:
                year = 0x64;
                break;

            case R.id.rb_year_custom:
                if (TextUtils.isEmpty(et_year.getText())) {
                    toastMsg("custom year empty");
                    return;
                }
                year = Integer.parseInt(et_year.getText().toString());
                year -= 2000;
                if (year < 0 || year > 0x63) {
                    toastMsg("custom day should be 2000-2099");
                    return;
                }
                break;
            default:
                toastMsg("year input err");
                return;
        }

        int month = monthsAdapter.getBinaryResult();

        if (month == 0) {
            toastMsg("choose at least one month");
            return;
        }

        int day;
        switch (rg_day.getCheckedRadioButtonId()) {
            case R.id.rb_day_any:
                day = 0x00;
                break;
            case R.id.rb_day_custom:
                if (TextUtils.isEmpty(et_day.getText())) {
                    toastMsg("custom day empty");
                    return;
                }
                day = Integer.parseInt(et_day.getText().toString());

                if (day < 0x01 || day > 0x1F) {
                    toastMsg("custom day should be 1-31");
                    return;
                }
                break;
            default:
                toastMsg("day input err");
                return;
        }

        int hour;
        switch (rg_hour.getCheckedRadioButtonId()) {
            case R.id.rb_hour_any:
                hour = 0x18;
                break;

            case R.id.rb_hour_random:
                hour = 0x19;
                break;

            case R.id.rb_hour_custom:
                if (TextUtils.isEmpty(et_hour.getText())) {
                    toastMsg("custom hour empty");
                    return;
                }
                hour = Integer.parseInt(et_hour.getText().toString());
                if (hour < 0x00 || hour > 0x17) {
                    toastMsg("custom hour should be 0-23");
                    return;
                }
                break;
            default:
                toastMsg("hour input err");
                return;
        }

        int minute;
        switch (rg_minute.getCheckedRadioButtonId()) {
            case R.id.rb_minute_any:
                minute = 0x3C;
                break;
            case R.id.rb_minute_cycle_15:
                minute = 0x3D;
                break;
            case R.id.rb_minute_cycle_20:
                minute = 0x3E;
                break;
            case R.id.rb_minute_random:
                minute = 0x3F;
                break;
            case R.id.rb_minute_custom:
                if (TextUtils.isEmpty(et_minute.getText())) {
                    toastMsg("custom minute empty");
                    return;
                }
                minute = Integer.parseInt(et_minute.getText().toString());
                if (minute < 0x00 || minute > 0x3B) {
                    toastMsg("custom minute should be 0-59");
                    return;
                }
                break;
            default:
                toastMsg("minute input err");
                return;
        }

        int second;
        switch (rg_second.getCheckedRadioButtonId()) {
            case R.id.rb_second_any:
                second = 0x3C;
                break;
            case R.id.rb_second_cycle_15:
                second = 0x3D;
                break;
            case R.id.rb_second_cycle_20:
                second = 0x3E;
                break;
            case R.id.rb_second_random:
                second = 0x3F;
                break;
            case R.id.rb_second_custom:
                if (TextUtils.isEmpty(et_second.getText())) {
                    toastMsg("custom second empty");
                    return;
                }
                second = Integer.parseInt(et_second.getText().toString());
                if (second < 0x00 || second > 0x3B) {
                    toastMsg("custom second should be 0-59");
                    return;
                }
                break;
            default:
                toastMsg("second input err");
                return;
        }

        int week = weeksAdapter.getBinaryResult();

        /*if (week == 0) {
            toastMsg("choose at least one week");
            return;
        }*/

        int action;
        int sceneId = 0;
        switch (rg_action.getCheckedRadioButtonId()) {
            case R.id.rb_action_off:
                action = 0x00;
                break;
            case R.id.rb_action_on:
                action = 0x01;
                break;
            case R.id.rb_action_no:
                action = 0x0F;
                break;
            case R.id.rb_action_scene:
                action = 0x02;
                sceneId = Integer.parseInt(et_scene.getText().toString());

                break;

            default:
                toastMsg("action input err");
                return;
        }

        TelinkLog.d("scheduler params year: " + year + " month: " + month + " day: " + day + " hour: " + hour +
                " minute: " + minute + " second: " + second + " week: " + week + " action: " + action);

        int transitionTime = 0;

        scheduler = new Scheduler.Builder()
                .setIndex(index)
                .setYear((byte) year)
                .setMonth((short) month)
                .setDay((byte) day)
                .setHour((byte) hour)
                .setMinute((byte) minute)
                .setSecond((byte) second)
                .setWeek((byte) week)
                .setAction((byte) action)
                .setTransTime((byte) transitionTime)
                .setSceneId((short) sceneId).build();
        long register = scheduler.getRegisterParam0();
        TelinkLog.d("scheduler register: " + Long.toBinaryString(register));
        int scene = scheduler.getRegisterParam1();
        int eleAdr = mDevice.getTargetEleAdr(SigMeshModel.SIG_MD_SCHED_S.modelId);
        if (eleAdr == -1) {
            toastMsg("scheduler model not found");
            return;
        }
//        MeshService.getInstance().cmdSetScheduler(eleAdr, 0, register, scene, 1);
        MeshService.getInstance().setSchedulerAction(eleAdr, true, 0, scheduler, null);
        toastMsg("scheduler setting sent");


    }


    private void initView() {


        TextView tv_scheduler_index = findViewById(R.id.tv_scheduler_index);
        tv_scheduler_index.setText(String.format("Index: 0x%s", Integer.toHexString(index)));

        rg_year = findViewById(R.id.rg_year);
        rg_day = findViewById(R.id.rg_day);
        rg_hour = findViewById(R.id.rg_hour);
        rg_minute = findViewById(R.id.rg_minute);
        rg_second = findViewById(R.id.rg_second);
        rg_action = findViewById(R.id.rg_action);


        et_year = findViewById(R.id.et_year);
        et_day = findViewById(R.id.et_day);
        et_hour = findViewById(R.id.et_hour);
        et_minute = findViewById(R.id.et_minute);
        et_second = findViewById(R.id.et_second);
        et_scene = findViewById(R.id.et_scene);

        rv_months = findViewById(R.id.rv_months);
        rv_weeks = findViewById(R.id.rv_weeks);
        cb_month_all = findViewById(R.id.cb_month_all);
        cb_week_all = findViewById(R.id.cb_week_all);

        rv_months.setLayoutManager(new GridLayoutManager(this, 3));
        rv_weeks.setLayoutManager(new GridLayoutManager(this, 3));
        monthsAdapter = new SelectableListAdapter(this, monthList);
        monthsAdapter.setStatusChangedListener(this);
        weeksAdapter = new SelectableListAdapter(this, weekList);
        weeksAdapter.setStatusChangedListener(this);

        rv_months.setAdapter(monthsAdapter);
        rv_weeks.setAdapter(weeksAdapter);

        ((RadioButton) findViewById(R.id.rb_year_custom)).setOnCheckedChangeListener(this);
        ((RadioButton) findViewById(R.id.rb_day_custom)).setOnCheckedChangeListener(this);
        ((RadioButton) findViewById(R.id.rb_hour_custom)).setOnCheckedChangeListener(this);
        ((RadioButton) findViewById(R.id.rb_minute_custom)).setOnCheckedChangeListener(this);
        ((RadioButton) findViewById(R.id.rb_second_custom)).setOnCheckedChangeListener(this);
        ((RadioButton) findViewById(R.id.rb_action_scene)).setOnCheckedChangeListener(this);
        cb_month_all.setOnClickListener(this);
        cb_week_all.setOnClickListener(this);
//        findViewById(R.id.btn_save).setOnClickListener(this);

//        findViewById(R.id.btn_set_time).setOnClickListener(this);
    }

    private void fillSchedulerInfo() {
        if (scheduler == null) {
            cb_month_all.performClick();
            cb_week_all.performClick();
            return;
        }
        byte index = scheduler.getIndex();
        Scheduler.Register register = scheduler.getRegister();

        long year = register.getYear();
        if (year == Scheduler.YEAR_ANY) {
            rg_year.check(R.id.rb_year_any);
        } else {
            rg_year.check(R.id.rb_year_custom);
            et_year.setText(String.valueOf(2000 + year));
        }

        long month = register.getMonth();
        for (int i = 0; i < monthList.length; i++) {
            monthList[i].selected = ((month >> i) & 1) == 1;
        }
        monthsAdapter.notifyDataSetChanged();
        cb_month_all.setChecked(monthsAdapter.allSelected());

        long day = register.getDay();
        if (day == Scheduler.DAY_ANY) {
            rg_day.check(R.id.rb_day_any);
        } else {
            rg_day.check(R.id.rb_day_custom);
            et_day.setText(String.valueOf(day));
        }

        long hour = register.getHour();
        if (hour == Scheduler.HOUR_ANY) {
            rg_hour.check(R.id.rb_hour_any);
        } else if (hour == Scheduler.HOUR_RANDOM) {
            rg_hour.check(R.id.rb_hour_random);
        } else {
            rg_hour.check(R.id.rb_hour_custom);
            et_hour.setText(String.valueOf(hour));
        }

        long minute = register.getMinute();
        if (minute == Scheduler.MINUTE_ANY) {
            rg_minute.check(R.id.rb_minute_any);
        } else if (minute == Scheduler.MINUTE_RANDOM) {
            rg_minute.check(R.id.rb_minute_random);
        } else if (minute == Scheduler.MINUTE_CYCLE_15) {
            rg_minute.check(R.id.rb_minute_cycle_15);
        } else if (minute == Scheduler.MINUTE_CYCLE_20) {
            rg_minute.check(R.id.rb_minute_cycle_20);
        } else {
            rg_minute.check(R.id.rb_minute_custom);
            et_minute.setText(String.valueOf(minute));
        }

        long second = register.getSecond();
        if (second == Scheduler.SECOND_ANY) {
            rg_second.check(R.id.rb_second_any);
        } else if (second == Scheduler.SECOND_RANDOM) {
            rg_second.check(R.id.rb_second_random);
        } else if (second == Scheduler.SECOND_CYCLE_15) {
            rg_second.check(R.id.rb_second_cycle_15);
        } else if (second == Scheduler.SECOND_CYCLE_20) {
            rg_second.check(R.id.rb_second_cycle_20);
        } else {
            rg_second.check(R.id.rb_second_custom);
            et_second.setText(String.valueOf(second));
        }

        long week = register.getWeek();
        for (int i = 0; i < weekList.length; i++) {
            weekList[i].selected = ((week >> i) & 1) == 1;
        }
        weeksAdapter.notifyDataSetChanged();
        cb_week_all.setChecked(weeksAdapter.allSelected());

        long action = register.getAction();
        if (action == Scheduler.ACTION_OFF) {
            rg_action.check(R.id.rb_action_off);
        } else if (action == Scheduler.ACTION_ON) {
            rg_action.check(R.id.rb_action_on);
        } else if (action == Scheduler.ACTION_NO) {
            rg_action.check(R.id.rb_action_no);
        } else if (action == Scheduler.ACTION_SCENE) {
            rg_action.check(R.id.rb_action_scene);
            et_scene.setText(String.valueOf(register.getSceneId()));
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        switch (buttonView.getId()) {
            case R.id.rb_year_custom:
                et_year.setEnabled(isChecked);
                break;
            case R.id.rb_day_custom:
                et_day.setEnabled(isChecked);
                break;
            case R.id.rb_hour_custom:
                et_hour.setEnabled(isChecked);
                break;
            case R.id.rb_minute_custom:
                et_minute.setEnabled(isChecked);
                break;
            case R.id.rb_second_custom:
                et_second.setEnabled(isChecked);
                break;
            case R.id.rb_action_scene:
                et_scene.setEnabled(isChecked);
                break;
        }
    }


    @Override
    public void performed(Event<String> event) {
        super.performed(event);
        switch (event.getType()) {
            case NotificationEvent.EVENT_TYPE_SCHEDULER_STATUS:
                byte[] data = ((NotificationEvent) event).getRawData();
                //        F0:13:02:00:01:00:5F: 20:F9:FF:8C:FB:FE:1F:00:00:00: 00:00:00:00
                TelinkLog.d("scheduler: " + Arrays.bytesToHexString(data, ":"));
                SchedulerNotificationParser.SchedulerInfo schedulerInfo = SchedulerNotificationParser.create().parse(data);
                if (schedulerInfo == null || scheduler == null) return;
                if (schedulerInfo.register == scheduler.getRegisterParam0() && schedulerInfo.sceneId == scheduler.getRegisterParam1()) {
                    toastMsg("scheduler saved");
                    mDevice.saveScheduler(scheduler);
                    TelinkMeshApplication.getInstance().getMesh().saveOrUpdate(this);
                    finish();
                } else {
                    toastMsg("scheduler err");
                }
                /*TelinkLog.d("parsed scheduler info: " + schedulerInfo.register + " -- " + schedulerInfo.sceneId);
                TelinkLog.d("local scheduler info: " + scheduler.getRegisterParam0() + " -- " + scheduler.getRegisterParam1());*/
                break;

            case NotificationEvent.EVENT_TYPE_TIME_STATUS:
                toastMsg("set time success");
                break;
        }
    }

    @Override
    public void onStatusChanged(BaseSelectableListAdapter adapter) {
        if (adapter == monthsAdapter) {
            cb_month_all.setChecked(monthsAdapter.allSelected());
        } else if (adapter == weeksAdapter) {
            cb_week_all.setChecked(weeksAdapter.allSelected());
        }
    }
}
