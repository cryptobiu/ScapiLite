package crypto.cs.biu.scapilite.background;

import android.app.IntentService;
import android.content.Intent;

import static crypto.cs.biu.scapilite.util.Logger.log;

public class PollExecutionService extends IntentService {

    public PollExecutionService() {
        super("PollExecutionService");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        String dataString = workIntent.getDataString();
        log("onHandleIntent " + dataString);
    }
}
