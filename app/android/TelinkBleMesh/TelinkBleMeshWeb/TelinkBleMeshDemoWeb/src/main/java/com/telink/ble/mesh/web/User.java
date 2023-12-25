package com.telink.ble.mesh.web;

import java.io.Serializable;

/**
 * <p>
 *
 * </p>
 *
 * @author kee
 * @since 2022-11-24
 */
public class User implements Serializable {

    private static final long serialVersionUID = 1L;

    private Integer id;

    private String name;

    // ("用户密码， sha256值")
    private String password;

    // ("用户邮箱")
    private String email;

    // ("用户手机号码， 暂时不用")
    private String phone;

    // ("用户签名信息")
    private String sign;

    // ("用户昵称")
    private String nickname;

    private String company;

    private Long createTime;

    // ("更新信息的时间")
    private Long updateTime;

    // ("最后一次登录的时间")
    private Long lastLoginTime;


    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getPhone() {
        return phone;
    }

    public void setPhone(String phone) {
        this.phone = phone;
    }

    public String getSign() {
        return sign;
    }

    public void setSign(String sign) {
        this.sign = sign;
    }

    public String getNickname() {
        return nickname;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public String getCompany() {
        return company;
    }

    public void setCompany(String company) {
        this.company = company;
    }

    public Long getCreateTime() {
        return createTime;
    }

    public void setCreateTime(Long createTime) {
        this.createTime = createTime;
    }

    public Long getUpdateTime() {
        return updateTime;
    }

    public void setUpdateTime(Long updateTime) {
        this.updateTime = updateTime;
    }

    public Long getLastLoginTime() {
        return lastLoginTime;
    }

    public void setLastLoginTime(Long lastLoginTime) {
        this.lastLoginTime = lastLoginTime;
    }

    @Override
    public String toString() {
        return "User{" +
                "id=" + id +
                ", name=" + name +
                ", password=" + password +
                ", email=" + email +
                ", phone=" + phone +
                ", sign=" + sign +
                ", nickname=" + nickname +
                ", company=" + company +
                ", createTime=" + createTime +
                ", updateTime=" + updateTime +
                ", lastLoginTime=" + lastLoginTime +
                "}";
    }
}
