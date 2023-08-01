/********************************************************************************************************
 * @file SchedulerListAdapter.java
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
package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.recyclerview.widget.RecyclerView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.model.db.Scheduler;
import com.telink.ble.mesh.model.db.SchedulerRegister;
import com.telink.ble.mesh.ui.SchedulerSettingActivity;

import java.io.Serializable;
import java.util.List;

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
        SchedulerRegister register = scheduler.register.getTarget();
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
