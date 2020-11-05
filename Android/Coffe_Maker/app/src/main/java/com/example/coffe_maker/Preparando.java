package com.example.coffe_maker;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.EditText;
import android.widget.TextView;

public class Preparando extends AppCompatActivity {

    TextView textView3;
    String data1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preparando);


        textView3  = findViewById(R.id.textView3);

        Bundle bundle = getIntent().getExtras();
        if (bundle != null)
        {
            data1 = bundle.getString("quant");

        }

        textView3.setText(data1);


    }
}