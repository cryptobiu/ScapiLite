package crypto.cs.biu.scapilite;

import android.content.res.AssetManager;
import android.os.AsyncTask;
import android.os.Environment;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

public class ProtocolActivity extends AsyncTask<Void, Void, Void>
{
    AssetManager _manager;
    String _partyId;
    List<String> _args;

    ProtocolActivity(AssetManager m, List<String> args)
    {
        _manager = m; _args = args;
    }

    @Override
    protected Void doInBackground(Void... arg0)
    {
        String partyId = "";
        String partiesNumber = "";
        String inputFile = "";
        String outputFile = "";
        String circuitFile= "";
        String proxyAddress = "";
        String fieldType = "";
        String internalIterationsNumber = "";
        String NG = "";
        partyId = _args.get(0);
        partiesNumber = _args.get(1);
        inputFile = _args.get(2);
        outputFile = _args.get(3);
        circuitFile= _args.get(4);
        proxyAddress = _args.get(5);
        fieldType = _args.get(6);
        internalIterationsNumber = _args.get(7);
        NG = _args.get(8);

        downloadCircuit(circuitFile);

        protocolMain(_manager, partyId, partiesNumber, inputFile, outputFile, circuitFile,
                proxyAddress, fieldType, internalIterationsNumber, NG,
                Environment.getExternalStorageDirectory().getAbsolutePath());

        return null;
    }

    private void downloadCircuit(String circuitFile)
    {
        try
        {
            URL u = new URL(circuitFile);
            HttpURLConnection c = (HttpURLConnection) u.openConnection();
            c.setRequestMethod("GET");
            c.setDoOutput(true);
            c.connect();
            FileOutputStream f = new FileOutputStream(
                    new File(Environment.getExternalStorageDirectory().getAbsolutePath()));

            InputStream in = c.getInputStream();

            byte[] buffer = new byte[1024];
            int len1 = 0;
            while ( (len1 = in.read(buffer)) > 0 )
                f.write(buffer,0, len1);
            f.close();

        }
        catch(MalformedURLException e)
        {
            Log.println(Log.ERROR, "Error", e.getMessage());
        }
        catch(IOException e)
        {
            Log.println(Log.ERROR, "Error", e.getMessage());
        }
    }

    public native void protocolMain(AssetManager assetManager, String partyId, String partiesNumber,
                                    String inputFile, String outputFile, String circuitFile,
                                    String proxyAddress, String fieldType,
                                    String internalIterationsNumber, String NG, String filesPath);

}
