package biu.cs.crypto.scapilite.model;

import java.io.Serializable;

/**
 * Created by Blagojco on 12/04/2018- 08:58
 */

public class Poll implements Serializable
{

    public final static String POOL_NOT_ANSWERED = "default";
    public final static String POOL_ACCEPTED = "accepted";
    public final static String POOL_DECLINED = "declined";

    public final static String POOL_RESULT_TYPE_BASIC = "basic";
    public final static String POOL_RESULT_TYPE_HISTOGRAM = "histogram";


    private String id;
    private String title;
    private String name;
    private String description;
    private long executionTime;
    private boolean active;
    private long userRegistrationSecondsBeforeExecution;
    private String status;
//    private String resultType;
    private String resultType;

    public String getId()
    {
        return id;
    }

    public void setId(String id)
    {
        this.id = id;
    }

    public String getName()
    {
        return name;
    }

    public void setName(String name)
    {
        this.name = name;
    }

    public String getTitle()
    {
        return title;
    }

    public void setTitle(String title)
    {
        this.title = title;
    }

    public String getDescription()
    {
        return description;
    }

    public void setDescription(String description)
    {
        this.description = description;
    }

    public Long getExecutionTime()
    {
        return executionTime;
    }

    public void setExecutionTime(Long executionTime)
    {
        this.executionTime = executionTime;
    }

    public boolean getActive()
    {
        return active;
    }

    public void setActive(boolean active)
    {
        this.active = active;
    }

    public long getUserRegistrationSecondsBeforeExecution()
    {
        return userRegistrationSecondsBeforeExecution;
    }

    public void setUserRegistrationSecondsBeforeExecution(long userRegistrationSecondsBeforeExecution)
    {
        this.userRegistrationSecondsBeforeExecution = userRegistrationSecondsBeforeExecution;
    }

    public String getStatus()
    {
        return status;
    }

    public void setStatus(String status)
    {
        this.status = status;
    }

    public String getResultType()
    {
        return resultType;
    }

    public void setResultType(String resultType)
    {
        this.resultType = resultType;
    }

    @Override
    public String toString()
    {
        return "Poll{" +
                "id='" + id + '\'' +
                ", title='" + title + '\'' +
                ", name='" + name + '\'' +
                ", description='" + description + '\'' +
                ", executionTime=" + executionTime +
                ", active=" + active +
                ", userRegistrationSecondsBeforeExecution=" + userRegistrationSecondsBeforeExecution +
                ", status='" + status + '\'' +
                ", resultType='" + resultType + '\'' +
                '}';
    }
}
