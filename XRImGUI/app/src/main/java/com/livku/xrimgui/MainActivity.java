package com.livku.xrimgui;

import androidx.appcompat.app.AppCompatActivity;

import android.app.NativeActivity;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.opengl.GLSurfaceView;
import android.widget.TextView;
import android.view.Surface;
import android.view.SurfaceHolder;

import com.livku.xrimgui.databinding.ActivityMainBinding;

public class MainActivity extends NativeActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
    }
}