package com.inellipse.biumatrix.dto;

import com.inellipse.biumatrix.model.OfflineInstance;

public class OfflineInstanceDTO {

    private String id;
    private String externalId;
    private String userId;
    private String pollId;
    private Boolean answer;
    private String ip;
    private String port;
    private String encryptedEcryptionKey;

    public OfflineInstanceDTO() {
    }

    public OfflineInstanceDTO(OfflineInstance offlineInstance) {
        this.id = offlineInstance.getId();
        this.externalId = offlineInstance.getExternalId();
        this.userId = offlineInstance.getUserId();
        this.pollId = offlineInstance.getPollId();
        this.answer = offlineInstance.getAnswer();
        this.ip = offlineInstance.getIp();
        this.port = offlineInstance.getPort();
        this.encryptedEcryptionKey = offlineInstance.getEncryptedEcryptionKey();
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getExternalId() {
        return externalId;
    }

    public void setExternalId(String externalId) {
        this.externalId = externalId;
    }

    public String getUserId() {
        return userId;
    }

    public void setUserId(String userId) {
        this.userId = userId;
    }

    public String getPollId() {
        return pollId;
    }

    public void setPollId(String pollId) {
        this.pollId = pollId;
    }

    public Boolean getAnswer() {
        return answer;
    }

    public void setAnswer(Boolean answer) {
        this.answer = answer;
    }

    public String getIp() {
        return ip;
    }

    public void setIp(String ip) {
        this.ip = ip;
    }

    public String getPort() {
        return port;
    }

    public void setPort(String port) {
        this.port = port;
    }

    public String getEncryptedEcryptionKey() {
        return encryptedEcryptionKey;
    }

    public void setEncryptedEcryptionKey(String encryptedEcryptionKey) {
        this.encryptedEcryptionKey = encryptedEcryptionKey;
    }
}
