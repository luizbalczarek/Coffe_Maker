package com.example.coffe_maker;

import androidx.appcompat.app.AppCompatActivity;

import android.app.TimePickerDialog;
import android.os.Bundle;
import android.text.InputType;
import android.view.View;
import android.widget.EditText;
import android.widget.TimePicker;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Criar_agendamento extends AppCompatActivity {

    EditText editTextTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_criar_agendamento2);

        editTextTime = findViewById(R.id.editTextTime);

        editTextTime.setInputType(InputType.TYPE_NULL);

        editTextTime.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showTimeDialog(editTextTime);
            }
        });
    }

    private void showTimeDialog(final EditText editTextTime) {
        final Calendar relogio=Calendar.getInstance();

        TimePickerDialog.OnTimeSetListener timeSetListener=new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int hourOfDay, int minute) {
                relogio.set(Calendar.HOUR_OF_DAY,hourOfDay);
                relogio.set(Calendar.MINUTE,minute);
                SimpleDateFormat simpleDateFormat=new SimpleDateFormat("HH:mm");
                editTextTime.setText(simpleDateFormat.format(relogio.getTime()));
            }
        };

        new TimePickerDialog(Criar_agendamento.this,timeSetListener,relogio.get(Calendar.HOUR_OF_DAY),relogio.get(Calendar.MINUTE),true).show();
    }
}