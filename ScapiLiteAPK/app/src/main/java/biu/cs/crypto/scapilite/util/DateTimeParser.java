package biu.cs.crypto.scapilite.util;


import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

public class DateTimeParser {
    public static String parseStartTimeHHmmCET(long date) {
        SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm");
        dateFormat.setTimeZone(TimeZone.getTimeZone("CET"));
        return dateFormat.format(new Date(date));

    }

    public static String parseStartTimeddMMyyyy(long date) {
        return new SimpleDateFormat("dd.MM.yyyy").format(new Date(date));
    }

    public static String parseStartTimeEEEMMMdd(long date) {
        return new SimpleDateFormat("EEE, MMM dd").format(new Date(date));
    }

    public static String parseStartTimeEEEEMMMMdd(long date) {
        return new SimpleDateFormat("EEEE, MMMM dd").format(new Date(date));
    }

    public static String parseStartTimeMMdd(long date) {
        return new SimpleDateFormat("MM/dd").format(new Date(date));
    }

    public static String parseStartTimeHHmmss(long date) {
        return new SimpleDateFormat("HH:mm:ss").format(new Date(date));
    }

    public static String parseStartTimeHHmm(long date) {
        return new SimpleDateFormat("HH:mm").format(new Date(date));
    }

    public static String parseStartTimeHHmmAMPM(long date) {
        return new SimpleDateFormat("hh:mm a").format(new Date(date));
    }

    public static String parseStartTimeyyyyMMddHHmmssAMPM(long date) {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm:ss a").format(new Date(date));

    }

    public static String parseStartTimeyyyyMMddHHmmAMPM(long date) {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm a").format(new Date(date));
    }

    public static String parseStartTimeyyyyMMddHHmm(long date) {
        return new SimpleDateFormat("yyyy-MM-dd HH:mm").format(new Date(date));
    }

    public static String parseStartTimeyyyyMMdd(long date) {
        return new SimpleDateFormat("yyyy/MM/dd").format(new Date(date));

    }


    public static Date parseStringToDate(String stringDate) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        Date date = null;
        try {
            date = sdf.parse(stringDate);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return date;
    }

    public static Date parseStringToDateForURL(String stringDate) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd_hh:mm:ss");
        Date date = null;
        try {
            date = sdf.parse(new Date(Long.valueOf(stringDate)).toString());
        } catch (Exception e) {
            e.printStackTrace();
        }
        return date;
    }

    public static long parseFacebookDateToMiliseconds(String facebookDate) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssZ");
        Date date = null;
        try {
            date = sdf.parse(facebookDate);
            return date.getTime();
        } catch (ParseException e) {
            return new Date().getTime();
        }

    }

    public static Date parseFacebookDate(String facebookDate) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd");
        Date date = null;
        try {
            date = sdf.parse(facebookDate);


        } catch (ParseException e) {
            e.printStackTrace();
        }

        if (date != null) {
            return date;
        } else {
            return null;
        }
    }


    public static long hours(int hours) {
        // 1 hour = 3600000 ms
        return hours * 3600000;
    }

    public static long minutes(int minutes) {
        // 1 minute = 60000 ms
        return minutes * 60000;
    }

    public static long milisecondsFromHours(Long recordingArchiveHours) {
        return recordingArchiveHours * 60 * 60 * 1000;
    }

    public static String parseDuration(long seconds) {
        long hr = seconds / 3600;
        long rem = seconds % 3600;
        long mn = rem / 60;
        long sec = rem % 60;
        String hrStr = "";
        String mnStr = "";
        String secStr = "";
        if (hr > 0) {
            hrStr = (hr < 10 ? "0" : "") + hr;
        }

        if (mn > 0) {
            mnStr = (mn < 10 ? "0" : "") + mn;
        }

        secStr = (sec < 10 ? "0" : "") + sec;

        return hrStr + mnStr + secStr;
    }
}
