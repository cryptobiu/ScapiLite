package com.inellipse.biumatrix.security;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

import java.util.List;

@JsonIgnoreProperties(ignoreUnknown = true)
public class AuthenticatedUser {

    @JsonProperty("user_id")
    private String userId;

    @JsonProperty("user_name")
    private String username;

    @JsonProperty("authorities")
    private List<String> roles;

    @JsonProperty("scope")
    private List<String> scopes;

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public List<String> getRoles() {
        return roles;
    }

    public void setRoles(List<String> roles) {
        this.roles = roles;
    }

    public List<String> getScopes() {
        return scopes;
    }

    public void setScopes(List<String> scopes) {
        this.scopes = scopes;
    }

    @Override
    public String toString() {
        return "AuthenticatedUser{" +
                "userId='" + userId + '\'' +
                ", username='" + username + '\'' +
                ", roles=" + roles +
                ", scopes=" + scopes +
                '}';
    }
}