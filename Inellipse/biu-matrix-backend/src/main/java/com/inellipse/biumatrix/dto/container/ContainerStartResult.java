package com.inellipse.biumatrix.dto.container;

import com.amazonaws.services.ecs.model.RunTaskResult;

public class ContainerStartResult {

    private String id;
    private String cid;
    private boolean success;

    public ContainerStartResult(boolean success) {
        this.success = false;
    }

    public ContainerStartResult(RunTaskResult result, boolean success) {
        this(result, null, success);
    }

    public ContainerStartResult(RunTaskResult result, String cid, boolean success) {
        this.success = success;
        this.id = result.getTasks().get(0).getTaskArn();
        this.cid = cid;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getCid() {
        return cid;
    }

    public void setCid(String cid) {
        this.cid = cid;
    }

    public boolean isSuccess() {
        return success;
    }

    public void setSuccess(boolean success) {
        this.success = success;
    }

    public ContainerStartResult error() {
        this.success = false;
        return this;
    }

    @Override
    public String toString() {
        return "ContainerStartResult{" +
                "id='" + id + '\'' +
                ", cid='" + cid + '\'' +
                ", success=" + success +
                '}';
    }
}
