package com.openeqoa.server.network.tcp;

import com.openeqoa.server.ServerMain;
import com.openeqoa.server.network.NetBuffer;
import com.openeqoa.server.network.ServerConstants;
import com.openeqoa.server.network.client.ClientHandler;
import com.openeqoa.server.network.client.ClientManager;
import com.openeqoa.server.util.NetworkUtils;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import static com.openeqoa.server.util.Log.println;

public class TCPConnection {

    private ServerSocket serverSocket;

    /**
     * Opens a TCP server on a given socket.
     */
    public void openServer() {
        try {
            // Creates a socket to allow for communication between clients and the server.
            serverSocket = new ServerSocket(ServerConstants.LOGIN_SERVER_PORT);
        } catch (IOException e) {
            e.printStackTrace();
        }

        println(getClass(), "TCP Server opened on port: " + serverSocket.getLocalPort());
        listenForConnections();
    }

    /**
     * Listen for client connections and if possible establish
     * a link between the client and the server.
     */
    private void listenForConnections() {
        println(getClass(), "Listening for client connections...");

        // Creating a thread that runs as long as the server is alive and listens
        // for incoming connections
        new Thread(() -> {
            while (ServerMain.getInstance().isRunning()) {
                try {

                    // Listeners for a new client socket to connect to the server and
                    // throws the socket to a receive packets method to be handled
                    receivePackets(serverSocket.accept());

                } catch (IOException e) {

                    if (e instanceof SocketException && !ServerMain.getInstance().isRunning()) {
                        break;
                    }

                    e.printStackTrace();
                    // End application here
                }
            }
        }, "TCPConnectionListener").start();
    }

    /**
     * Start receiving packets for a new client connection.
     *
     * @param clientSocket A new client connection.
     */
    private void receivePackets(Socket clientSocket) {
        ClientManager clientManager = ServerMain.getInstance().getClientManager();
        println(getClass(), "Attempting to establish an incoming TCP connection...");

        // This thread listens for incoming packets from the socket passed
        // to the method
        new Thread(() -> {
            ClientHandler clientHandler = null;
            // Using a new implementation in java that handles closing the streams
            // upon initialization. These streams are for sending and receiving data
            try (
                    DataOutputStream outStream = new DataOutputStream(clientSocket.getOutputStream());
                    DataInputStream inStream = new DataInputStream(clientSocket.getInputStream())
            ) {

                // Creating a new client handle that contains the necessary components for
                // sending and receiving data
                clientHandler = new ClientHandler();
                clientHandler.initTCP(clientSocket, outStream, inStream);

                String ipAddress = clientSocket.getInetAddress().getHostAddress();

                println(getClass(), "Client IP " + ipAddress + " has logged in.");

                // Adding the client handle to a list of current client handles
                clientManager.addClient(ipAddress, clientHandler);

                // Process incoming login packet
                authenticateUser(clientHandler);
                loginResponsePacketOut(clientHandler);

            } catch (IOException e) {

                if (e instanceof EOFException || e instanceof SocketException) {
                    // The user has logged out of the server
                    if (clientHandler != null && ServerMain.getInstance().isRunning()) {

                        // The client has disconnected
                        String ipAddress = clientSocket.getInetAddress().getHostAddress();
                        println(getClass(), "Client IP " + ipAddress + " has logged out.");
                        clientManager.removeClient(ipAddress);
                        println(getClass(), "Clients Online: " + clientManager.getClientsOnline());
                    }
                } else {
                    e.printStackTrace();
                }

            } finally {
                // Closing the client socket for cleanup
                if (clientSocket != null) {
                    try {
                        clientSocket.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            } // Starting a new thread for the client in the format of address:port for the client and then starting the tread
        }, clientSocket.getInetAddress().getHostAddress() + ":" + clientSocket.getPort()).start();
    }

    private boolean authenticateUser(ClientHandler clientHandler) {

        byte[] buffer = null;
        try {
            DataInputStream stream = clientHandler.getDataInputStream();
            int bufferSize = stream.readInt();
            buffer = new byte[bufferSize - 4];
            stream.readFully(buffer);

        } catch (IOException e) {
            e.printStackTrace();
        }

        NetBuffer netBuffer = new NetBuffer(buffer);
        int messageType = netBuffer.readInt();
        netBuffer.readInt(); // read and skip
        netBuffer.readInt(); // read and skip
        String username = netBuffer.readASCIINullTerminate(32);

        // Add client details to the client handler
        clientHandler.setUsername(username);

        // TODO: read password

        // TODO: test username and password combination against a user database.

        // TODO: Return true if auth was success, false otherwise.
        return true; // Auto returning true since no form of authentication has been established.
    }

    private void loginResponsePacketOut(ClientHandler clientHandler) {

        // HEX network response found in the corsten server code.
        // Comments on string supplied by "EQOA Revival Packet Analysis" by Ben Turi.
        String loginResponse = "000000A8" + // Buffer length
                "00000025" + // Message Type?
                "00" + // Padding
                "00000000" + // User ID
                "0000" + // Result
                "0000" + // ACC STAT
                "0000000000000000" + // SubTime
                "0000000000000000" + // ParTime
                "00000000000000000000000000000000" +
                "0000000000000000000000000000000000000000000000000000" +
                "000000000026C15D5A00000003001E7F4F001E7F4F716C4432" +
                "36636878726B53314A74677600000000000000000000000000" +
                "00000000000000000000000000000000000000000000000000" +
                "000000000000000000" +
                "00" + // pad
                "00000001" + // unknown
                "00000003"; // unknown

        try {
            DataOutputStream outStream = clientHandler.getDataOutputStream();

            // Convert the String of hex characters into bytes.
            byte[] loginBytes = NetworkUtils.hexStringToByteArray(loginResponse);

            // Loop through each byte and send it to the game client.
            for (byte b : loginBytes) {
                outStream.writeByte(b);
            }

            outStream.flush();

            // Once the login response has been sent, it appears TCP is no longer needed.
            // So we will close the connection. From here on out, UDP will be used.
            // If a bug is ever found where the client is trying to rely on TCP,
            // we can remove this statement below.
            clientHandler.closeConnection();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Closing down the server's socket which means no more request from the clients may
     * be handled
     */
    private void closeTCP() {
        try {
            if (serverSocket != null) serverSocket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
