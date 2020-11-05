package com.example.coffe_maker;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.app.TimePickerDialog;
import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.text.InputType;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TimePicker;
import android.widget.Toast;

import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Criar_agendamento extends AppCompatActivity {

    EditText editTextTime;
    Button button6, button7, button8, button9;
    String horaA = "NO", quantidades = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_criar_agendamento2);
        button6 = (Button) findViewById(R.id.button6);
        button7 = (Button) findViewById(R.id.button7);
        button8 = (Button) findViewById(R.id.button8);
        button9 = (Button) findViewById(R.id.button9);


        editTextTime = findViewById(R.id.editTextTime);
        editTextTime.setInputType(InputType.TYPE_NULL);
        Spinner mySpinner = (findViewById(R.id.spinner));
        ArrayAdapter<String> myAdapter = new ArrayAdapter<>(Criar_agendamento.this,
                android.R.layout.simple_list_item_1, getResources().getStringArray(R.array.quantidades));

        myAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mySpinner.setAdapter(myAdapter);


        mySpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if (position == 0) {
                    quantidades = ":200";
                }
                else if (position == 1) {
                    quantidades = ":400";
                }
                else if (position == 2) {
                    quantidades = ":600";
                }
                else if (position == 3) {
                    quantidades = ":800";
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });

        editTextTime.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                showTimeDialog(editTextTime);
            }
        });

        button6.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {
                if (horaA == "NO") {
                    Toast.makeText(Criar_agendamento.this, "Selecione a hora", Toast.LENGTH_LONG).show();
                } else {

                    ConnectivityManager conMgr = (ConnectivityManager)
                            getSystemService(Context.CONNECTIVITY_SERVICE);
                    NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                    String url = "http://192.168.0.180/age_diasda:";
                    url = url + horaA;
                    url = url + quantidades;

                    if (networkInfo != null && networkInfo.isConnected()) {
                        new SolicitaDados().execute(url);
                    } else {
                        Toast.makeText(Criar_agendamento.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                    }
                }
            }
        });


        button7.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {
                if (horaA == "NO") {
                    Toast.makeText(Criar_agendamento.this, "Selecione a hora", Toast.LENGTH_LONG).show();
                } else {


                    ConnectivityManager conMgr = (ConnectivityManager)
                            getSystemService(Context.CONNECTIVITY_SERVICE);
                    NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                    String url = "http://192.168.0.180/age_sabado:";
                    url = url + horaA;
                    url = url + quantidades;

                    if (networkInfo != null && networkInfo.isConnected()) {
                        new SolicitaDados().execute(url);
                    } else {
                        Toast.makeText(Criar_agendamento.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                    }
                }
            }
        });



        button8.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {
                if (horaA == "NO") {
                    Toast.makeText(Criar_agendamento.this, "Selecione a hora", Toast.LENGTH_LONG).show();
                } else {

                    ConnectivityManager conMgr = (ConnectivityManager)
                            getSystemService(Context.CONNECTIVITY_SERVICE);
                    NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                    String url = "http://192.168.0.180/age_doming:";
                    url = url + horaA;
                    url = url + quantidades;
                    if (networkInfo != null && networkInfo.isConnected()) {
                        new SolicitaDados().execute(url);
                    } else {
                        Toast.makeText(Criar_agendamento.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                    }
                }
            }
        });


        button9.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {
                if (horaA == "NO") {
                    Toast.makeText(Criar_agendamento.this, "Selecione a hora", Toast.LENGTH_LONG).show();
                } else {

                    ConnectivityManager conMgr = (ConnectivityManager)
                            getSystemService(Context.CONNECTIVITY_SERVICE);
                    NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                    String url = "http://192.168.0.180/age_finais:";
                    url = url + horaA;
                    url = url + quantidades;

                    if (networkInfo != null && networkInfo.isConnected()) {
                        new SolicitaDados().execute(url);
                    } else {
                        Toast.makeText(Criar_agendamento.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                    }
                }
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
                horaA = simpleDateFormat.format(relogio.getTime());
            }
        };

        new TimePickerDialog(Criar_agendamento.this,timeSetListener,relogio.get(Calendar.HOUR_OF_DAY),relogio.get(Calendar.MINUTE),true).show();
    }


    private class SolicitaDados extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... url) {
            return Conexao.getDados(url[0]);
        }

        @Override
        protected void onPostExecute(String resultado) {

            if(resultado.contains("Cheio")){
                Toast.makeText(Criar_agendamento.this, "Agenda Cheia", Toast.LENGTH_LONG).show();

            } else if (resultado != null ){
                Toast.makeText(Criar_agendamento.this, "Agendamento Criado", Toast.LENGTH_LONG).show();

            }

            else {
                Toast.makeText(Criar_agendamento.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
            }
        }
    }


}





