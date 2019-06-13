package com.inellipse.biumatrix.model;

import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.index.Indexed;
import org.springframework.data.mongodb.core.mapping.Document;

@Document(collection = "offline_instance")
public class OfflineInstance {

    @Id
    private String id;

    @Indexed
    private String userId;

    @Indexed
    private String pollId;

    private String publicKey;

    @Indexed
    private String externalId;

    @Indexed
    private String cid;

    private String ip;
    private String port;
    private String encryptedEcryptionKey;
    private Boolean answer;

    public OfflineInstance() {
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
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

    public String getPublicKey() {
        return publicKey;
    }

    public void setPublicKey(String publicKey) {
        this.publicKey = publicKey;
    }

    public String getExternalId() {
        return externalId;
    }

    public void setExternalId(String externalId) {
        this.externalId = externalId;
    }

    public String getCid() {
        return cid;
    }

    public void setCid(String cid) {
        this.cid = cid;
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

    public Boolean getAnswer() {
        return answer;
    }

    public void setAnswer(Boolean answer) {
        this.answer = answer;
    }
}
