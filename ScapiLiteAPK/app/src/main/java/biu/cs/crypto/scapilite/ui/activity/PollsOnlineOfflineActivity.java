package biu.cs.crypto.scapilite.ui.activity;

import android.annotation.TargetApi;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;

import biu.cs.crypto.scapilite.MainActivity;
import biu.cs.crypto.scapilite.R;
import biu.cs.crypto.scapilite.model.Poll;
import biu.cs.crypto.scapilite.util.PreferencesManager;
import biu.cs.crypto.scapilite.util.constants.AppConstants;

/**
 * Created by Blagojco on 11/04/2018- 11:05
 */

public class PollsOnlineOfflineActivity extends AppCompatActivity
{

    private Poll nextPoll;

    @TargetApi(Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_online_offline_polls);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.mipmap.ic_launcher);

        init();
    }


    private void init()
    {
        nextPoll = (Poll) getIntent().getSerializableExtra(AppConstants.CHOOSEN_POLL);
    }


    public void chooseOnlineMode(View view)
    {
        Intent intent = new Intent(PollsOnlineOfflineActivity.this, MainActivity.class);
        intent.putExtra(AppConstants.CHOOSEN_POLL, nextPoll);
        intent.putExtra(AppConstants.POLL_STATUS, "");
        intent.putExtra(AppConstants.PARTICIPATION_TYPE, AppConstants.PARTICIPATION_TYPE_ONLINE);
        startActivity(intent);
    }


    public void chooseOfflineMode(View view)
    {
        Intent intent = new Intent(PollsOnlineOfflineActivity.this, PollsPrivateTrustedActivity.class);
        intent.putExtra(AppConstants.CHOOSEN_POLL, nextPoll);
        startActivity(intent);
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