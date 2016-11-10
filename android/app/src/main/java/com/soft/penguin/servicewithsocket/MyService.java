package com.soft.penguin.servicewithsocket;

import android.app.Service;
import android.content.Intent;
import android.net.LocalServerSocket;
import android.net.LocalSocket;
import android.os.Binder;
import android.os.IBinder;
import android.util.Log;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

public class MyService extends Service {
    private String TAG = "SERVICE!!!!";

    private final IBinder mBinder = new LocalBinder();
    private LocalServerSocket mServerSocket;
    private LocalSocket mAcceptedSocket;

    private ISocketCommunication mOnDataReceived;
    private IServiceStatus mServiceStatusCallback;

    private InputStream mInput;
    private OutputStream mOutput;

    public static String SOCKET_ADDRESS = "com.soft.penguin.localServerSock";

    public class LocalBinder extends Binder {
        public MyService getService() {
            // Return this instance of service so clients can call public methods
            return MyService.this;
        }
    }

    public MyService() {
        Log.d(TAG, "C'tor");
    }

    @Override
    public IBinder onBind(Intent intent) {
        Log.d(TAG, "onBind");
        return mBinder;
    }

    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand");

        if(mServiceStatusCallback != null) {
            mServiceStatusCallback.onStart();
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                boolean isEnd = false;

                while(true) {
                    try {
                        // Listens for a connection to be made to this socket and accepts
                        // it. The method blocks until a connection is made.
                        // So accept socket in own thread
                        mServerSocket = new LocalServerSocket(SOCKET_ADDRESS);
                        Log.d(TAG, "Waiting for clients");
                        mAcceptedSocket = mServerSocket.accept();
                        Log.d(TAG, "Accepted client socket");

                        if (mOnDataReceived != null) {
                            mOnDataReceived.onClientConnected();
                        }

                        mInput = mAcceptedSocket.getInputStream();
                        mOutput = mAcceptedSocket.getOutputStream();

                        while (!isEnd) {
                            byte[] buffer = new byte[2048];

                            int count = mInput.read(buffer);
                            String data = new String(buffer);
                            data = data.trim();

                            if (count < 0) {
                                Log.d(TAG, "Client disconnected");
                                isEnd = true;
                                if (mOnDataReceived != null) {
                                    mOnDataReceived.onClientDisconnected();
                                }
                                continue;
                            }

                            Log.d(TAG, "Data received: [" + count + "]: " + data);
                            if (mOnDataReceived != null) {
                                mOnDataReceived.onDataReceived(data);
                            }
                        }
                        isEnd = false;
                    } catch (IOException e) {
                        e.printStackTrace();

                        if (mOnDataReceived != null) {
                            mOnDataReceived.onClientDisconnected();
                        }
                    } finally {
                        try {
                            isEnd = false;
                            mAcceptedSocket.close();
                            mServerSocket.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            }
        }).start();

        Log.d(TAG, "Finished");
        return Service.START_NOT_STICKY;
    }

    public void writeData(String data){
        try {
            byte[] data_to_send =  data.getBytes();
            mOutput.write(data_to_send);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void setOnDataReceivedHandler(ISocketCommunication handler){
        this.mOnDataReceived = handler;
    }

    public void setServiceStatusHandler(IServiceStatus handler){
        this.mServiceStatusCallback = handler;
    }

    @Override
    public void onDestroy(){
        if(mServiceStatusCallback != null) {
            mServiceStatusCallback.onStop();
        }

        super.onDestroy();
    }
}
