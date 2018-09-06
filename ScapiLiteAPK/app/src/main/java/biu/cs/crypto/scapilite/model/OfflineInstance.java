package biu.cs.crypto.scapilite.model;

/**
 * Created by Juca on 6/17/2018.
 */

public class OfflineInstance
{
    private String id;
    private String externalId;
    private String userId;
    private String pollId;
    private boolean answer;
    private String ip;
    private long port;
    private String encryptedEcryptionKey;

    public String getId()
    {
        return id;
    }

    public void setId(String id)
    {
        this.id = id;
    }

    public String getExternalId()
    {
        return externalId;
    }

    public void setExternalId(String externalId)
    {
        this.externalId = externalId;
    }

    public String getUserId()
    {
        return userId;
    }

    public void setUserId(String userId)
    {
        this.userId = userId;
    }

    public String getPollId()
    {
        return pollId;
    }

    public void setPollId(String pollId)
    {
        this.pollId = pollId;
    }

    public boolean isAnswer()
    {
        return answer;
    }

    public void setAnswer(boolean answer)
    {
        this.answer = answer;
    }

    public String getIp()
    {
        return ip;
    }

    public void setIp(String ip)
    {
        this.ip = ip;
    }

    public long getPort()
    {
        return port;
    }

    public void setPort(long port)
    {
        this.port = port;
    }

    public String getEncryptedEcryptionKey()
    {
        return encryptedEcryptionKey;
    }

    public void setEncryptedEcryptionKey(String encryptedEcryptionKey)
    {
        this.encryptedEcryptionKey = encryptedEcryptionKey;
    }
}
