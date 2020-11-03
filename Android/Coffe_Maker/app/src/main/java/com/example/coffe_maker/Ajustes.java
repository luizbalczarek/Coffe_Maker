package com.example.coffe_maker;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class Ajustes extends AppCompatActivity {
    Button button2, button3, button4, button5;
    Button button10;
    static EditText txtResul2;
    String ret800 = "OK,800";
    int quantidade = 0;


    public void irTelaPreparando(View view){
        Intent intent4 = new Intent(getApplicationContext(), Preparando.class );
        startActivity(intent4);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_ajustes);
        button2 = (Button) findViewById(R.id.button2);
        button3 = (Button) findViewById(R.id.button3);
        button4 = (Button) findViewById(R.id.button4);
        button5 = (Button) findViewById(R.id.button5);
        button10 = (Button) findViewById(R.id.button5);
        txtResul2 = (EditText) findViewById(R.id. txtResul2);


        button2.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/200";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Ajustes.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

        button3.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/400";

                if(networkInfo != null && networkInfo.isConnected()){

                    new SolicitaDados().execute(url);


                }else {
                    Toast.makeText(Ajustes.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

        button4.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/600";

                if(networkInfo != null && networkInfo.isConnected()){
                    new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Ajustes.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

        button5.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/800";

                if(networkInfo != null && networkInfo.isConnected()){
                     new SolicitaDados().execute(url);
                }else {
                    Toast.makeText(Ajustes.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
                }
            }
        });

    }

    class SolicitaDados extends AsyncTask<String, Void, String> {

        @Override
        protected String doInBackground(String... url) {
            return Conexao.getDados(url[0]);
        }

        @Override
        protected void onPostExecute(String resultado) {

            if(resultado != null){
                txtResul2.setText(resultado);
 //               ret800 =resultado;
 //                if (ret800 == "OK,800" ){
//                     int quantidade = 800;
 //               }
            } else {
                Toast.makeText(Ajustes.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
            }
        }
    }


}


