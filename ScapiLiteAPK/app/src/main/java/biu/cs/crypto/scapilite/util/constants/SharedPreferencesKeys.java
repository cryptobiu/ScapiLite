package biu.cs.crypto.scapilite.util.constants;

/**
 * Created by Blagojco on 16/03/2018- 16:01
 */

public class SharedPreferencesKeys
{
    private static String prefPackage = "com.inellipse.biumatrixsallary.";
    public static final String PREF_USER_APPLICATION_PREFERENCES = prefPackage + "preferences";

    public static final String TOKEN = prefPackage + "access_token";
    public static final String LATEST_POLL = prefPackage + "latestPollAnswer";
    public static final String POLL = prefPackage + "poll";
    public static final String USER = prefPackage + "fbuser";
    public static final String NEXT_POLL = prefPackage + "nextpoll";
    public static final String PRIVATE = prefPackage + "private";
    public static final String PRIVATE_PEM = prefPackage + "privatePem";
    public static final String PUBLIC = prefPackage + "public";
    public static final String PUBLIC_PEM = prefPackage + "publicPem";
    public static final String IP = prefPackage + "ip";
    public static final String MATRIX_RESPONSE = prefPackage + "matrixResponse";
    public static final String OFFLINE_INSTANCE_PER_POLL = prefPackage + "offlineInstance";
    public static final String CIRCUIT_STRING = prefPackage + "circuitString";
    public static final String DEVICE_MAC_ADDRESS = prefPackage + "macaddress";
}
