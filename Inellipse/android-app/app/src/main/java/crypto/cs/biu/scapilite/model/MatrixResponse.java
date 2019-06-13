package crypto.cs.biu.scapilite.model;

import com.google.gson.annotations.SerializedName;

/**
 * Created by Juca on 6/21/2018.
 */

public class MatrixResponse
{
    private String partyID;
    private String partiesNumber;
    private String inputFile;
    private String outputFile;
    private String circuitFile;
    private String partiesFile;
    private String fieldType;
    private String internalIterationsNumber;
    private String NG;
    private String circuitFileAddress;

    public String getPartyID()
    {
        return partyID;
    }

    public void setPartyID(String partyID)
    {
        this.partyID = partyID;
    }

    public String getPartiesNumber()
    {
        return partiesNumber;
    }

    public void setPartiesNumber(String partiesNumber)
    {
        this.partiesNumber = partiesNumber;
    }

    public String getInputFile()
    {
        return inputFile;
    }

    public void setInputFile(String inputFile)
    {
        this.inputFile = inputFile;
    }

    public String getOutputFile()
    {
        return outputFile;
    }

    public void setOutputFile(String outputFile)
    {
        this.outputFile = outputFile;
    }

    public String getCircuitFile()
    {
        return circuitFile;
    }

    public void setCircuitFile(String circuitFile)
    {
        this.circuitFile = circuitFile;
    }

    public String getPartiesFile()
    {
        return partiesFile;
    }

    public void setPartiesFile(String partiesFile)
    {
        this.partiesFile = partiesFile;
    }

    public String getFieldType()
    {
        return fieldType;
    }

    public void setFieldType(String fieldType)
    {
        this.fieldType = fieldType;
    }

    public String getInternalIterationsNumber()
    {
        return internalIterationsNumber;
    }

    public void setInternalIterationsNumber(String internalIterationsNumber)
    {
        this.internalIterationsNumber = internalIterationsNumber;
    }

    public String getNG()
    {
        return NG;
    }

    public void setNG(String NG)
    {
        this.NG = NG;
    }

    public String getCircuitFileAddress()
    {
        return circuitFileAddress;
    }

    public void setCircuitFileAddress(String circuitFileAddress)
    {
        this.circuitFileAddress = circuitFileAddress;
    }

    @Override
    public String toString()
    {
        return "MatrixResponse{" +
                "partyID='" + partyID + '\'' +
                ", partiesNumber='" + partiesNumber + '\'' +
                ", inputFile='" + inputFile + '\'' +
                ", outputFile='" + outputFile + '\'' +
                ", circuitFile='" + circuitFile + '\'' +
                ", partiesFile='" + partiesFile + '\'' +
                ", fieldType='" + fieldType + '\'' +
                ", internalIterationsNumber='" + internalIterationsNumber + '\'' +
                ", NG='" + NG + '\'' +
                ", circuitFileAddress='" + circuitFileAddress + '\'' +
                '}';
    }
}
