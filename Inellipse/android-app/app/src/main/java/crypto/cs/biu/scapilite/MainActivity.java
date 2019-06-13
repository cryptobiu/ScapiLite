package crypto.cs.biu.scapilite;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.app.ProgressDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.android.volley.AuthFailureError;
import com.android.volley.DefaultRetryPolicy;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;
import com.squareup.picasso.Picasso;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Calendar;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.TimeUnit;

import crypto.cs.biu.scapilite.adapter.PollAdapter;
import crypto.cs.biu.scapilite.background.AlarmReceiver;
import crypto.cs.biu.scapilite.model.OfflineInstance;
import crypto.cs.biu.scapilite.model.Poll;
import crypto.cs.biu.scapilite.model.User;
import crypto.cs.biu.scapilite.ui.activity.PollResultActivity;
import crypto.cs.biu.scapilite.util.Alerts;
import crypto.cs.biu.scapilite.util.ConnectionHelper;
import crypto.cs.biu.scapilite.util.DateTimeParser;
import crypto.cs.biu.scapilite.util.PreferencesManager;
import crypto.cs.biu.scapilite.util.SecurityHelper;
import crypto.cs.biu.scapilite.util.constants.AppConstants;
import de.hdodenhof.circleimageview.CircleImageView;

import static crypto.cs.biu.scapilite.util.Logger.log;
import static crypto.cs.biu.scapilite.util.Logger.logError;

public class MainActivity extends AppCompatActivity implements PollAdapter.OnPollClick
{
    private CircleImageView profile_image;
    private TextView user_name;
    private TextView new_poll_starttime;
    private TextView new_poll_title;
    private TextView new_poll_desc;
    private TextView poll_countdown;
    private TextView poll_results;
    private TextView poll_status_text;
    private LinearLayout poll_accepted_wrapper;
    private LinearLayout poll_answer_wrapper;
    private Button poll_answer_button;
    private LinearLayout poll_declined_wrapper;
    private LinearLayout poll_not_answered_wrapper;
    private LinearLayout next_poll_wrapper;
    private Poll nextPoll;
    private User loggedUser;
    private EditText poll_answer_edittext;
    private TextView poll_total_users;
    private MyCount counter;
    private String participationType = "";
    private String pollStatus = "";
    private ProgressDialog creatingInstanceDialog;
//    private boolean isExecuting = false;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        getSupportActionBar().setDisplayShowHomeEnabled(true);
        getSupportActionBar().setIcon(R.mipmap.ic_launcher);

        init();
        PreferencesManager.putNextPoll(nextPoll);
        populatePoll(nextPoll);
    }

    @Override
    protected void onResume()
    {
        super.onResume();
    }

    private void populatePoll(Poll nextPoll)
    {
        try
        {
            new_poll_title.setText(nextPoll.getTitle());
            new_poll_desc.setText(nextPoll.getDescription());
            new_poll_starttime.setText(DateTimeParser.parseStartTimeyyyyMMddHHmm(nextPoll.getExecutionTime()));

            if (nextPoll.getExecutionTime() > new Date().getTime())
            {
                startCountdownTimer(nextPoll.getExecutionTime());

                if (nextPoll.getStatus().equals(Poll.POOL_ACCEPTED))
                {
                    poll_status_text.setText(getString(R.string.you_accepted));
                    poll_accepted_wrapper.setVisibility(View.VISIBLE);
                    poll_declined_wrapper.setVisibility(View.GONE);
                    poll_not_answered_wrapper.setVisibility(View.GONE);
                    poll_answer_wrapper.setVisibility(View.VISIBLE);
                    showUserAnswerDisabled();

                }
                else if (nextPoll.getStatus().equals(Poll.POOL_DECLINED))
                {

                    poll_status_text.setText(getString(R.string.you_declined));
                    poll_declined_wrapper.setVisibility(View.VISIBLE);
                    poll_accepted_wrapper.setVisibility(View.GONE);
                    poll_not_answered_wrapper.setVisibility(View.GONE);

                }
                else if (nextPoll.getStatus().equals(Poll.POOL_NOT_ANSWERED))
                {
                    poll_status_text.setText(getString(R.string.do_you_want_to_join));
                    poll_not_answered_wrapper.setVisibility(View.VISIBLE);
                    poll_declined_wrapper.setVisibility(View.GONE);
                    poll_accepted_wrapper.setVisibility(View.GONE);
                }
            }
            else
            {
                //TODO SHOW RESULT
                poll_status_text.setText("");
                poll_countdown.setText(getString(R.string.poll_already_executed));
            }


            if (nextPoll.getExecutionTime() < new Date().getTime() + nextPoll.getUserRegistrationSecondsBeforeExecution())
            {
                hideAcceptPollOptions();
            }

            next_poll_wrapper.setVisibility(View.VISIBLE);

        }
        catch (Exception e)
        {
            next_poll_wrapper.setVisibility(View.GONE);
            logError("populatePoll error ", e);
        }
    }

    private void hideAcceptPollOptions()
    {
        poll_answer_wrapper.setVisibility(View.GONE);
        poll_not_answered_wrapper.setVisibility(View.GONE);
        poll_declined_wrapper.setVisibility(View.GONE);
        poll_accepted_wrapper.setVisibility(View.GONE);

        if (!nextPoll.getStatus().equals(Poll.POOL_ACCEPTED))
        {
            poll_status_text.setText(getString(R.string.time_over_cannot_join));
        }
//        else
//        {
//            showAnswerEditText();
//        }

//        getTotalUsersOfPoll();
    }

//    private void showAnswerEditText()
//    {
//        poll_answer_wrapper.setVisibility(View.VISIBLE);
//
//    }


//    private void startPollExecutionService()
//    {
////        Intent intent = new Intent(this, PollExecutionService.class);
////        startService(intent);
//        executePoll(null);
//    }


    private void init()
    {

        nextPoll = (Poll) getIntent().getSerializableExtra(AppConstants.CHOOSEN_POLL);
        participationType = getIntent().getStringExtra(AppConstants.PARTICIPATION_TYPE);
        pollStatus = getIntent().getStringExtra(AppConstants.POLL_STATUS);
        profile_image = (CircleImageView) findViewById(R.id.profile_image);
        user_name = (TextView) findViewById(R.id.user_name);
        user_name.setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View v)
            {
                openPollResultActivity(String.valueOf(new Random().nextInt(5000)));
            }
        });
        new_poll_starttime = (TextView) findViewById(R.id.new_poll_starttime);
        new_poll_title = (TextView) findViewById(R.id.new_poll_title);
        new_poll_desc = (TextView) findViewById(R.id.new_poll_desc);
        poll_status_text = (TextView) findViewById(R.id.poll_status_text);
        poll_countdown = (TextView) findViewById(R.id.poll_countdown);
        poll_results = (TextView) findViewById(R.id.poll_results);
        poll_results.setVisibility(View.GONE);
        next_poll_wrapper = (LinearLayout) findViewById(R.id.next_poll_wrapper);
        poll_accepted_wrapper = (LinearLayout) findViewById(R.id.poll_accepted_wrapper);
        poll_answer_edittext = findViewById(R.id.poll_answer_edittext);
        poll_answer_wrapper = (LinearLayout) findViewById(R.id.poll_answer_wrapper);
        poll_total_users = (TextView) findViewById(R.id.poll_total_users);
        poll_total_users.setVisibility(View.GONE);
        poll_answer_button = (Button) findViewById(R.id.poll_answer_button);
        poll_declined_wrapper = (LinearLayout) findViewById(R.id.poll_declined_wrapper);
        poll_not_answered_wrapper = (LinearLayout) findViewById(R.id.poll_not_answered_wrapper);

        loggedUser = PreferencesManager.getUser();
        if (loggedUser != null)
        {
            user_name.setText(loggedUser.getName());
            if (loggedUser.getImage() != null)
            {
                Picasso.with(this).load(Uri.parse(loggedUser.getImage())).into(profile_image);
            }
        }
    }

//    public void executePoll(View view)
//    {
//
//        if (this != null)
//        {
//            next_poll_wrapper.setVisibility(View.GONE);
//
//            final SpotsDialog dialog = new SpotsDialog(this);
//            dialog.show();
//
//            AssetManager mgr = getResources().getAssets();
//            ProtocolActivity protocolActivity = new ProtocolActivity(this, mgr, "0");
//            protocolActivity.doInBackground();
//
//            //TODO sert input PreferencesManager.getPollAnswer(nextPoll.getId());
//
////            protocolInit();
////            setInput();
////            run();
////
////
////            final Handler handler = new Handler();
////            handler.postDelayed(new Runnable() {
////                @Override
////                public void run() {
////                    String output = getOutput();
////                    poll_results.setText(output);
////                    poll_results.setVisibility(View.VISIBLE);
////                    dialog.dismiss();
////                }
////            }, 5000);
//        }
//
//        openPollResultActivity(String.valueOf(new Random().nextInt(5000)));
//    }

    private void openPollResultActivity(String result)
    {
        Intent intent = new Intent(MainActivity.this, PollResultActivity.class);
        intent.putExtra(AppConstants.CHOOSEN_POLL, nextPoll);
        intent.putExtra(AppConstants.POLL_RESULT, result);
        startActivity(intent);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
//    public native void protocolInit();
//
//    public native void setInput();
//
//    public native void run();
//
//    public native String getOutput();
    public void declineNextPoll(View view)
    {
        final AlertDialog dialog = Alerts.showAreYouSureDialog(this, getString(R.string.decline), getString(R.string.decline_question), null, null);
        dialog.getButton(DialogInterface.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener()
        {
            @Override
            public void onClick(View view)
            {
                respondToPoll(Poll.POOL_DECLINED);
                dialog.dismiss();
            }
        });
    }

    public void acceptNextPoll(View view)
    {
        respondToPoll(Poll.POOL_ACCEPTED);
    }

    private void respondToPoll(String status)
    {
        RequestQueue queue = Volley.newRequestQueue(this);
        String url = ConnectionHelper.BASE_URL + "polls/details?pollId=" + nextPoll.getId() + "&userId=" + loggedUser.getId() + "&status=" + status;

        Log.d("acceptNextPoll", url);

        StringRequest stringRequest = new StringRequest(Request.Method.POST, url,
                new Response.Listener<String>()
                {
                    @Override
                    public void onResponse(String response)
                    {
                        log("response " + response);
                        nextPoll = new Gson().fromJson(response, new TypeToken<Poll>()
                        {
                        }.getType());

                        PreferencesManager.putNextPoll(nextPoll);

                        populatePoll(nextPoll);
                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("Error", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        queue.add(stringRequest);
    }


    private void getTotalUsersOfPoll()
    {
        RequestQueue queue = Volley.newRequestQueue(this);
        String url = ConnectionHelper.BASE_URL + "poll/" + nextPoll.getId() + "/users";

        Log.d("getTotalUsersOfPoll", url);

        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>()
                {
                    @Override
                    public void onResponse(String response)
                    {
                        log("response " + response);
                        List<User> users = new Gson().fromJson(response, new TypeToken<List<User>>()
                        {
                        }.getType());

                        if (users != null)
                        {
                            showTotalUsers(users.size());
                        }
                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("getTotalUsersOfPoll Error", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        queue.add(stringRequest);
    }

    private void showTotalUsers(int size)
    {
        poll_total_users.setText(getString(R.string.total_users) + " " + size);
        poll_total_users.setVisibility(View.VISIBLE);

    }

    public void startCountdownTimer(long dateTime)
    {
        long diff;
        long newLong = new Date().getTime();
        diff = dateTime - newLong;

        if (counter != null)
        {
            counter.cancel();
        }

        counter = new MyCount(diff, 1000);
        counter.start();
    }

    public void saveAnswer(View view)
    {
        log("saveAnswer participationType " + participationType);
        if (!poll_answer_edittext.getText().toString().isEmpty())
        {
            poll_answer_button.setEnabled(false);
            //ONLINE
            if (participationType.equals(AppConstants.PARTICIPATION_TYPE_ONLINE))
            {

                registerToPoll(nextPoll);
                PreferencesManager.putPollAnswer(nextPoll.getId(), poll_answer_edittext.getText().toString());
            }
            else
            //OFFLINE
            {
                sharePublicKey();

            }
        }

        showUserAnswerDisabled();

    }

    private void registerToPoll(final Poll nextPoll)
    {
        final String myIpAddress = getIpAddress();
        log("myIpAddress " + myIpAddress);
        PreferencesManager.putIp(myIpAddress);
        //
        RequestQueue queue = Volley.newRequestQueue(this);
        String url = ConnectionHelper.BASE_URL_MATRIX + "/polls/registerToPoll/" + nextPoll.getName() + "/" + PreferencesManager.getMacAddress()/*myIpAddress*/ + "/online_mobile/";

        log("registerToPoll url " + url);
        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>()
                {
                    @Override
                    public void onResponse(String response)
                    {
                        log("registerToPoll" + response);
                        setAlarmsForPreparingAndExecution(nextPoll);
                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("registerToPoll Error, That didn't work!", error);
            }
        });

        // Add the request to the RequestQueue.
        queue.add(stringRequest);
//
//        // download parties file
//        downloadData();
    }

    private void setAlarmsForPreparingAndExecution(Poll nextPoll)
    {
        log("setAlarmsForPreparingAndExecution nextPoll " + nextPoll.toString());
//        Calendar caltEST = Calendar.getInstance();
//        caltEST.add(Calendar.SECOND, 5);

        AlarmManager alarmMgr0 = (AlarmManager) getSystemService(Context.ALARM_SERVICE);

        Calendar calPrepare = Calendar.getInstance();
        //prepare 15 seconds later
        Date preparationTime = new Date(nextPoll.getExecutionTime() - nextPoll.getUserRegistrationSecondsBeforeExecution() * 1000 + 110000);
        log("setAlarmsForPreparingAndExecution preparationTime " + preparationTime);
        calPrepare.setTime(preparationTime);

        Intent intent0 = new Intent(this, AlarmReceiver.class);
        intent0.putExtra(AppConstants.POLL_STATUS, AppConstants.POLL_STATUS_READY_FOR_PREPARATION);
        intent0.putExtra(AppConstants.POLL_ID, nextPoll.getId());
        intent0.putExtra(AppConstants.POLL_NAME, nextPoll.getName());
        intent0.putExtra(AppConstants.POLL_ANSWER, PreferencesManager.getPollAnswer(nextPoll.getId()));
        intent0.setAction("matrix.alarm");
        PendingIntent pendingIntent0 = PendingIntent.getBroadcast(this, 12020, intent0, PendingIntent.FLAG_ONE_SHOT);
        alarmMgr0.setExact(AlarmManager.RTC_WAKEUP, calPrepare.getTimeInMillis(), pendingIntent0);


        Calendar calExecute = Calendar.getInstance();
        calExecute.setTime(new Date(nextPoll.getExecutionTime()));
        log("setAlarmsForPreparingAndExecution getExecutionTime" + new Date(nextPoll.getExecutionTime()));

        Intent intent1 = new Intent(this, AlarmReceiver.class);
        intent1.putExtra(AppConstants.POLL_STATUS, AppConstants.POLL_STATUS_READY_FOR_EXECUTION);
        intent1.putExtra(AppConstants.POLL_ID, nextPoll.getId());
        intent1.putExtra(AppConstants.POLL_ANSWER, PreferencesManager.getPollAnswer(nextPoll.getId()));
        intent1.putExtra(AppConstants.POLL_NAME, nextPoll.getName());
        intent1.setAction("matrix.alarm");
        PendingIntent pendingIntent1 = PendingIntent.getBroadcast(this, 12021, intent1, PendingIntent.FLAG_ONE_SHOT);
        alarmMgr0.setExact(AlarmManager.RTC_WAKEUP, calExecute.getTimeInMillis(), pendingIntent1);

    }


    private String getIpAddress()
    {
        try
        {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
                 en.hasMoreElements(); )
            {
                NetworkInterface ni = en.nextElement();
                if (ni.getDisplayName().compareTo("wlan0") == 0)
                {
                    for (Enumeration<InetAddress> enumIpAddr = ni.getInetAddresses();
                         enumIpAddr.hasMoreElements(); )
                    {
                        InetAddress inetAddress = enumIpAddr.nextElement();
                        if (!inetAddress.isLoopbackAddress())
                        {
                            return inetAddress.getHostAddress();
                        }
                    }
                }
            }
        }

        catch (Exception ex)
        {
            Log.e("IP Address", ex.toString());
        }

        return "";
    }


    private void showUserAnswerDisabled()
    {

        if (PreferencesManager.getPollAnswer(nextPoll.getId()) != null && !PreferencesManager.getPollAnswer(nextPoll.getId()).isEmpty())
        {
            poll_answer_button.setVisibility(View.GONE);
            poll_answer_edittext.setText(PreferencesManager.getPollAnswer(nextPoll.getId()));
            poll_answer_edittext.setEnabled(false);
        }
    }


    @Override
    public void onPollClick(Poll item)
    {
        nextPoll = item;
        PreferencesManager.putNextPoll(item);
        populatePoll(item);
    }

    // countdowntimer is an abstract class, so extend it and fill in methods
    public class MyCount extends CountDownTimer
    {
        MyCount(long millisInFuture, long countDownInterval)
        {
            super(millisInFuture, countDownInterval);
        }

        @Override
        public void onFinish()
        {
            if (nextPoll.getStatus().equals(Poll.POOL_ACCEPTED))
            {

//                startPollExecutionService();
                showUserAnswerDisabled();
            }

            poll_countdown.setVisibility(View.GONE);


        }

        @Override
        public void onTick(long millisUntilFinished)
        {
            long millis = millisUntilFinished;

//            log("onTick1 " + millis);
//            log("onTick2 " + nextPoll.getUserRegistrationSecondsBeforeExecution());
            if (millis < nextPoll.getUserRegistrationSecondsBeforeExecution() * 1000)
            {
//                log("onTick3 " + millis);
                hideAcceptPollOptions();

            }
            String dayText = "";
            String hourText = "";
            String minuteText = "";
            String secondText = "";

            if ((TimeUnit.MILLISECONDS.toDays(millis)) > 0)
            {
                dayText = TimeUnit.MILLISECONDS.toDays(millis) + " " + getString(R.string.day) + ", ";
            }

            long hours = TimeUnit.MILLISECONDS.toHours(millis) - TimeUnit.DAYS.toHours(TimeUnit.MILLISECONDS.toDays(millis));
            if (hours > 0 || !dayText.equals(""))
            {
                if (hours < 10)
                {
                    hourText = "0";
                }
                hourText += hours + ":";
            }


            long minutes = (TimeUnit.MILLISECONDS.toMinutes(millis) - TimeUnit.HOURS.toMinutes(TimeUnit.MILLISECONDS.toHours(millis)));
            if (minutes < 10)
            {
                minuteText = "0";
            }
            minuteText += minutes + ":";

            long seconds = (TimeUnit.MILLISECONDS.toSeconds(millis) - TimeUnit.MINUTES.toSeconds(TimeUnit.MILLISECONDS.toMinutes(millis)));
            if (seconds > 0 || !dayText.equals("") || !hourText.equals("") || !minuteText.equals(""))
            {
                if (seconds < 10)
                {
                    secondText = "0";
                }
                secondText += seconds;
            }

            String hms = dayText
                    + hourText
                    + minuteText
                    + secondText;

            poll_countdown.setText(/*context.getString(R.string.ends_in) + " " +*/ hms);
        }
    }


//    private boolean allFileExists()
//    {
//        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "parties.conf");
//
//        if (!file.exists())
//        {
//            return false;
//        }
//
//        return true;
//    }


    //--------------------------- OFFLINE MODE----------------------------//

    private void sharePublicKey()
    {
//        String url = ConnectionHelper.BASE_URL + "containers/start?publicKey=" + PreferencesManager.getPublic();
        String url = ConnectionHelper.BASE_URL + "offline-instance/create";

        JSONObject jsonObject = new JSONObject();
        try
        {
            jsonObject.put("pollId", nextPoll.getId());
            jsonObject.put("publicKey", PreferencesManager.getPublicPem());
        }
        catch (JSONException e)
        {
            e.printStackTrace();
        }

        log("sharePublicKey jsonObject " + jsonObject);

        final JsonObjectRequest request = new JsonObjectRequest(Request.Method.POST, url, jsonObject, new Response.Listener<JSONObject>()
        {
            @Override
            public void onResponse(JSONObject response)
            {
                log("sharePublicKey " + response);
                OfflineInstance offlineInstance = new Gson().fromJson(response.toString(), new TypeToken<OfflineInstance>()
                {
                }.getType());

                PreferencesManager.putOfflineInstancePerPoll(offlineInstance);

                if (offlineInstance != null) {
                    creatingInstanceDialog = ProgressDialog.show(MainActivity.this, getString(R.string.creating_instance), getString(R.string.please_wait), true);
                    getInstanceInfo(offlineInstance.getId(), poll_answer_edittext.getText().toString());
                }
            }
        }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("sharePublicKey error ", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        request.setRetryPolicy(new DefaultRetryPolicy(
                30000,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

        RequestQueue queue = Volley.newRequestQueue(this);
        queue.add(request);

    }


    private void getInstanceInfo(final String instanceId, final String answer)
    {
        RequestQueue queue = Volley.newRequestQueue(this);

        String url = ConnectionHelper.BASE_URL + "offline-instance/" + instanceId;

        log("getInstanceInfo " + url);

        JsonObjectRequest jsonRequest = new JsonObjectRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONObject>()
                {
                    @Override
                    public void onResponse(JSONObject response)
                    {
                        log("getInstanceInfo " + response.toString());
                        OfflineInstance offlineInstance = new Gson().fromJson(response.toString(), new TypeToken<OfflineInstance>()
                        {
                        }.getType());

                        if (offlineInstance != null && offlineInstance.getIp() != null && !offlineInstance.getIp().equals(""))
                        {
                            PreferencesManager.putOfflineInstancePerPoll(offlineInstance);
                            sendAnswer(offlineInstance, answer);

                        }
                        else
                        {
                            final Handler handler = new Handler();
                            handler.postDelayed(new Runnable()
                            {
                                @Override
                                public void run()
                                {
                                    getInstanceInfo(instanceId, answer);
                                }
                            }, 2000);
                        }
                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("getInstanceInfo error", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        queue.add(jsonRequest);
    }

    private void sendAnswer(OfflineInstance offlineInstance, String answer)
    {
        //encryptedAnswer is still not used. it is TODO
        String encryptedAnswer = answer;
        try
        {
            String simetricKey = SecurityHelper.decryptRSAToString(offlineInstance.getEncryptedEcryptionKey(), PreferencesManager.getPrivate(), "");

            log("simetricKey plain " + simetricKey);
            log("simetricKey plain length " + simetricKey.length());
            log("simetricKey plain getBytes().length " + simetricKey.getBytes().length);
            encryptedAnswer = SecurityHelper.encrypt11(answer, simetricKey);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }


//        String url = ConnectionHelper.BASE_URL + "containers/start?publicKey=" + PreferencesManager.getPublic();
        String url = "http://" + offlineInstance.getIp() + ":" + offlineInstance.getPort() + "/poll-answer";
        JSONObject jsonObject = new JSONObject();
        try
        {
            jsonObject.put("answer", answer);
        }
        catch (JSONException e)
        {
            e.printStackTrace();
        }

        log("sendAnswer jsonObject " + jsonObject);

        final JsonObjectRequest request = new JsonObjectRequest(Request.Method.POST, url, jsonObject, new Response.Listener<JSONObject>()
        {
            @Override
            public void onResponse(JSONObject response)
            {
                if (MainActivity.this != null) {
                    log("sendAnswer response " + response);
                    if (creatingInstanceDialog != null && creatingInstanceDialog.isShowing()) {
                        creatingInstanceDialog.dismiss();
                    }

                    final AlertDialog dialog = Alerts.showAlert(MainActivity.this, getString(R.string.instance_created), getString(R.string.instance_created_text), getString(R.string.ok));
                    dialog.getButton(DialogInterface.BUTTON_POSITIVE).setOnClickListener(new View.OnClickListener() {
                        @Override
                        public void onClick(View view) {
                            dialog.dismiss();
                            showUserAnswerDisabled();
                        }
                    });
                }


            }
        }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                logError("sendAnswer error ", error);
            }
        })
        {
            @Override
            public Map<String, String> getHeaders() throws AuthFailureError
            {
                Map<String, String> headers = new HashMap<>();
//                headers.put("Authorization", PreferencesManager.getAccessToken());
                return headers;
            }
        };

        request.setRetryPolicy(new DefaultRetryPolicy(
                30000,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));

        RequestQueue queue = Volley.newRequestQueue(this);
        queue.add(request);

    }
    //--------------------------- END OFFLINE MODE----------------------------//
}
