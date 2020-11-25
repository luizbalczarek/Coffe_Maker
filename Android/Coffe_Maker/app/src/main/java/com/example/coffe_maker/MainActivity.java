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
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import okhttp3.OkHttpClient;

public class MainActivity extends AppCompatActivity {

    ImageButton imageButton3;
//    TextView txtOi;
    EditText txtResul;



    public void irTelaPreparo(View view){
        Intent intent1 = new Intent(getApplicationContext(), Ajustes.class );
        startActivity(intent1);
    }



    public void irTelaCriarAgendamento(View view){
        Intent intent3 = new Intent(getApplicationContext(), Criar_agendamento.class );
        startActivity(intent3);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        imageButton3 = (ImageButton) findViewById(R.id.imageButton3);

        imageButton3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {


                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180:8081/get_agendamentos";

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
                 String[] dados_recebidos = resultado.split(",");
                 String age1="", age2="", age3="";
                 age1 = (dados_recebidos[0]);
                 age2 = (dados_recebidos[1]);
                 age3 = (dados_recebidos[2]);

                 Intent intent2 = new Intent(getApplicationContext(), Agendamentos.class );

                 Bundle bundle = new Bundle();
                 bundle.putString( "ag1p", age1);
                 bundle.putString( "ag2p", age2);
                 bundle.putString( "ag3p", age3);

                 intent2.putExtras(bundle);
                 startActivity(intent2);

             } else {
                 Toast.makeText(MainActivity.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
             }
         }
     }
}