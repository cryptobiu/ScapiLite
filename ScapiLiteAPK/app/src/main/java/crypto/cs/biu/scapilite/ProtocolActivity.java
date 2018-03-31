package crypto.cs.biu.scapilite;

import android.content.res.AssetManager;
import android.os.AsyncTask;

public class ProtocolActivity extends AsyncTask<Void, Void, Void>
{
    AssetManager _manager;

    ProtocolActivity(AssetManager m)
    {
        _manager = m;
    }

    @Override
    protected Void doInBackground(Void... arg0)
    {
        protocolMain(_manager);
        return null;
    }

    public native void protocolMain(AssetManager assetManager);

}
