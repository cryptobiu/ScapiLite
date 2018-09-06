package biu.cs.crypto.scapilite.ui.activity;

import android.annotation.TargetApi;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonArrayRequest;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import biu.cs.crypto.scapilite.MainActivity;
import biu.cs.crypto.scapilite.R;
import biu.cs.crypto.scapilite.adapter.PollAdapter;
import biu.cs.crypto.scapilite.model.PageResponse;
import biu.cs.crypto.scapilite.model.Poll;
import biu.cs.crypto.scapilite.util.ConnectionHelper;
import biu.cs.crypto.scapilite.util.PreferencesManager;
import biu.cs.crypto.scapilite.util.constants.AppConstants;
import biu.cs.crypto.scapilite.util.Logger;

import static biu.cs.crypto.scapilite.util.Logger.logError;

/**
 * Created by Blagojco on 11/04/2018- 11:05
 */

public class PollsActivity extends AppCompatActivity implements PollAdapter.OnPollClick
{

    private RecyclerView polls_recyclerview;
    private LinearLayout no_poll_wrapper;
    private TextView no_poll;

    @TargetApi(Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_polls);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.mipmap.ic_launcher);

        init();


    }

    @Override
    protected void onResume()
    {
        super.onResume();
        getAllPolls();
    }

    private void init()
    {
        polls_recyclerview = (RecyclerView) findViewById(R.id.polls_recyclerview);
        polls_recyclerview.setLayoutManager(new LinearLayoutManager(this));


        no_poll = (TextView) findViewById(R.id.no_poll);
        no_poll_wrapper = (LinearLayout) findViewById(R.id.no_poll_wrapper);
        no_poll_wrapper.setVisibility(View.GONE);
    }

    private void getAllPolls()
    {
        RequestQueue queue = Volley.newRequestQueue(this);

        String url = ConnectionHelper.BASE_URL + "polls?active=true";

        Logger.log("getAllPolls " + url);

        JsonArrayRequest jsonRequest = new JsonArrayRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONArray>()
                {
                    @Override
                    public void onResponse(JSONArray response)
                    {
                        Logger.log("getAllPolls " + response.toString());
                        no_poll_wrapper.setVisibility(View.GONE);
                        List<Poll> activePolls = new Gson().fromJson(response.toString(), new TypeToken<List<Poll>>()
                        {
                        }.getType());

                        populateActivePolls(activePolls);

                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                no_poll_wrapper.setVisibility(View.VISIBLE);
                Logger.logError("getAllPolls error", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                Logger.log("TOKEN " + PreferencesManager.getAccessToken());
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        queue.add(jsonRequest);
    }


    private void getAllPollsPageable()
    {
        RequestQueue queue = Volley.newRequestQueue(this);

        String url = ConnectionHelper.BASE_URL + "polls/next/pageable";

        Logger.log("getAllPolls " + url);

        JsonObjectRequest jsonRequest = new JsonObjectRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONObject>()
                {
                    @Override
                    public void onResponse(JSONObject response)
                    {
                        Logger.log("getAllPolls " + response.toString());
                        no_poll_wrapper.setVisibility(View.GONE);
                        PageResponse activePolls = new Gson().fromJson(response.toString(), new TypeToken<PageResponse>()
                        {
                        }.getType());

                        populateActivePolls(activePolls.getContent());

                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                no_poll_wrapper.setVisibility(View.VISIBLE);
                Logger.logError("getAllPolls error", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                Logger.log("TOKEN " + PreferencesManager.getAccessToken());
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        queue.add(jsonRequest);
    }

    private void populateActivePolls(List<Poll> activePolls)
    {

        PollAdapter adapter = new PollAdapter(this, activePolls, this);
        polls_recyclerview.setAdapter(adapter);
    }

    @Override
    public void onPollClick(Poll item)
    {
        if (item.getExecutionTime() < new Date().getTime())
        {
            Intent intent = new Intent(PollsActivity.this, MainActivity.class);
            intent.putExtra(AppConstants.CHOOSEN_POLL, item);
            intent.putExtra(AppConstants.POLL_STATUS, "");
            intent.putExtra(AppConstants.PARTICIPATION_TYPE, AppConstants.PARTICIPATION_TYPE_ONLINE);
            startActivity(intent);
        }
        else
        {
            Intent intent = new Intent(PollsActivity.this, PollsOnlineOfflineActivity.class);
            intent.putExtra(AppConstants.CHOOSEN_POLL, item);
            startActivity(intent);
//        finish();
        }
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