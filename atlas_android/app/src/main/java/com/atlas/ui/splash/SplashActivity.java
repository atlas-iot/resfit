package com.atlas.ui.splash;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.ViewGroup.LayoutParams;
import android.widget.TextView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.atlas.BuildConfig;
import com.atlas.R;
import com.atlas.ui.main.MainActivity;
import com.atlas.utils.AtlasConstants;

public class SplashActivity extends AppCompatActivity {
    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        TextView versionText = new TextView(this);
        versionText.setText("Version " + BuildConfig.VERSION_NAME);
        versionText.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        versionText.setGravity(Gravity.CENTER | Gravity.BOTTOM);
        versionText.setTextSize(TypedValue.COMPLEX_UNIT_SP, 18);
        versionText.setTextColor(getColor(R.color.colorAccent));
        versionText.setPadding(0, 0, 0, 50);
        setContentView(versionText);

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent mainIntent = new Intent(SplashActivity.this, MainActivity.class);
                SplashActivity.this.startActivity(mainIntent);
                SplashActivity.this.finish();
            }
        }, AtlasConstants.ATLAS_SPLASH_DISPLAY_TIME);
    }
}
