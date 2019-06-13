package crypto.cs.biu.scapilite.util;

import android.util.Log;

public class Logger {
    public static final String TAG = "Matrix";

    public static void log(String message) {
        Log.d(TAG, message);
    }

    public static void logError(String message, Exception e) {

        Log.e(TAG, message, e);
    }

    public static void logError(String message, Throwable e) {
        Log.e(TAG, message, e);
    }
}
