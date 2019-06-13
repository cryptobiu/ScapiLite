package crypto.cs.biu.scapilite.util;

import android.content.Context;
import android.content.DialogInterface;
import android.support.v7.app.AlertDialog;

import crypto.cs.biu.scapilite.R;

import static crypto.cs.biu.scapilite.util.Logger.logError;


public class Alerts {
    public static AlertDialog showAlert(Context context, String title, String message, String buttonText) {
        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
        try {
            alertDialog.setTitle(title);
            alertDialog.setMessage(message);
            alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, buttonText, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            alertDialog.show();
        } catch (Exception e) {
            logError("showAlert ", e);
        }
        return alertDialog;


    }


    public static AlertDialog showAreYouSureDialog(Context context, String title, String message, String negativeTitle, String positiveTitle) {
        String negative = negativeTitle == null ? context.getString(R.string.no) : negativeTitle;
        String positive = positiveTitle == null ? context.getString(R.string.yes) : positiveTitle;

        AlertDialog alertDialog = new AlertDialog.Builder(context).create();
        try {
            alertDialog.setTitle(title);
            alertDialog.setMessage(message);
            alertDialog.setButton(AlertDialog.BUTTON_NEGATIVE, negative, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });

            alertDialog.setButton(AlertDialog.BUTTON_POSITIVE, positive, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            });
            alertDialog.show();
        } catch (Exception e) {
            logError("showAlert ", e);
        }
        return alertDialog;
    }


}
