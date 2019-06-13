package crypto.cs.biu.scapilite.ui.activity;

import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.webkit.JavascriptInterface;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;

import java.lang.reflect.Method;

import crypto.cs.biu.scapilite.R;
import crypto.cs.biu.scapilite.util.PreferencesManager;
import crypto.cs.biu.scapilite.util.constants.AppConstants;

import static crypto.cs.biu.scapilite.util.Logger.log;

public class WebViewActivity extends AppCompatActivity {

    private WebView webview;
    private String url;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_web_view);

        ActionBar actionbar = getSupportActionBar();
        //setup action bar
        if (actionbar != null) {
            actionbar.setDisplayShowHomeEnabled(true);
            actionbar.setDisplayHomeAsUpEnabled(true);
            actionbar.setIcon(R.mipmap.ic_launcher);
            actionbar.setDisplayShowTitleEnabled(false);
        }

        init();

        url = getIntent().getStringExtra(AppConstants.INTENT_WEB_URL);

        log("url " + url);
        if (url == null || url.equals("")) {

            webview.loadUrl("");
        } else {
            webview.loadUrl(url);
        }

        // Uncoment bellow to allow debugging
//		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
//			WebView.setWebContentsDebuggingEnabled(true);
//		}


        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            webview.getSettings().setMixedContentMode(WebSettings.MIXED_CONTENT_COMPATIBILITY_MODE);
        } else {
            try {
                Method m = WebSettings.class.getMethod("setMixedContentMode", int.class);
                if (m == null) {
                } else {
                    m.invoke(webview.getSettings(), 2); // 2 = MIXED_CONTENT_COMPATIBILITY_MODE
                }
            } catch (Exception ex) {
            }

        }

        log("webViewActivity " + url);

    }


    @JavascriptInterface
    private void init() {

        webview = (WebView) findViewById(R.id.webview);
        webview.addJavascriptInterface(new WebAppInterface(this), "Android");


        webview.getSettings().setJavaScriptEnabled(true);

        webview.setWebChromeClient(new WebChromeClient() {
            public void onProgressChanged(WebView view, int progress) {
                // Activities and WebViews measure progress with different scales.
                // The progress meter will automatically disappear when we reach 100%
                setProgress(progress * 1000);
            }
        });

//		webview.setWebViewClient(new WebViewClient()
//		{
//			public void onReceivedError(WebView view, int errorCode, String description, String failingUrl)
//			{
//			}
//		});

//
//        webview.setWebViewClient(new WebViewClient() {
//            @Override
//            public boolean shouldOverrideUrlLoading(WebView view, String url) {
//                view.loadUrl(url);
//                return true;
//
//            }
//        });


    }


    public class WebAppInterface {
        Context mContext;

        WebAppInterface(Context c) {
            mContext = c;
        }

        @JavascriptInterface
        public void successfullyPaid() {

        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_logout, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.logout:
                PreferencesManager.logoutUser();
                startActivity(new Intent(this, LoginActivity.class));

                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
