/********************************************************************************************************
 * @file     SchedulerListAdapter.java 
 *
 * @brief    for TLSR chips
 *
 * @author	 telink
 * @date     Sep. 30, 2010
 *
 * @par      Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd.
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.ui.SchedulerSettingActivity;
import com.telink.ble.mesh.entity.Scheduler;

import java.io.Serializable;
import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * scheduler list
 * Created by Administrator on 2016/10/25.
 */
public class SchedulerListAdapter extends BaseRecyclerViewAdapter<SchedulerListAdapter.ViewHolder> {

    private final String[] Months = {
            ("Jan"), ("Feb"), ("Mar"),
            ("Apr"), ("May"), ("June"),
            ("July"), ("Aug"), ("Sep"),
            ("Oct"), ("Nov"), ("Dec"),};

    private final String[] Weeks = {
            ("Monday"), ("Tuesday"),
            ("Wednesday"), ("Thursday"),
            ("Friday"), ("Saturday"),
            ("Sunday")};
    List<Scheduler> mSchedulerList;
    Context mContext;
    int address;

    public SchedulerListAdapter(Context context, List<Scheduler> schedulers, int address) {
        mContext = context;
        mSchedulerList = schedulers;
        this.address = address;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_scheduler, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_scheduler_id = itemView.findViewById(R.id.tv_scheduler_id);
        holder.tv_scheduler_info = itemView.findViewById(R.id.tv_scheduler_info);
        holder.iv_edit = itemView.findViewById(R.id.iv_edit);

        return holder;
    }

    @Override
    public int getItemCount() {
        return mSchedulerList == null ? 0 : mSchedulerList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);

        Scheduler scheduler = mSchedulerList.get(position);
        Scheduler.Register register = scheduler.getRegister();
        holder.tv_scheduler_id.setText("scheduler index: 0x" + Long.toHexString(scheduler.getIndex()));
        holder.iv_edit.setTag(position);
        holder.iv_edit.setOnClickListener(editClick);
        StringBuilder info = new StringBuilder();
        info.append("\nYear: ");
        long year = register.getYear();
        info.append(year == Scheduler.YEAR_ANY ? "Any" : year);

        info.append("\nMonth: ");
        long month = register.getMonth();

        boolean head = true;
        for (int i = 0; i < Months.length; i++) {

            if (((month >> i) & 1) == 1) {
                if (!head) {
                    info.append("/");
                }
                head = false;
                info.append(Months[i]);
            }
        }


        info.append("\nDay: ");
        long day = register.getDay();
        info.append(day == Scheduler.DAY_ANY ? "Any" : day);


        info.append("\nHour: ");
        long hour = register.getHour();
        if (hour == Scheduler.HOUR_ANY) {
            info.append("Any");
        } else if (hour == Scheduler.HOUR_RANDOM) {
            info.append("Random");
        } else {
            info.append(hour);
        }


        info.append("\nMinute: ");
        long minute = register.getMinute();
        if (minute == Scheduler.MINUTE_ANY) {
            info.append("Any");
        } else if (minute == Scheduler.MINUTE_RANDOM) {
            info.append("Random");
        } else if (minute == Scheduler.MINUTE_CYCLE_15) {
            info.append("Every 15 minutes");
        } else if (minute == Scheduler.MINUTE_CYCLE_20) {
            info.append("Every 20 minutes");
        } else {
            info.append(minute);
        }

        info.append("\nSecond: ");
        long second = register.getSecond();
        if (second == Scheduler.SECOND_ANY) {
            info.append("Any");
        } else if (second == Scheduler.SECOND_RANDOM) {
            info.append("Random");
        } else if (second == Scheduler.SECOND_CYCLE_15) {
            info.append("Every 15 seconds");
        } else if (second == Scheduler.SECOND_CYCLE_20) {
            info.append("Every 20 seconds");
        } else {
            info.append(second);
        }


        info.append("\nWeek: ");
        long week = register.getWeek();

        boolean wHead = true;
        for (int i = 0; i < Weeks.length; i++) {

            if (((week >> i) & 1) == 1) {
                if (!wHead) {
                    info.append("/");
                }
                wHead = false;
                info.append(Weeks[i]);
            }
        }

        info.append("\nAction: ");
        long action = register.getAction();
        if (action == Scheduler.ACTION_OFF) {
            info.append("Off");
        } else if (action == Scheduler.ACTION_ON) {
            info.append("On");
        } else if (action == Scheduler.ACTION_NO) {
            info.append("NO");
        } else if (action == Scheduler.ACTION_SCENE) {
            info.append("Scene -- ").append(register.getSceneId());
        }

        holder.tv_scheduler_info.setText(info.toString());
    }

    private View.OnClickListener editClick = new View.OnClickListener() {
        @Override
        public void onClick(View v) {
            int position = (int) v.getTag();
            Intent intent = new Intent(mContext, SchedulerSettingActivity.class);
            intent.putExtra("address", address)
                    .putExtra("scheduler", (Serializable) mSchedulerList.get(position));

            mContext.startActivity(intent);
        }
    };

    class ViewHolder extends RecyclerView.ViewHolder {


        private TextView tv_scheduler_id, tv_scheduler_info;
        private ImageView iv_edit;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
