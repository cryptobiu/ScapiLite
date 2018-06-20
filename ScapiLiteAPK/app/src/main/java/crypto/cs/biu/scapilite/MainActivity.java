package crypto.cs.biu.scapilite;

import android.app.DownloadManager;
import android.content.res.AssetManager;
import android.net.Uri;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.EditText;

import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static { System.loadLibrary("primitives"); }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
//        runProtocol();
    }

    private void runProtocol()
    {
        AssetManager mgr = getResources().getAssets();
        ProtocolActivity protocolActivity = new ProtocolActivity(mgr, "0");
        protocolActivity.doInBackground();
    }

    @Override
    public void onClick (View view)
    {
        final String myIpAddress = getIpAddress();
        RequestQueue queue = Volley.newRequestQueue(this);
        String url = "http://35.171.69.162/polls/registerToPoll/HyperMPC/"
                + myIpAddress + "/online_mobile/";

        // Request a string response from the provided URL.
        StringRequest stringRequest = new StringRequest(Request.Method.GET, url,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        // Display the first 500 characters of the response string.
                        Log.println(Log.INFO, "Info", response);
                    }
                }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Log.println(Log.ERROR, "Error", "That didn't work!");
            }
        });

        // Add the request to the RequestQueue.
        queue.add(stringRequest);
//
//        // download parties file
//        downloadData();


    }


    private String getIpAddress()
    {
        try
        {
            for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces();
                    en.hasMoreElements();)
            {
                NetworkInterface ni = en.nextElement();
                if(ni.getDisplayName().compareTo("wlan0") == 0)
                {
                    for (Enumeration<InetAddress> enumIpAddr = ni.getInetAddresses();
                         enumIpAddr.hasMoreElements();)
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

    private long downloadData()
    {
        // the code snippet taken from
        // https://www.androidtutorialpoint.com/networking/android-download-manager-tutorial-download-file-using-download-manager-internet/

        long downloadReference;
        Uri partiesFileUrl = Uri.parse("http://35.171.69.162/polls/parties");

        DownloadManager downloadManager = (DownloadManager)getSystemService(DOWNLOAD_SERVICE);
        DownloadManager.Request request = new DownloadManager.Request(partiesFileUrl);

        request.setTitle("Data Download");
        request.setDescription("Android parties file download");

        //set download destination

        request.setDestinationInExternalFilesDir(this,
                Environment.getExternalStorageDirectory().getAbsolutePath(),
                "parties.conf");

        //Enqueue download and save into referenceId
        downloadReference = downloadManager.enqueue(request);
        return downloadReference;
    }
}
