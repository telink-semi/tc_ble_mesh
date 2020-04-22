package com.telink.ble.mesh.ui.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.Switch;
import android.widget.TextView;

import com.telink.ble.mesh.demo.R;
import com.telink.ble.mesh.TelinkMeshApplication;
import com.telink.ble.mesh.model.AppSettings;
import com.telink.ble.mesh.core.message.generic.OnOffSetMessage;
import com.telink.ble.mesh.foundation.MeshService;

import java.util.List;

import androidx.recyclerview.widget.RecyclerView;

/**
 * on/off switch
 * Created by Administrator on 2016/10/25.
 */
public class SwitchListAdapter extends BaseRecyclerViewAdapter<SwitchListAdapter.ViewHolder> {


    List<Integer> mAdrList;
    Context mContext;
    int address;

    public SwitchListAdapter(Context context, List<Integer> adrList) {
        mContext = context;
        mAdrList = adrList;
    }

    @Override
    public ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View itemView = LayoutInflater.from(mContext).inflate(R.layout.item_switch, parent, false);
        ViewHolder holder = new ViewHolder(itemView);
        holder.tv_ele = itemView.findViewById(R.id.tv_ele);
        holder.switch_ele = itemView.findViewById(R.id.switch_ele);
        return holder;
    }

    @Override
    public int getItemCount() {
        return mAdrList == null ? 0 : mAdrList.size();
    }

    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        super.onBindViewHolder(holder, position);
        int adr = mAdrList.get(position);
        holder.tv_ele.setText("ele adr: " + adr);
        holder.switch_ele.setTag(adr);
        holder.switch_ele.setOnCheckedChangeListener(switchChangeListner);
    }

    private CompoundButton.OnCheckedChangeListener switchChangeListner = new CompoundButton.OnCheckedChangeListener() {
        @Override
        public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
            int adr = (int) buttonView.getTag();
            boolean ack = !AppSettings.ONLINE_STATUS_ENABLE;
            int rspMax = ack ? 1 : 0;
            int appKeyIndex = TelinkMeshApplication.getInstance().getMeshInfo().getDefaultAppKeyIndex();
            OnOffSetMessage message = OnOffSetMessage.getSimple(adr, appKeyIndex, (byte) (isChecked ? 1 : 0), ack, rspMax);
            MeshService.getInstance().sendMeshMessage(message);
        }
    };


    class ViewHolder extends RecyclerView.ViewHolder {


        private TextView tv_ele;
        private Switch switch_ele;

        public ViewHolder(View itemView) {
            super(itemView);
        }
    }
}
