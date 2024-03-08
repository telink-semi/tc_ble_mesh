package com.telink.ble.mesh.model.web;

/**
 * scheduler action
 */
public enum SchedulerAction {


    OFF(0, "off"),

    ON(1, "on"),

    /**
     * 允许以成员角色访问， 允许组网和发送配置指令
     */
    SCENE(2, "scene"),

    NO(3, "no"),
    ;
    public final long action;
    public final String desc;

    SchedulerAction(long action, String desc) {
        this.action = action;
        this.desc = desc;
    }

    public static SchedulerAction getByAction(long action) {
        for (SchedulerAction st : values()) {
            if (st.action == action) {
                return st;
            }
        }
        return null;
    }

}
