package com.soft.penguin.servicewithsocket;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.IBinder;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.soft.penguin.servicewithsocket.MyService.*;

public class MainActivity extends AppCompatActivity {

    private Button mStartServiceBtn;
    private Button mStopServiceBtn;
    private Button mClearLogsBtn;
    private Button mSendBtn;

    private TextView mLogsTxVw;
    private TextView mServiceStatusTxVw;
    private TextView mClientStatusTxVw;
    private EditText mEditTxtToSend;

    private ServiceConnection mServiceConnection;
    private boolean mIsServiceBound;

    private MyService mMyService;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mStartServiceBtn = (Button) findViewById(R.id.button_start_service);
        mStartServiceBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                startService(new Intent(getApplicationContext(), MyService.class));
                Toast.makeText(MainActivity.this, "Staring service", Toast.LENGTH_SHORT).show();
            }
        });

        mStopServiceBtn = (Button) findViewById(R.id.button_stop_service);
        mStopServiceBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                stopService(new Intent(getApplicationContext(), MyService.class));
                Toast.makeText(MainActivity.this, "Stopping service", Toast.LENGTH_SHORT).show();
            }
        });

        mClearLogsBtn = (Button) findViewById(R.id.button_clear_logs);
        mClearLogsBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mLogsTxVw.setText("");
            }
        });

        mSendBtn = (Button) findViewById(R.id.button_send);
        mSendBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String data = mEditTxtToSend.getText().toString();
                mMyService.writeData(data);
            }
        });

        mLogsTxVw = (TextView) findViewById(R.id.textView_logs);
        mServiceStatusTxVw = (TextView) findViewById(R.id.textView_service_status);
        mClientStatusTxVw = (TextView) findViewById(R.id.textView_client_status);
        mEditTxtToSend = (EditText) findViewById(R.id.editTxt_input);

        mServiceConnection = new ServiceConnection() {
            @Override
            public void onServiceConnected(ComponentName name, IBinder service) {
                LocalBinder binder = (LocalBinder) service;
                mMyService = binder.getService();
                mIsServiceBound = true;

                mMyService.setOnDataReceivedHandler(new ISocketCommunication() {
                    @Override
                    public void onDataReceived(final String data) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                if(data != null){
                                    String logs = data.replace("\\\n", System.getProperty("line.separator"));
                                    String old_logs;
                                    if(mLogsTxVw.getText() != null){
                                        old_logs = mLogsTxVw.getText().toString();
                                        logs = old_logs + System.getProperty("line.separator") + logs;
                                    }
                                    mLogsTxVw.setText(logs);
                                }
                            }
                        });
                    }

                    @Override
                    public void onClientConnected() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mClientStatusTxVw.setText("yes");
                            }
                        });
                    }

                    @Override
                    public void onClientDisconnected() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mClientStatusTxVw.setText("no");
                            }
                        });
                    }
                });

                mMyService.setServiceStatusHandler(new IServiceStatus() {
                    @Override
                    public void onStart() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mServiceStatusTxVw.setText("start");
                            }
                        });
                    }

                    @Override
                    public void onStop() {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mServiceStatusTxVw.setText("stop");
                            }
                        });
                    }
                });
            }

            @Override
            public void onServiceDisconnected(ComponentName name) {
                mIsServiceBound = false;
            }
        };

        Intent intent = new Intent(MainActivity.this, MyService.class);
        bindService(intent, mServiceConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        if(mIsServiceBound && mServiceConnection != null){
            unbindService(mServiceConnection);
        }
    }
}
