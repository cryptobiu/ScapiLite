package biu.cs.crypto.scapilite.ui.activity;

import android.annotation.TargetApi;
import android.app.ProgressDialog;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.CheckBox;

import biu.cs.crypto.scapilite.MainActivity;
import biu.cs.crypto.scapilite.R;
import biu.cs.crypto.scapilite.model.Poll;
import biu.cs.crypto.scapilite.util.Alerts;
import biu.cs.crypto.scapilite.util.PreferencesManager;
import biu.cs.crypto.scapilite.util.constants.AppConstants;

/**
 * Created by Blagojco on 11/04/2018- 11:05
 */

public class PollsPrivateTrustedActivity extends AppCompatActivity
{

    private Poll nextPoll;
    private CheckBox i_agree_checkbox;

    @TargetApi(Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_offline_polls);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.mipmap.ic_launcher);

        init();
    }


    private void init()
    {
        nextPoll = (Poll) getIntent().getSerializableExtra(AppConstants.CHOOSEN_POLL);
        i_agree_checkbox = findViewById(R.id.i_agree_checkbox);
    }


    public void chooseTrustedInstance(View view)
    {

        if (!i_agree_checkbox.isChecked())
        {
            Alerts.showAlert(PollsPrivateTrustedActivity.this, "", getString(R.string.must_accept), getString(R.string.ok));
            return;
        }
//        final ProgressDialog dialog = ProgressDialog.show(this, getString(R.string.creating_instance),
//                getString(R.string.please_wait), true);
//
//
//        final Handler handler = new Handler();
//        handler.postDelayed(new Runnable() {
//            @Override
//            public void run() {
//                dialog.dismiss();
//
//                String url = ConnectionHelper.BASE_URL_WEB + "poll/" + nextPoll.getId();
//                Intent i = new Intent(Intent.ACTION_VIEW);
//                i.setData(Uri.parse(url));
//                startActivity(i);
//            }
//        }, 10000);

        Intent intent = new Intent(PollsPrivateTrustedActivity.this, MainActivity.class);
        intent.putExtra(AppConstants.CHOOSEN_POLL, nextPoll);
        intent.putExtra(AppConstants.POLL_STATUS, "");
        intent.putExtra(AppConstants.PARTICIPATION_TYPE, AppConstants.PARTICIPATION_TYPE_OFFLINE);
        startActivity(intent);
    }


    public void choosePrivateInstance(View view)
    {
//        Intent intent = new Intent(PollsPrivateTrustedActivity.this, WebViewActivity.class);
//        intent.putExtra(AppConstants.CHOOSEN_POLL, nextPoll);
//        intent.putExtra(AppConstants.INTENT_WEB_URL, "https://hud-e.iron.io/signup"/*ConnectionHelper.BASE_URL_WEB + "privateInstance"*/);
//        startActivity(intent);

        Alerts.showAlert(this, getString(R.string.not_ready), getString(R.string.this_feature_is_not_ready), getString(R.string.ok));
        final ProgressDialog dialog = ProgressDialog.show(this, getString(R.string.creating_instance),
                getString(R.string.please_wait), true);


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_logout, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        switch (item.getItemId())
        {
            case R.id.logout:
                PreferencesManager.logoutUser();
                startActivity(new Intent(this, LoginActivity.class));

                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}