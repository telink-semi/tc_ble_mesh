/********************************************************************************************************
 * @file NetworkShareState.java
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
package com.telink.ble.mesh.model.web;

import com.telink.ble.mesh.demo.R;

/**
 * 网络分享状态
 *
 * @see MeshNetworkShareInfo#getState()
 * 0-申请中； 1-正常； 2-被禁用；
 */
public enum NetworkShareState {

    /**
     * 由申请人发起申请时创建
     */
    APPLYING(0, "申请中"),

    /**
     * 允许以游客角色访问， 允许组网和发送配置指令
     */
    VISITOR(1, "访客"),

    /**
     * 允许以成员角色访问， 允许组网和发送配置指令
     */
    MEMBER(2, "成员"),

    FORBIDDEN(3, "被禁用"),
    ;
    public final int state;
    public final String desc;

    NetworkShareState(int state, String desc) {
        this.state = state;
        this.desc = desc;
    }

    public static NetworkShareState getByState(int state) {
        for (NetworkShareState st : values()) {
            if (st.state == state) {
                return st;
            }
        }
        return null;
    }


    /**
     * 检查新的状态是否正确， 只能传入 Visitor， Member， Disabled
     */
    public static boolean checkNewState(int state) {
        return state == VISITOR.state || state == MEMBER.state || state == FORBIDDEN.state;
    }


    /**
     * @return resourceId
     */
    public static int getColor(int state) {
        if (state == MEMBER.state) {
            return R.color.network_st_member;
        } else if (state == VISITOR.state) {
            return R.color.network_st_visitor;
        } else if (state == APPLYING.state) {
            return R.color.network_st_applying;
        } else if (state == FORBIDDEN.state) {
            return R.color.network_st_forbidden;
        }
        return R.color.colorPrimary;
    }
}
