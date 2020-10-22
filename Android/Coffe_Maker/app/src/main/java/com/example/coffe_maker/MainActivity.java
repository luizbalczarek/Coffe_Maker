package com.example.coffe_maker;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import okhttp3.OkHttpClient;

public class MainActivity extends AppCompatActivity {

    Button btnLed1;
    TextView txtOi;
    EditText txtResul;

    public void irTelaPreparo(View view){
        Intent intent1 = new Intent(getApplicationContext(), Ajustes.class );
        startActivity(intent1);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        btnLed1 = (Button) findViewById(R.id.btnLed1);

        txtOi = (TextView) findViewById(R.id.txtOi);

        txtResul = (EditText) findViewById(R.id. txtResul);

        btnLed1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/";

                if(networkInfo != null && networkInfo.isConnected()){

                     new SolicitaDados().execute(url);


                }else {
                    Toast.makeText(MainActivity.this, "Nenhuma conexão foi detectada", Toast.LENGTH_LONG).show();
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

                txtResul.setText(resultado);
             } else {
                 Toast.makeText(MainActivity.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
             }
         }
     }
}