package com.inellipse.biumatrix.dto;

import com.inellipse.biumatrix.model.PollDetails;

public class PollDetailsDTO {

    private String id;
    private String pollId;
    private String userId;
    private String status;

    public PollDetailsDTO() {
    }

    public PollDetailsDTO(PollDetails pollDetails) {
        this.id = pollDetails.getId();
        this.pollId = pollDetails.getPollId();
        this.userId = pollDetails.getUserId();
        this.status = pollDetails.getStatus();
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getPollId() {
        return pollId;
    }

    public void setPollId(String pollId) {
        this.pollId = pollId;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }
}
