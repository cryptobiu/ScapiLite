package biu.cs.crypto.scapilite.application;

import android.app.Application;
import android.content.SharedPreferences;

import biu.cs.crypto.scapilite.util.constants.SharedPreferencesKeys;


public class AppController extends Application {

    private static AppController mInstance;
    private SharedPreferences appPreferences;

    public static AppController getInstance() {
        return mInstance;
    }

    public SharedPreferences getAppSharedPreferences() {
        if (appPreferences == null) {
            appPreferences = getSharedPreferences(SharedPreferencesKeys.PREF_USER_APPLICATION_PREFERENCES, MODE_PRIVATE);
        }
        return appPreferences;
    }


    @Override
    public void onCreate() {
        super.onCreate();
        mInstance = this;
    }

}


