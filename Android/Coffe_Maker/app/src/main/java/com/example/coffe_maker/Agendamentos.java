package com.example.coffe_maker;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class Agendamentos extends AppCompatActivity {
    EditText age1, age2, age3;
    Button atualizar, del_age1, del_age2, del_age3;
    String data1, data2, data3;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_agendamentos);

        age1 = (EditText) findViewById(R.id. age1);
        age2 = (EditText) findViewById(R.id. age2);
        age3 = (EditText) findViewById(R.id. age3);
        atualizar = (Button) findViewById(R.id. atualizar);
        del_age1 = (Button) findViewById(R.id. del_age1);
        del_age2 = (Button) findViewById(R.id. del_age2);
        del_age3 = (Button) findViewById(R.id. del_age3);

        Bundle bundle = getIntent().getExtras();
        if (bundle != null)
        {
            data1 = bundle.getString("ag1p");
            data2 = bundle.getString("ag2p");
            data3 = bundle.getString("ag3p");

        }

        age1.setText(data1);
        age2.setText(data2);
        age3.setText(data3);


        atualizar.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180:8081/get_agendamentos";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Agendamentos.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });


        del_age1.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180:8081/del_age1";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Agendamentos.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

        del_age2.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180:8081/del_age2";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Agendamentos.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

        del_age3.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180:8081/del_age3";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Agendamentos.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });


    }

    private class SolicitaDados extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... url) {
            return Conexao.getDados(url[0]);
        }

        @Override
        protected void onPostExecute(String resultado) {


            if(resultado != null){

                String[] dados_recebidos = resultado.split(",");
                age1.setText(dados_recebidos[0]);
                age2.setText(dados_recebidos[1]);
                age3.setText(dados_recebidos[2]);

       //         txtResul2.setText(resultado);
//                if (resultado.contains("ag1vazio") ){
//                    age1.setText("Vazio");
//                }
//
//                if (resultado.contains("ag2vazio") ){
//                    age2.setText("Vazio");
//                }
//                if (resultado.contains("ag3vazio") ){
//                    age3.setText("Vazio");
//                }
            } else {
                Toast.makeText(Agendamentos.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
            }
        }
    }
}