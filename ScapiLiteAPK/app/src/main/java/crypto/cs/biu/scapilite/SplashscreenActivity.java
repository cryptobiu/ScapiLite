package crypto.cs.biu.scapilite;

import android.Manifest;
import android.app.Activity;
import android.app.AlarmManager;
import android.app.DownloadManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.Toast;

import com.android.volley.DefaultRetryPolicy;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.facebook.AccessToken;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Calendar;

import crypto.cs.biu.scapilite.background.AlarmReceiver;
import crypto.cs.biu.scapilite.model.MatrixResponse;
import crypto.cs.biu.scapilite.ui.activity.LoginActivity;
import crypto.cs.biu.scapilite.ui.activity.PollsActivity;
import crypto.cs.biu.scapilite.util.ConnectionHelper;
import crypto.cs.biu.scapilite.util.Helper;
import crypto.cs.biu.scapilite.util.PreferencesManager;
import crypto.cs.biu.scapilite.util.SecurityHelper;
import crypto.cs.biu.scapilite.util.constants.AppConstants;

import static crypto.cs.biu.scapilite.util.Logger.log;
import static crypto.cs.biu.scapilite.util.Logger.logError;


public class SplashscreenActivity extends AppCompatActivity
{
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splashscreen);
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        SecurityHelper.generatePrivateAndPublicKeys(AppConstants.KEY_PAR_TRANS);
        PreferencesManager.putMacAddress(Helper.getMacAddress(this));
        log("getMacAddress  " + PreferencesManager.getMacAddress());

        final Handler handler = new Handler();
        handler.postDelayed(new Runnable()
        {
            @Override
            public void run()
            {
                startApplication();
            }
        }, 2000);

    }

    public void startApplication()
    {
        log("AccessToken.getCurrentAccessToken() " + AccessToken.getCurrentAccessToken());

        if (AccessToken.getCurrentAccessToken() != null || (PreferencesManager.getAccessToken() != null && !PreferencesManager.getAccessToken().equals("")))
        {
            if (PreferencesManager.getUser() == null)
            {
                startActivity(new Intent(SplashscreenActivity.this, LoginActivity.class));
            }
            else
            {
                startActivity(new Intent(SplashscreenActivity.this, PollsActivity.class));
            }
            finish();

        }
    }


//        Calendar caltEST = Calendar.getInstance();
//        caltEST.add(Calendar.SECOND, 5);
//
//        //Create alarm manager
//        AlarmManager alarmMgr0 = (AlarmManager) getSystemService(Context.ALARM_SERVICE);
//
//        //Create pending intent & register it to your alarm notifier class
//        Intent intent0 = new Intent(this, AlarmReceiver.class);
//        intent0.putExtra(AppConstants.POLL_STATUS, AppConstants.POLL_STATUS_READY_FOR_PREPARATION);
//        intent0.putExtra(AppConstants.POLL_ID, "123");
//        intent0.putExtra(AppConstants.POLL_NAME, "Test031");
//        intent0.setAction("matrix.alarm");
//        PendingIntent pendingIntent0 = PendingIntent.getBroadcast(this, 12020, intent0, PendingIntent.FLAG_ONE_SHOT);
//        alarmMgr0.setExact(AlarmManager.RTC_WAKEUP, caltEST.getTimeInMillis(), pendingIntent0);

}

