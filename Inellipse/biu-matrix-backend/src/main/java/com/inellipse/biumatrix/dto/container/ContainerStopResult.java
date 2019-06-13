package com.inellipse.biumatrix.dto.container;

public class ContainerStopResult {

    private String id;
    private boolean success;

    public ContainerStopResult(String id, boolean success) {
        this.id = id;
        this.success = success;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public boolean isSuccess() {
        return success;
    }

    public void setSuccess(boolean success) {
        this.success = success;
    }

    @Override
    public String toString() {
        return "ContainerStopResult{" +
                "id='" + id + '\'' +
                ", success=" + success +
                '}';
    }
}
