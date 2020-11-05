package com.example.coffe_maker;

import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.Navigation;

import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class Ajustes extends AppCompatActivity {
    Button button2, button3, button4, button5;
    Button button10;
    String quantidade = "0" ;


//    public void irTelaPreparando(View view){
//        String quantidade = "" ;
//        Intent intent4 = new Intent(Ajustes.this, Preparando.class );
//        Bundle bundle = new Bundle();
//        bundle.putString( "quant", quantidade);
//        intent4.putExtras(bundle);
//        startActivity(intent4);
//    }


    public void irTelaPreparando(View view){
        Intent intent4 = new Intent(Ajustes.this, Preparando.class );
    //    Bundle bundle = new Bundle();
    //    bundle.putString( "quant", quantidade);
    //    intent4.putExtras(bundle);
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



        button2.setOnClickListener(new View.OnClickListener() {
            @RequiresApi(api = Build.VERSION_CODES.M)
            @Override
            public void onClick(View view) {

                ConnectivityManager conMgr = (ConnectivityManager)
                        getSystemService(Context.CONNECTIVITY_SERVICE);
                NetworkInfo networkInfo = conMgr.getActiveNetworkInfo();
                String url= "http://192.168.0.180/make_200";

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
                String url= "http://192.168.0.180/make_400";

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
                String url= "http://192.168.0.180/make_600";

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
                String url= "http://192.168.0.180/make_800";

                if(networkInfo != null && networkInfo.isConnected()){
                     new SolicitaDados().execute(url);

                    //if (quantidade == 800) {
                    //    button5.setText("OK");
                        //  Navigation.findNavController(view).navigate(R.id.preparando);
                  //  }

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

           // if(resultado != null){
                if(resultado.contains("OK,800")) {
                    //    txtResul2.setText(resultado);

                    Intent intent5 = new Intent(Ajustes.this, Preparando.class );
                    Bundle bundle = new Bundle();
                    bundle.putString( "quant", "800");
                    intent5.putExtras(bundle);
                    startActivity(intent5);
                    //startActivity(new Intent(Ajustes.this, Preparando.class));


            } else if(resultado.contains("OK,600")){
                //    txtResul2.setText(resultado);
                    Intent intent5 = new Intent(Ajustes.this, Preparando.class );
                    Bundle bundle = new Bundle();
                    bundle.putString( "quant", "600");
                    intent5.putExtras(bundle);
                    startActivity(intent5);

            } else if(resultado.contains("OK,400")){
                    Intent intent5 = new Intent(Ajustes.this, Preparando.class );
                    Bundle bundle = new Bundle();
                    bundle.putString( "quant", "400");
                    intent5.putExtras(bundle);
                    startActivity(intent5);

                }else if(resultado.contains("OK,200")){
                    Intent intent5 = new Intent(Ajustes.this, Preparando.class );
                    Bundle bundle = new Bundle();
                    bundle.putString( "quant", "200");
                    intent5.putExtras(bundle);
                    startActivity(intent5);



                }else {
                Toast.makeText(Ajustes.this, "Ocorreu um erro", Toast.LENGTH_LONG).show();
             //   Navigation.findNavController(pre)
            }
        }
    }


}


