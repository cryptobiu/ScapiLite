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

import static crypto.cs.biu.scapilite.util.Logger.logError;


public class DeviceUtil {

    public static String getMacAdress(Context context) {
        String macaddress = getEthMacAddress();
        if (macaddress != null) {
            return macaddress;
        } else {
            macaddress = getWifiMacAdress(context);
            if (macaddress == null || macaddress.equals("02:00:00:00:00:00")) {
                macaddress = getMacAdressForAndroidM(context);
            }
        }
        return macaddress;
    }

    private static String getMacAdressForAndroidM(Context context) {
        try {
            String interfaceName = "wlan0";
            List<NetworkInterface> interfaces = Collections.list(NetworkInterface.getNetworkInterfaces());
            for (NetworkInterface intf : interfaces) {
                if (!intf.getName().equalsIgnoreCase(interfaceName)) {
                    continue;
                }

                byte[] mac = intf.getHardwareAddress();
                if (mac == null) {
                    return "";
                }

                StringBuilder buf = new StringBuilder();
                for (byte aMac : mac) {
                    buf.append(String.format("%02X:", aMac));
                }
                if (buf.length() > 0) {
                    buf.deleteCharAt(buf.length() - 1);
                }
                return buf.toString();
            }
        } catch (Exception ex) {
        } // for now eat exceptions
        return "unknown-mac-" + getManufacturerSerialNumber();
    }


    private static String getWifiMacAdress(Context context) {
        try {
            WifiManager manager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
            WifiInfo info = manager.getConnectionInfo();

            if (info.getMacAddress() != null) {
                return info.getMacAddress();
            } else {
                manager.setWifiEnabled(true);
                return info.getMacAddress();
            }
        } catch (Exception e) {
            logError("getWifiMacAdress 1 ", e);
            try {
                // turn on wifi and get wifi
                WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
                wifiManager.setWifiEnabled(true);
                WifiInfo info = wifiManager.getConnectionInfo();
                return info.getMacAddress();
            } catch (Exception e2) {
                logError("getWifiMacAdress 2 ", e);
                return null;
            }
        }
    }

    private static String getEthMacAddress() {
        try {
            return loadFileAsString("/sys/class/net/eth0/address").toUpperCase().substring(0, 17);
        } catch (IOException e) {
            return null;
        }
    }

    public static String loadFileAsString(String filePath) throws IOException {
        StringBuffer fileData = new StringBuffer(1000);
        BufferedReader reader = new BufferedReader(new FileReader(filePath));
        char[] buf = new char[1024];
        int numRead = 0;
        while ((numRead = reader.read(buf)) != -1) {
            String readData = String.valueOf(buf, 0, numRead);
            fileData.append(readData);
        }
        reader.close();
        return fileData.toString();
    }

    private static String getManufacturerSerialNumber() {
        String serial = null;
        try {
            Class<?> c = Class.forName("android.os.SystemProperties");
            java.lang.reflect.Method get = c.getMethod("get", String.class, String.class);
            serial = (String) get.invoke(c, "ril.serialnumber", "unknown");
        } catch (Exception e) {
            logError("getManufacturerSerialNumber catch ", e);
        }

        if (serial.equals("unknown")) {
            return android.os.Build.SERIAL;
        } else {
            return serial;
        }
    }

    public static boolean isHybridBox() {
        return false;
    }

}
