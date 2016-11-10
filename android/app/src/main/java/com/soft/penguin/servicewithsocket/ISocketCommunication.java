package com.soft.penguin.servicewithsocket;

/**
 * Created by oleg on 11/9/16.
 */

public interface ISocketCommunication {
    void onDataReceived(String data);
    void onClientConnected();
    void onClientDisconnected();
}
