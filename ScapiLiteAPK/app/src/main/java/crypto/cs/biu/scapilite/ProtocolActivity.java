package crypto.cs.biu.scapilite;

import android.content.res.AssetManager;
import android.os.AsyncTask;

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
        protocolMain(_manager, _partyId);
        return null;
    }

    public native void protocolMain(AssetManager assetManager, String partyId);

}
