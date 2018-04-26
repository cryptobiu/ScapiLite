package crypto.cs.biu.scapilite;

import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;

import java.io.IOException;

public class ProtocolActivity extends AsyncTask<Void, Void, Void>
{
    AssetManager _manager;
    String _partyId;

    ProtocolActivity(AssetManager m, String partyId)
    {
        _manager = m; _partyId = partyId;
    }

    @Override
    protected Void doInBackground(Void... arg0)
    {
        protocolMain(_manager, _partyId,
                Environment.getExternalStorageDirectory().getAbsolutePath());

        return null;
    }

    public native void protocolMain(AssetManager assetManager, String partyId, String filesPath);

}
