package crypto.cs.biu.scapilite;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.res.AssetManager;
import android.media.RingtoneManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Environment;
import android.support.v4.app.NotificationCompat;
import android.util.Log;
import android.widget.Toast;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import crypto.cs.biu.scapilite.model.MatrixResponse;
import crypto.cs.biu.scapilite.util.PreferencesManager;

import static crypto.cs.biu.scapilite.util.Logger.log;

public class ProtocolActivity extends AsyncTask<String, Void, String>
{
    // Used to load the 'native-lib' library on application startup.
    static
    {
        System.loadLibrary("primitives");
    }

    private final Context context;

    AssetManager _manager;
    MatrixResponse matrixResponse;
    String circuitContent;
    String inputAnswer;

    public ProtocolActivity(Context context, AssetManager m, MatrixResponse matrixResponse, String circuitContent, String inputAnswer)
    {
        this.context = context;
        _manager = m;
        this.matrixResponse = matrixResponse;
        this.circuitContent = circuitContent;
        this.inputAnswer = inputAnswer;
    }

    @Override
    protected String doInBackground(String... pollId)
    {
        //hardcoded in the lib
        String proxyAddress = "";

        log("callingTheProtocolMain _manager= " + _manager);
        log("callingTheProtocolMain PartyID= " + matrixResponse.getPartyID());
        log("callingTheProtocolMain PartiesNumber= " + matrixResponse.getPartiesNumber());
        log("callingTheProtocolMain inputarg0[0]= OLD: " + pollId[0]);
        log("callingTheProtocolMain input= OLD: " + PreferencesManager.getPollAnswer(pollId[0]));
        log("callingTheProtocolMain input= " + inputAnswer);
        log("callingTheProtocolMain OutputFile= " + matrixResponse.getOutputFile());
        log("callingTheProtocolMain circuitContent= " + circuitContent);
        log("callingTheProtocolMain FieldType= " + matrixResponse.getFieldType());
        log("callingTheProtocolMain InternalIterationsNumber= " + matrixResponse.getInternalIterationsNumber());


        String output = protocolMain(_manager, matrixResponse.getPartyID(), matrixResponse.getPartiesNumber(), inputAnswer,
                matrixResponse.getOutputFile(), circuitContent,
                matrixResponse.getFieldType(), matrixResponse.getInternalIterationsNumber());

        ArrayList<String> wordArrayList = new ArrayList<>();
        for(String word : output.split(" "))
            wordArrayList.add(word);

        log("Poll stats are :\navg:" + Integer.parseInt(wordArrayList.get(0)) /
                        Integer.parseInt(matrixResponse.getPartiesNumber())
                        + "\nstd:" + wordArrayList.get(1));


        if (output != null && !output.trim().equals(""))
        {
            return output;

        }
        else
        {
            return "N/A";
        }
    }


    public native String protocolMain(AssetManager assetManager, String partyId, String partiesNumber,
                                      String inputFile, String outputFile, String circuitFile,
                                      String fieldType, String internalIterationsNumber);

    @Override
    protected void onPostExecute(String result)
    {
        Toast.makeText(context, "protocolMain finished ", Toast.LENGTH_SHORT).show();

        sendNotification(result);
        super.onPostExecute(result);
    }

    private void sendNotification(String messageBody)
    {
        Intent intent = new Intent(context, MainActivity.class);
        intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0 /* Request code */, intent,
                PendingIntent.FLAG_ONE_SHOT);

        String channelId = "channel_id";
        Uri defaultSoundUri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
        NotificationCompat.Builder notificationBuilder =
                new NotificationCompat.Builder(context, channelId)
                        .setSmallIcon(R.mipmap.ic_launcher)
                        .setContentTitle(context.getString(R.string.app_name))
                        .setContentText(messageBody)
                        .setAutoCancel(true)
                        .setSound(defaultSoundUri)
                        .setContentIntent(pendingIntent);

        NotificationManager notificationManager =
                (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        // Since android Oreo notification channel is needed.
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationChannel channel = new NotificationChannel(channelId,
                    "Channel human readable title",
                    NotificationManager.IMPORTANCE_DEFAULT);
            notificationManager.createNotificationChannel(channel);
        }

        notificationManager.notify(0 /* ID of notification */, notificationBuilder.build());
    }

}
