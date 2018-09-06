package biu.cs.crypto.scapilite.background;

import android.app.IntentService;
import android.content.Intent;

import biu.cs.crypto.scapilite.util.Logger;

public class PollExecutionService extends IntentService {

    public PollExecutionService() {
        super("PollExecutionService");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        String dataString = workIntent.getDataString();
        Logger.log("onHandleIntent " + dataString);
    }
}
