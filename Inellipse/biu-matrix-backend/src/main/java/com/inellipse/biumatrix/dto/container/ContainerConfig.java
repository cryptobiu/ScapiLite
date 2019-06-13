package com.inellipse.biumatrix.dto.container;

import java.util.HashMap;
import java.util.Map;

public class ContainerConfig {

    private String userId;
    private String userPublicKey;
    private Map<String, String> data = new HashMap<>();

    public String getUserId() {
        return userId;
    }

    public ContainerConfig setUserId(String userId) {
        this.userId = userId;
        return this;
    }

    public String getUserPublicKey() {
        return userPublicKey;
    }

    public ContainerConfig setUserPublicKey(String userPublicKey) {
        this.userPublicKey = userPublicKey;
        return this;
    }

    public Map<String, String> getData() {
        return data;
    }

    public void setData(Map<String, String> data) {
        this.data = data;
    }

    public void putData(String key, String value) {
        this.data.put(key, value);
    }
}
