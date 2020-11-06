package com.example.coffe_maker;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.view.View;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;

public class Preparando extends AppCompatActivity {

     CountDownTimer countDownTimer;
     long timeLeftInMilliseconds = 100000;
     long timeLeftInMilliseconds2 = 1;
     boolean timeRunnng;

    TextView textView3;
    String data1;
    ProgressBar progressBar;
    int data2 = 0, data3 = 0, i=0;
    TextView text_view_progress;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preparando);
        progressBar = (ProgressBar) findViewById(R.id.progressBar);
        textView3 = findViewById(R.id.textView3);


        Bundle bundle = getIntent().getExtras();
        if (bundle != null)
        {
            data1 = bundle.getString("quant");
            if(data1!=null)
                try {
                    //   timeLeftInMilliseconds = Integer.parseInt(data1);
                } catch(NumberFormatException e){
                    //Log.i();
                }


        }
       // timeLeftInMilliseconds = timeLeftInMilliseconds*1000;
        starStop();




        final Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                if (i<=100) {
                 //   textView3.setText("" + i);
                    progressBar.setProgress(i);
                    i++;
                    handler.postDelayed(this, 200);
                } else{
                    handler.removeCallbacks(this);
                }
            }
        }, 200);


    }
    public void starStop(){
        if (timeRunnng){
            stopTimer();
        }else {
            starTimer();
        }
    }

    public void starTimer() {
        countDownTimer = new CountDownTimer(timeLeftInMilliseconds, 1000) {
            @Override
            public void onTick(long timeLeftInMilliseconds2) {
                timeLeftInMilliseconds = 1;
                updateTimer();
            }
            @Override
            public void onFinish() {
            }
        }.start();
        timeRunnng =true;
    }


        public void stopTimer(){
            countDownTimer.cancel();
            timeRunnng = false;
        }

        public void updateTimer(){
        int minutes = (int) timeLeftInMilliseconds / 60000;
        int seconds = (int) timeLeftInMilliseconds % 60000 / 1000;

        String timeLeftText;

            timeLeftText = ""+ minutes;
            timeLeftText += ":";
            if (seconds < 10) timeLeftText += "0";
            timeLeftText += seconds;

            textView3.setText(timeLeftText);
        }
}

