package crypto.cs.biu.scapilite;

import android.content.res.AssetManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
//        System.loadLibrary("AndroidScapiLite");
        System.loadLibrary("primitives");
//        System.loadLibrary("gmp");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        AssetManager mgr = getResources().getAssets();
        ProtocolActivity protocolActivity = new ProtocolActivity(mgr);
        protocolActivity.doInBackground();
//        String res = protocolMain(mgr);
//        tv.setText(res);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String testLibs();
//    public native String protocolMain(AssetManager assetManager);
}
