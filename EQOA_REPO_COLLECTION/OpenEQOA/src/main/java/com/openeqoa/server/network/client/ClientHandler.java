package com.openeqoa.server.network.client;

import com.openeqoa.server.network.udp.Write;
import lombok.Getter;
import lombok.Setter;

import java.io.*;
import java.net.Socket;

@Getter
public class ClientHandler {

    // TCP
    private Socket tcpSocket;
    private DataOutputStream dataOutputStream;
    private DataInputStream dataInputStream;

    // User Details
    @Setter
    private String username;


    public void initTCP(Socket tcpSocket, DataOutputStream dataOutputStream, DataInputStream dataInputStream) {
        this.tcpSocket = tcpSocket;
        this.dataOutputStream = dataOutputStream;
        this.dataInputStream = dataInputStream;
    }

    @FunctionalInterface
    private interface Reader {
        void accept() throws IOException;
    }

    public String readString() {
        try {
            return dataInputStream.readUTF();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    public int readInt() {
        readIn(() -> dataInputStream.readInt());
        return 0;
    }

    public int readChar() {
        readIn(() -> dataInputStream.readChar());
        return 0;
    }

    private void readIn(Reader reader) {
        try {
            reader.accept();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }


    public byte readByte() {
        try {
            return dataInputStream.readByte();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return 0x0;
    }

    /**
     * This is used to send the entity packet data.
     *
     * @param opcode        The code that defines what this packet contents will contain.
     * @param writeCallback The data we will be sending to the client.
     */
    public void write(byte opcode, Write writeCallback) {
//        try {
//            outputStream.writeByte(opcode);
//            writeCallback.accept(outputStream);
//            outputStream.flush();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
    }

    /**
     * Disconnects this client from the server.
     */
    public void closeConnection() {
        try {
            if (tcpSocket != null) tcpSocket.close();
            if (dataOutputStream != null) dataOutputStream.close();
            if (dataInputStream != null) dataInputStream.close();
            tcpSocket = null;
            dataOutputStream = null;
            dataInputStream = null;
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
