package crypto.cs.biu.scapilite.util;

import android.content.SharedPreferences;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import crypto.cs.biu.scapilite.application.AppController;
import crypto.cs.biu.scapilite.model.MatrixResponse;
import crypto.cs.biu.scapilite.model.OfflineInstance;
import crypto.cs.biu.scapilite.model.Poll;
import crypto.cs.biu.scapilite.model.User;
import crypto.cs.biu.scapilite.util.constants.SharedPreferencesKeys;

import static crypto.cs.biu.scapilite.util.Logger.log;


public class PreferencesManager
{

    private static SharedPreferences getContentSP()
    {
        return AppController.getInstance().getAppSharedPreferences();
    }

    public static void putAccessToken(String token)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.TOKEN, token).commit();
    }

    public static String getAccessToken()
    {
        return "Bearer " + getContentSP().getString(SharedPreferencesKeys.TOKEN, "");
    }


    public static void putUser(User user)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.USER, new Gson().toJson(user)).commit();
    }

    public static User getUser()
    {
        return new Gson().fromJson(getContentSP().getString(SharedPreferencesKeys.USER, ""), new TypeToken<User>()
        {
        }.getType());
    }


    public static void putNextPoll(Poll poll)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.NEXT_POLL, new Gson().toJson(poll)).commit();
    }

    public static Poll getNextPoll()
    {
        return new Gson().fromJson(getContentSP().getString(SharedPreferencesKeys.NEXT_POLL, ""), new TypeToken<Poll>()
        {
        }.getType());
    }


    public static void putPollAnswer(String pollId, String answer)
    {
        log("putPollAnswer111 " + SharedPreferencesKeys.POLL + "." + pollId + " answer= " + answer);
        getContentSP().edit().putString(SharedPreferencesKeys.POLL + "." + pollId, answer).commit();
    }

    public static String getPollAnswer(String pollId)
    {
        log("getPollAnswer111 " + SharedPreferencesKeys.POLL + "." + pollId + " answer= " + getContentSP().getString(SharedPreferencesKeys.POLL + "." + pollId, "") + "===");
        return getContentSP().getString(SharedPreferencesKeys.POLL + "." + pollId, "");
    }

//    public static void putLatestPollAnswer(String answer)
//    {
//        getContentSP().edit().putString(SharedPreferencesKeys.LATEST_POLL, answer).commit();
//    }
//
//
//    public static String getLatestPollAnswer()
//    {
//        return getContentSP().getString(SharedPreferencesKeys.LATEST_POLL, "");
//    }


    public static void logoutUser()
    {
        getContentSP().edit().clear().commit();
    }

    public static void putPrivate(String privKeyStr)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.PRIVATE, privKeyStr).commit();

    }

    public static String getPrivate()
    {
        log("getPrivate " + getContentSP().getString(SharedPreferencesKeys.PRIVATE, ""));
        return getContentSP().getString(SharedPreferencesKeys.PRIVATE, "");
    }


    public static void putPrivatePem(String privKeyStr)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.PRIVATE_PEM, privKeyStr).commit();

    }

    public static String getPrivatePem()
    {
        log("getPrivatePem " + getContentSP().getString(SharedPreferencesKeys.PRIVATE_PEM, ""));
        return getContentSP().getString(SharedPreferencesKeys.PRIVATE_PEM, "");
    }


    public static void putPublic(String pubKeyStr)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.PUBLIC, pubKeyStr).commit();
    }

    public static String getPublic()
    {
        log("getPublic" + getContentSP().getString(SharedPreferencesKeys.PUBLIC, ""));
        return getContentSP().getString(SharedPreferencesKeys.PUBLIC, "");
    }

    public static void putPublicPem(String pubKeyStr)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.PUBLIC_PEM, pubKeyStr).commit();
    }

    public static String getPublicPem()
    {
        log("getPublicPem" + getContentSP().getString(SharedPreferencesKeys.PUBLIC_PEM, ""));
        return getContentSP().getString(SharedPreferencesKeys.PUBLIC_PEM, "");
    }

    public static void putOfflineInstancePerPoll(OfflineInstance offlineInstance)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.OFFLINE_INSTANCE_PER_POLL + "." + offlineInstance.getPollId(), new Gson().toJson(offlineInstance)).commit();
    }

    public static OfflineInstance getOfflineInstancePerPoll(String pollId)
    {
        return new Gson().fromJson(getContentSP().getString(SharedPreferencesKeys.OFFLINE_INSTANCE_PER_POLL + "." + pollId, ""), new TypeToken<OfflineInstance>()
        {
        }.getType());
    }

    public static void putIp(String ip)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.IP, ip).commit();
    }

    public static String getIp()
    {
        return getContentSP().getString(SharedPreferencesKeys.IP, "");
    }

    public static void putMatrixResponse(MatrixResponse matrixResponse)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.MATRIX_RESPONSE, new Gson().toJson(matrixResponse)).commit();
    }

    public static MatrixResponse getMatrixResponse()
    {
        return new Gson().fromJson(getContentSP().getString(SharedPreferencesKeys.MATRIX_RESPONSE, ""), new TypeToken<MatrixResponse>()
        {
        }.getType());
    }

    public static void putCiruitFileString(String response)
    {
        getContentSP().edit().putString(SharedPreferencesKeys.CIRCUIT_STRING, response).commit();
    }

    public static String getCiruitFileString()
    {
        log("getCiruitFileString " + getContentSP().getString(SharedPreferencesKeys.CIRCUIT_STRING, ""));
        return getContentSP().getString(SharedPreferencesKeys.CIRCUIT_STRING, "");
    }

    public static void putMacAddress(String macAddress)
    {
        macAddress = macAddress.replace(":", "");
        log("putMacAddress " + macAddress);
        getContentSP().edit().putString(SharedPreferencesKeys.DEVICE_MAC_ADDRESS, macAddress.toUpperCase()).commit();
    }

    public static String getMacAddress()
    {
        return getContentSP().getString(SharedPreferencesKeys.DEVICE_MAC_ADDRESS, "").toUpperCase();
    }


}