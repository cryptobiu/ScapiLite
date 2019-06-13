package com.inellipse.biumatrix.dto;

import com.inellipse.biumatrix.model.Poll;

public class PollDTO {

    private String id;
    private String title;
    private String description;
    private String name;
    private Long executionTime;
    private Boolean active;
    private Long userRegistrationSecondsBeforeExecution;
    private String status;
    private long totalAccepted;
    private String resultType;

    public PollDTO() {
    }

    public PollDTO(Poll poll) {
        this.id = poll.getId();
        this.title = poll.getTitle();
        this.description = poll.getDescription();
        this.name = poll.getName();
        this.executionTime = poll.getExecutionTime();
        this.active = poll.getActive();
        this.userRegistrationSecondsBeforeExecution = poll.getUserRegistrationSecondsBeforeExecution();
        this.resultType = poll.getResultType();
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Long getExecutionTime() {
        return executionTime;
    }

    public void setExecutionTime(Long executionTime) {
        this.executionTime = executionTime;
    }

    public Boolean getActive() {
        return active;
    }

    public void setActive(Boolean active) {
        this.active = active;
    }

    public Long getUserRegistrationSecondsBeforeExecution() {
        return userRegistrationSecondsBeforeExecution;
    }

    public void setUserRegistrationSecondsBeforeExecution(Long userRegistrationSecondsBeforeExecution) {
        this.userRegistrationSecondsBeforeExecution = userRegistrationSecondsBeforeExecution;
    }

    public String getStatus() {
        return status;
    }

    public void setStatus(String status) {
        this.status = status;
    }

    public long getTotalAccepted() {
        return totalAccepted;
    }

    public void setTotalAccepted(long totalAccepted) {
        this.totalAccepted = totalAccepted;
    }

    public String getResultType() {
        return resultType;
    }

    public void setResultType(String resultType) {
        this.resultType = resultType;
    }
}
