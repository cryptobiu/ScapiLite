package crypto.cs.biu.scapilite.util;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.NetworkInterface;
import java.util.Collections;
import java.util.List;

import static crypto.cs.biu.scapilite.util.Logger.log;
import static crypto.cs.biu.scapilite.util.Logger.logError;

/**
 * Created by Juca on 6/22/2018.
 */

public class Helper
{

    public static String getMacAddress(Context context)
    {
        String macAddressFromShared = PreferencesManager.getMacAddress();
        log("macAddressFromShared " + macAddressFromShared);
        if (macAddressFromShared.equals(""))
        {
            String macaddress = getEthMacAddress();
            if (macaddress != null)
            {
                return macaddress.toUpperCase();
            }
            else
            {
                macaddress = getWifiMacAdress(context);
                if (macaddress == null || macaddress.equals("02:00:00:00:00:00"))
                {
                    macaddress = getMacAdressForAndroidM(context);
                }
            }

            return macaddress.toUpperCase();
        }
        else
        {
            return macAddressFromShared;

        }
    }

    private static String getMacAdressForAndroidM(Context context)
    {
        try
        {
            String interfaceName = "wlan0";
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces)
            {
                if (!intf.getName().equalsIgnoreCase(interfaceName))
                {
                    continue;
                }

                byte[] mac = intf.getHardwareAddress();
                if (mac == null)
                {
                    return "";
                }

                StringBuilder buf = new StringBuilder();
                for (byte aMac : mac)
                {
                    buf.append(String.format("%02X:", aMac));
                }
                if (buf.length() > 0)
                {
                    buf.deleteCharAt(buf.length() - 1);
                }
                return buf.toString();
            }
        }
        catch (Exception ex)
        {
            logError("getMacaddress", ex);
        } // for now eat exceptions
        return "";
    }

    private static String getWifiMacAdress(Context context)
    {
        try
        {
            log("getWifiMacAdress 1 ");
            WifiManager manager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = manager.getConnectionInfo();

            if (info.getMacAddress() != null)
            {

                return info.getMacAddress();
            }
            else
            {
                manager.setWifiEnabled(true);
                return info.getMacAddress();
            }
        }
        catch (Exception e)
        {
            logError("getWifiMacAdress 1 ", e);
            try
            {
                // turn on wifi and get wifi
                WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
                wifiManager.setWifiEnabled(true);
                WifiInfo info = wifiManager.getConnectionInfo();
                return info.getMacAddress();
            }
            catch (Exception e2)
            {
                //Mint.logException(e);
                logError("getWifiMacAdress 2 ", e);
                return null;
            }
        }
    }


    private static String getEthMacAddress()
    {
        try
        {
            return loadFileAsString("/sys/class/net/eth0/address").toUpperCase().substring(0, 17);
        }
        catch (IOException e)
        {
            return null;
        }
    }

    public static String loadFileAsString(String filePath) throws java.io.IOException
    {
        StringBuffer fileData = new StringBuffer(1000);
        BufferedReader reader = new BufferedReader(new FileReader(filePath));
        char[] buf = new char[1024];
        int numRead = 0;
        while ((numRead = reader.read(buf)) != -1)
        {
            String readData = String.valueOf(buf, 0, numRead);
            fileData.append(readData);
        }
        reader.close();
        return fileData.toString();
    }


}
