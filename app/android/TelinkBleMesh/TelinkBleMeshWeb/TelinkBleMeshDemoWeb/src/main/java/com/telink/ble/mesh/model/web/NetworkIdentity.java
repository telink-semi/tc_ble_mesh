package com.telink.ble.mesh.model.web;

/**
 * 网络分享状态
 *
 * @see MeshNetworkShareInfo#getState()
 * 0-申请中； 1-正常； 2-被禁用；
 */
public enum NetworkIdentity {

    /**
     * 创建者
     */
    CREATOR(0, "创建者"),

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
    public final int identity;
    public final String desc;

    NetworkIdentity(int identity, String desc) {
        this.identity = identity;
        this.desc = desc;
    }

    public static NetworkIdentity getByIdentity(int identity) {
        for (NetworkIdentity st : values()) {
            if (st.identity == identity) {
                return st;
            }
        }
        return null;
    }

}
