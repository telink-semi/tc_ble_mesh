package com.telink.ble.mesh.web.entity;

/**
 * 网络身份
 *
 * 0-创建者； 1-2-正常； 3-被禁用；
 */
public enum NetworkRole {

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
    public final int role;
    public final String desc;

    NetworkRole(int role, String desc) {
        this.role = role;
        this.desc = desc;
    }

    public static NetworkRole getByRole(int role) {
        for (NetworkRole st : values()) {
            if (st.role == role) {
                return st;
            }
        }
        return null;
    }

}
