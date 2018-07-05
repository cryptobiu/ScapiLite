//package crypto.cs.biu.scapilite.ui.activity;
//
//import android.content.Intent;
//import android.os.Bundle;
//import android.support.v7.app.AppCompatActivity;
//import android.view.View;
//
//import crypto.cs.biu.scapilite.MainActivity;
//import crypto.cs.biu.scapilite.R;
//import crypto.cs.biu.scapilite.model.User;
//import crypto.cs.biu.scapilite.util.PreferencesManager;
//
///**
// * Created by Blagojco on 11/04/2018- 11:05
// */
//
//public class LoginQuestionsActivity extends AppCompatActivity {
//
//    private User user;
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState) {
//        super.onCreate(savedInstanceState);
//
//        setContentView(R.layout.activity_login_questions);
//        getSupportActionBar().setDisplayShowHomeEnabled(true);
//        getSupportActionBar().setIcon(R.mipmap.ic_launcher);
//
//        init();
//
//    }
//
//    private void init() {
//        user = PreferencesManager.getUser();
//    }
//
//    public void registerUser(View view) {
//        //TODO register user
//        startActivity(new Intent(this, MainActivity.class));
//    }
//
//}