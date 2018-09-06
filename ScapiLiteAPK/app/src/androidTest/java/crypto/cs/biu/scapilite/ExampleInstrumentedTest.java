package biu.cs.crypto.scapilite;

import android.content.Context;
import android.content.res.AssetManager;
import android.support.test.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.JUnit4;

import crypto.cs.biu.scapilite.ProtocolActivity;
import crypto.cs.biu.scapilite.model.MatrixResponse;
import crypto.cs.biu.scapilite.util.PreferencesManager;

import static org.junit.Assert.*;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(JUnit4.class)
public class ExampleInstrumentedTest {


    @Test
    public void readCircuit() throws Exception
    {
        Context context = InstrumentationRegistry.getContext();
        AssetManager assetManager = context.getAssets();
        String circuitContent = "10\n" +
                "3\n" +
                "\n" +
                "1 1\n" +
                "0\n" +
                "\n" +
                "2 1\n" +
                "1\n" +
                "\n" +
                "3 1\n" +
                "2\n" +
                "\n" +
                "1 2\n" +
                "7\n" +
                "12\n" +
                "\n" +
                "2 2\n" +
                "7\n" +
                "12\n" +
                "\n" +
                "3 2\n" +
                "7\n" +
                "12\n" +
                "\n" +
                "\n" +
                "2 1 0 0 3 2\n" +
                "2 1 1 1 4 2\n" +
                "2 1 2 2 5 2\n" +
                "2 1 0 1 6 1\n" +
                "2 1 2 6 7 1\n" +
                "2 1 3 4 8 1\n" +
                "2 1 5 8 9 1\n" +
                "2 1 7 7 10 2\n" +
                "2 1 9 3 11 5\n" +
                "2 1 11 10 12 6\n";
        MatrixResponse response = PreferencesManager.getMatrixResponse();
        response.setFieldType("ZpMersenne");
        response.setInternalIterationsNumber("1");
        response.setPartiesNumber("3");
        response.setOutputFile("output.txt");
        response.setPartyID("1");
        ProtocolActivity pa = new ProtocolActivity(context, assetManager, response,
                circuitContent, "8");
//        pa.doInBackground();
        assertEquals("8", pa.inputAnswer);
        assertEquals("1", pa.matrixResponse.getPartyID());
    }
}
