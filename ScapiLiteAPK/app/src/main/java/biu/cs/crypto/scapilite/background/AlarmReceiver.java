package biu.cs.crypto.scapilite.background;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.util.Log;
import android.widget.Toast;

import com.android.volley.DefaultRetryPolicy;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import biu.cs.crypto.scapilite.ProtocolActivity;
import biu.cs.crypto.scapilite.model.MatrixResponse;
import biu.cs.crypto.scapilite.util.ConnectionHelper;
import biu.cs.crypto.scapilite.util.PreferencesManager;
import biu.cs.crypto.scapilite.util.constants.AppConstants;
import biu.cs.crypto.scapilite.util.Logger;

import static biu.cs.crypto.scapilite.util.Logger.logError;

/**
 * Created by Juca on 6/20/2018.
 */

public class AlarmReceiver extends BroadcastReceiver
{

    private String pollId;
    private String pollName;
    private String pollAnswer;

    @Override
    public void onReceive(Context context, Intent intent)
    {
        Logger.log("AlarmReceiver1 onReceive");
//        Poll poll = (Poll) intent.getExtras().getSerializable(AppConstants.CHOOSEN_POLL);
        String pollStatus = intent.getExtras().getString(AppConstants.POLL_STATUS);
        pollId = intent.getExtras().getString(AppConstants.POLL_ID);
        pollName = intent.getExtras().getString(AppConstants.POLL_NAME);
        pollAnswer = intent.getExtras().getString(AppConstants.POLL_ANSWER);
        Logger.log("AlarmReceiver1 pollStatus " + pollStatus);

        Toast.makeText(context, "AlarmReceived " + pollStatus, Toast.LENGTH_SHORT).show();


        if (pollStatus.equals(AppConstants.POLL_STATUS_READY_FOR_PREPARATION))
        {
            prepareOnlinePoll(context, pollName);
        }
        else if (pollStatus.equals(AppConstants.POLL_STATUS_READY_FOR_EXECUTION))
        {
            Logger.log("AlarmReceiver1 before execution " + pollAnswer);
            Logger.log("AlarmReceiver1 before execution pollId " + pollId);
            MatrixResponse response = PreferencesManager.getMatrixResponse();

            AssetManager mgr = context.getResources().getAssets();
            ProtocolActivity protocolActivity = new ProtocolActivity(context, mgr, response, PreferencesManager.getCiruitFileString(), pollAnswer);
            protocolActivity.execute(pollId);

            Logger.log("AlarmReceiver1 executed");
        }
    }


    public void prepareOnlinePoll(final Context context, String pollName)
    {
        RequestQueue queue = Volley.newRequestQueue(context);

        String url = ConnectionHelper.PREPARE_ONLINE_POLL + pollName + "/" + PreferencesManager.getMacAddress()/*PreferencesManager.getIp()*/;
        Logger.log("AlarmReceiver prepareOnlinePoll " + url);


        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>()
                {
                    @Override
                    public void onResponse(String response)
                    {
                        Logger.log("getPollParams response  " + response);
                        Toast.makeText(context, "getPollParams response " + response, Toast.LENGTH_LONG).show();
                        MatrixResponse matrixResponse = new Gson().fromJson(response, new TypeToken<MatrixResponse>()
                        {
                        }.getType());


                        PreferencesManager.putMatrixResponse(matrixResponse);


                        Logger.log("AlarmReceiver prepareOnlinePoll " + matrixResponse.toString());
                        getCircuitFile(context, matrixResponse.getCircuitFile());

                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                Logger.logError("AlarmReceiver prepareOnlinePoll ", error);
            }
        });

        // Add the request to the RequestQueue.
        queue.add(stringRequest);


    }


//    private void downloadFile(Context context, String url)
//    {
////        Uri downloadUri = Uri.parse(url);
////        DownloadManager.Request request = new DownloadManager.Request(downloadUri);
////        request.setDescription("Downloading a file");
////        DownloadManager downloadManager = (DownloadManager) getSystemService(Context.DOWNLOAD_SERVICE);
////        long id = downloadManager.enqueue(request.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_WIFI | DownloadManager.Request.NETWORK_MOBILE)
////                .setAllowedOverRoaming(false)
////                .setTitle("File Downloading...")
////                .setDescription("Image File Download")
////                .setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, ""));
//
//        File file = new File(Environment.getExternalStorageDirectory().getAbsolutePath(), "parties.txt");
//        if (file.exists())
//        {
//            file.delete();
//        }
//
//        long downloadReference;
//        Uri partiesFileUrl = Uri.parse(url);
//
//        DownloadManager downloadManager = (DownloadManager) context.getSystemService(DOWNLOAD_SERVICE);
//        DownloadManager.Request request = new DownloadManager.Request(partiesFileUrl);
//
//        request.setTitle("Data Download");
//        request.setDescription("Android parties file download");
//
//        log("AlarmReceiver downloading: " + url);
//        //set download destination
//        request.setDestinationInExternalFilesDir(context, Environment.getExternalStorageDirectory().getAbsolutePath(), "parties.txt");
//
//
//        //Enqueue download and save into referenceId
//        downloadReference = downloadManager.enqueue(request);
//
//        BroadcastReceiver onComplete = new BroadcastReceiver()
//        {
//            public void onReceive(Context ctxt, Intent intent)
//            {
//                log("onComplete onReceive ");
//            }
//        };

//        context.registerReceiver(onComplete, new IntentFilter(DownloadManager.ACTION_DOWNLOAD_COMPLETE));


//    }

    public static String readFileAsString(File file)
    {
        String result = "";
        if (file.exists())
        {
            //byte[] buffer = new byte[(int) new File(filePath).length()];
            FileInputStream fis = null;
            try
            {
                //f = new BufferedInputStream(new FileInputStream(filePath));
                //f.read(buffer);

                fis = new FileInputStream(file);
                char current;
                while (fis.available() > 0)
                {
                    current = (char) fis.read();
                    result = result + String.valueOf(current);
                }
            }
            catch (Exception e)
            {
                Log.d("TourGuide", e.toString());
            }
            finally
            {
                if (fis != null)
                {
                    try
                    {
                        fis.close();
                    }
                    catch (IOException ignored)
                    {
                    }
                }
            }
            //result = new String(buffer);
        }
        return result;
    }

    private void getCircuitFile(final Context context, String url)
    {
        RequestQueue queue = Volley.newRequestQueue(context);
        Logger.log("getCircuitFile call " + url);

        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>()
                {
                    @Override
                    public void onResponse(String response)
                    {
                        Logger.log("getCircuitFile response  " + response);
                        PreferencesManager.putCiruitFileString(response);
                    }
                }, new Response.ErrorListener()
        {
            @Override
            public void onErrorResponse(VolleyError error)
            {
                Logger.logError("getCircuitFile error ", error);
            }
        });

        stringRequest.setRetryPolicy(new DefaultRetryPolicy(
                120000,
                DefaultRetryPolicy.DEFAULT_MAX_RETRIES,
                DefaultRetryPolicy.DEFAULT_BACKOFF_MULT));
        // Add the request to the RequestQueue.
        queue.add(stringRequest);

    }
}
