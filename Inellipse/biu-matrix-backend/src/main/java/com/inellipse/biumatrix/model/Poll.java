package com.inellipse.biumatrix.model;

import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.index.Indexed;
import org.springframework.data.mongodb.core.mapping.Document;

@Document(collection = "poll")
public class Poll {

    @Id
    private String id;
    private String title;
    private String description;

    @Indexed
    private String name;

    @Indexed
    private Long executionTime;

    @Indexed
    private Boolean active;
    private Long userRegistrationSecondsBeforeExecution;
    private String resultType;

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

    public String getResultType() {
        return resultType;
    }

    public void setResultType(String resultType) {
        this.resultType = resultType;
    }

    @Override
    public String toString() {
        return "Poll{" +
                "id='" + id + '\'' +
                ", title='" + title + '\'' +
                ", description='" + description + '\'' +
                ", name='" + name + '\'' +
                ", executionTime=" + executionTime +
                ", active=" + active +
                ", userRegistrationSecondsBeforeExecution=" + userRegistrationSecondsBeforeExecution +
                ", resultType='" + resultType + '\'' +
                '}';
    }
}
