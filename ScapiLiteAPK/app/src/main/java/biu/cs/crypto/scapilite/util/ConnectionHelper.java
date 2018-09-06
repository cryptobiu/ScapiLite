package biu.cs.crypto.scapilite.util;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;

import biu.cs.crypto.scapilite.R;


public class ConnectionHelper
{

    //    public static String BASE_URL = "http://10.0.7.60:8088/biumatrix/";
//    public static String BASE_URL_WEB = "http://10.0.7.60:4200/";
//    public static String BASE_URL = "http://biumatrix.inellipse.com/biumatrix/";
//    public static String BASE_URL_WEB = "http://biu.inellipse.com/";
    public static String BASE_URL = "http://privatepoll.biu-mpc.io/biumatrix/";

    //Matrix
    public static String BASE_URL_MATRIX = "http://35.171.69.162";
    public static final String PREPARE_ONLINE_POLL = BASE_URL_MATRIX + "/polls/getPollParams/";
    public static final String GET_PARTIES_FILE = BASE_URL_MATRIX + "/polls/parties";
    public static final String GET_CIRCUIT_FILE = BASE_URL_MATRIX + "/polls/circuit";

    public static boolean isConnectedToInternet(Context context)
    {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo netInfo = cm.getActiveNetworkInfo();
        return netInfo != null && netInfo.isConnectedOrConnecting();
    }

    public static void turnOnWireless(Context context)
    {
        try
        {
            WifiManager wifi = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            if (!wifi.isWifiEnabled())
            {
                wifi.setWifiEnabled(true);
            }
        }
        catch (Exception var2)
        {
        }

    }

    public static boolean isConnectedWifi(Context context)
    {
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connectivityManager.getNetworkInfo(1);
        return networkInfo.isConnected();
    }

    public static String getConnectionType(Context context)
    {
        ConnectivityManager cm = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo wifi = cm.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        NetworkInfo mobile = cm.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
        return wifi.isConnectedOrConnecting() ? "wifi" : (mobile.isConnectedOrConnecting() ? "lte" : null);
    }

    public static boolean isConnected3G(Context context)
    {
        ConnectivityManager connectivityManager = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo networkInfo = connectivityManager.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);
        return networkInfo.isConnected();
    }

    public static void showNetworkProblemDialog(Context context)
    {
        Alerts.showAlert(context, context.getString(R.string.network_problem_title), context.getString(R.string.network_problem_text), context.getString(R.string.ok));
    }

}
