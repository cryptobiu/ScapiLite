package com.inellipse.biumatrix.dto;

import com.inellipse.biumatrix.model.User;

public class UserDTO {


    private String id;
    private String password;
    private boolean active;
    private String name;
    private String email;
    private String facebookId;
    private String gender;
    private String googleId;
    private String image;
    private String username;

    public UserDTO() {
    }

    public UserDTO(User u) {
        this.id = u.getId();
        this.name = u.getName();
        this.email = u.getEmail();
        this.facebookId = u.getFacebookId();
        this.gender = u.getGender();
        this.googleId = u.getGoogleId();
        this.active = u.isActive();
        this.image = u.getImage();
        this.username = u.getUsername();
    }


    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getEmail() {
        return email;
    }

    public void setEmail(String email) {
        this.email = email;
    }

    public String getFacebookId() {
        return facebookId;
    }

    public void setFacebookId(String facebookId) {
        this.facebookId = facebookId;
    }

    public String getGender() {
        return gender;
    }

    public void setGender(String gender) {
        this.gender = gender;
    }

    public String getGoogleId() {
        return googleId;
    }

    public void setGoogleId(String googleId) {
        this.googleId = googleId;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public boolean isActive() {
        return active;
    }

    public void setActive(boolean active) {
        this.active = active;
    }

    public String getImage() {
        return image;
    }

    public void setImage(String image) {
        this.image = image;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }
}
