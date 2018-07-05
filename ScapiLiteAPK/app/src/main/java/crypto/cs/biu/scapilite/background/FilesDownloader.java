//package crypto.cs.biu.scapilite.background;
//
//import android.content.Context;
//import android.net.Uri;
//import android.os.AsyncTask;
//
//import java.io.File;
//import java.io.FileOutputStream;
//import java.io.IOException;
//import java.net.HttpURLConnection;
//import java.net.MalformedURLException;
//import java.net.URL;
//
//import static crypto.cs.biu.scapilite.util.Logger.log;
//import static crypto.cs.biu.scapilite.util.Logger.logError;
//
//
///**
// * Created by Blagojco on 7/14/2015- 11:10
// */
//public class FilesDownloader extends AsyncTask<String, Void, Boolean> {
//    public static String EPG_LOCATION = null;
//    public static boolean DOWNLOADING_NOW = false;
//    private Context context;
//    private long epgVersion;
//
//    public FilesDownloader(Context context, long epgVersion) {
//        this.context = context;
//        this.epgVersion = epgVersion;
//    }
//
//
//    @Override
//    protected Boolean doInBackground(String... params) {
//        return downloadFile(params[0]);
//    }
//
//    private Boolean downloadFile(String sUrl) {
//
//        try {
//            String fileName = Uri.parse(sUrl).getLastPathSegment();
//            final File outpuFile = new File(context.getCacheDir(), fileName);
//            URL url = new URL(sUrl);
//            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
//            conn.setRequestMethod("GET");
//
//            if (conn.getResponseCode() == 200) {
//                FileOutputStream fos = new FileOutputStream(outpuFile);
//
//                int count = 0;
//                byte[] buffer = new byte[1024];
//                while ((count = conn.getInputStream().read(buffer)) > 0) {
//                    fos.write(buffer, 0, count);
//                    fos.flush();
//                }
//                fos.close();
//                importDatabase(Uri.parse(outpuFile.getAbsolutePath()));
//
//                return true;
//            } else {
//                log("ProgramDatabase conn.getResponseCode()  " + conn.getResponseCode());
//
//            }
//
//        } catch (MalformedURLException e) {
//            logError("doInBackground ProgramDatabase MalformedURLException  ", e);
//
//        } catch (IOException e) {
//            logError("doInBackground ProgramDatabase IOException  ", e);
//
//
//        }
//
//        return false;
//    }
//
//
//    public void importDatabase(Uri path) {
//        try {
//            log("ProgramDatabaseDownloader ProgramDatabase importDatabase " + path);
//            if (path != null) {
//                File newDbRoot = new File("/data/data/" + context.getPackageName() + "/databases");
//                log("ProgramDatabaseDownloader newDbRoot importDatabase " + newDbRoot);
//
////                File newDb = new File("/data/data/" + context.getPackageName() + "/databases/" + AppConfig.DATABASE_NAME);
////                log("ProgramDatabaseDownloader newDb importDatabase " + newDb);
////
////                File downloadedDB = new File(path.toString().replace("file://", ""));
////
////                log("ProgramDatabaseDownloader downloadedDB getPath " + downloadedDB.getPath());
////                log("ProgramDatabaseDownloader downloadedDB exists " + downloadedDB.exists());
////
////                if (downloadedDB.exists()) {
////                    if (!newDbRoot.exists()) {
////                        log("ProgramDatabaseDownloader newDbRoot mkdir ");
////                        newDbRoot.mkdir();
////                    }
////
////                    if (newDb.exists()) {
////                        log("ProgramDatabaseDownloader newDb delete ");
////                        newDb.delete();
////                    }
////
//////				newDb.createNewFile();
//////                    copyFile(downloadedDB, new FileInputStream(downloadedDB), new FileOutputStream(newDb), epgVersion);
////                    DOWNLOADING_NOW = false;
////
////                }
//            }
//        } catch (Exception e) {
//            //Mint.logException(e);
//            logError("importDatabase error ", e);
//            DOWNLOADING_NOW = false;
//
//        }
//    }
//
////    public void copyFile(File downloadedDB, FileInputStream fromFile, FileOutputStream toFile, long epgVersion) throws IOException {
////        FileChannel fromChannel = null;
////        FileChannel toChannel = null;
////        try {
////            fromChannel = fromFile.getChannel();
////            toChannel = toFile.getChannel();
////            fromChannel.transferTo(0, fromChannel.size(), toChannel);
////            PreferencesManager.putEpgVersion(epgVersion);
////        } catch (Exception e) {
////            //Mint.logException(e);
////            logError("ProgramDatabase copyFile error 1 ", e);
////            PreferencesManager.putEpgVersion(0);
////
////        } finally {
////            try {
////                if (fromChannel != null) {
////                    fromChannel.close();
////                }
////                PreferencesManager.putEpgVersion(epgVersion);
////            } catch (Exception e) {
////                //Mint.logException(e);
////                logError("ProgramDatabase copyFile error 1 ", e);
////                PreferencesManager.putEpgVersion(0);
////
////            } finally {
////                if (toChannel != null) {
////                    toChannel.close();
////                }
////            }
////
////            log("copyFile ProgramDatabaseDownloader done ");
////
////            log("ProgramDatabaseDownloader copyFile fromChannel " + fromChannel);
////
////            log("ProgramDatabaseDownloader ProgramDatabase downloadedDB.delete " + downloadedDB.delete());
////            log("ProgramDatabaseDownloader downloadedDB getParent " + Uri.fromFile(downloadedDB));
////            context.sendBroadcast(new Intent(Intent.ACTION_MEDIA_SCANNER_SCAN_FILE, Uri.fromFile(downloadedDB)));
////        }
////    }
//
//
//}
