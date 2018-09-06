package biu.cs.crypto.scapilite;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;

import com.facebook.AccessToken;

import biu.cs.crypto.scapilite.R;
import biu.cs.crypto.scapilite.ui.activity.LoginActivity;
import biu.cs.crypto.scapilite.ui.activity.PollsActivity;
import biu.cs.crypto.scapilite.util.Helper;
import biu.cs.crypto.scapilite.util.PreferencesManager;
import biu.cs.crypto.scapilite.util.SecurityHelper;
import biu.cs.crypto.scapilite.util.constants.AppConstants;
import biu.cs.crypto.scapilite.util.Logger;

import static biu.cs.crypto.scapilite.util.Logger.logError;


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
        Logger.log("getMacAddress  " + PreferencesManager.getMacAddress());

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
        Logger.log("AccessToken.getCurrentAccessToken() " + AccessToken.getCurrentAccessToken());

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

