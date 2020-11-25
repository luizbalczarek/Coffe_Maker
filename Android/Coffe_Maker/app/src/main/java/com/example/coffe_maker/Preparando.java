package com.example.coffe_maker;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.os.Handler;
import android.view.View;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import java.security.cert.CertPathBuilder;
import java.util.Random;
import java.util.concurrent.TimeUnit;

public class Preparando extends AppCompatActivity {


    private static final String CHANNEL_ID = "33";
    TextView textView3;
    String data1;
    ProgressBar progressBar;
    int progress=100;

    int seconds , minutes,secs, tempo ;


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
                       secs = Integer.parseInt(data1);
                       tempo = secs;
                } catch(NumberFormatException e){
                }
        }


        
        new MyCountDown(10000000, 1000);

        }

    public class MyCountDown extends CountDownTimer
    {

        long duration, interval;
        public MyCountDown(long millisInFuture, long countDownInterval) {
            super(millisInFuture, countDownInterval);
            duration = millisInFuture;
            interval = countDownInterval;
            start();
        }

        @Override
        public void onFinish() {
        //    secs = 10;
        }

        @Override
        public void onTick(long duration) {
            textView3.setText(String.valueOf(secs));
            secs = secs - 1;
            minutes = secs / 60;
            seconds = secs % 60;
            textView3.setText(String.format("%02d", minutes)
                    + ":" + String.format("%02d", seconds));

            if (secs <= 0 ) {
                cancel();
                Toast.makeText(Preparando.this, "Café Pronto", Toast.LENGTH_LONG).show();
                addNotification();
                Intent intent4 = new Intent(Preparando.this, MainActivity.class );
                startActivity(intent4);
            }

            progress = ((secs*100)/tempo);
            progressBar.setProgress(progress);
        }
    }

    private void addNotification() {
        // Builds your notification
        NotificationCompat.Builder builder = new NotificationCompat.Builder(this)
                .setSmallIcon(R.mipmap.ic_launcher_round)
                .setContentTitle("Cafèolette")
                .setContentText("Seu café está Pronto!")
                .setAutoCancel(true)
                .setDefaults(Notification.DEFAULT_ALL);

        // Creates the intent needed to show the notification
        Intent notificationIntent = new Intent(this, MainActivity.class);
        PendingIntent contentIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);
        builder.setContentIntent(contentIntent);

        // Add as notification
        NotificationManager manager = (NotificationManager) getSystemService(Context.NOTIFICATION_SERVICE);
        manager.notify(0, builder.build());
    }
}

