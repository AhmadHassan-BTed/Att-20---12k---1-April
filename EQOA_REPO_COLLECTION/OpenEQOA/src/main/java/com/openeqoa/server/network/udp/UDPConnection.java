package com.openeqoa.server.network.udp;

import com.openeqoa.server.ServerMain;
import com.openeqoa.server.network.ServerConstants;
import com.openeqoa.server.util.NetworkUtils;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.util.function.Consumer;

import static com.openeqoa.server.util.Log.println;

public class UDPConnection {

    private final static boolean PRINT_DEBUG = true;

    private final EventBus eventBus = new EventBus();
    private DatagramSocket serverSocket;

    /**
     * Opens a server on a given socket and registers event listeners.
     *
     * @param registerListeners Listeners to listen to.
     */
    public void openServer(Consumer<EventBus> registerListeners) {

        // Creates a socket to allow for communication between clients and the server.
        try {
            serverSocket = new DatagramSocket(ServerConstants.GAME_SERVER_PORT);
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }

        println(getClass(), "UDP Server opened on port: " + serverSocket.getLocalPort());
        registerListeners.accept(eventBus);
        listenForPackets();
    }

    /**
     * Listen for incoming client packets.
     */
    private void listenForPackets() {
        println(getClass(), "Listening for incoming packets...");

        byte[] buffer = new byte[576];

        new Thread(() -> {
            while (ServerMain.getInstance().isRunning()) {
                DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

                try {
                    serverSocket.receive(packet);
                    processPacket(packet, buffer);
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }, "UDPConnectionListener").start();
    }

    /**
     * Here we process the packet and break it down before it reaches the even bus.
     *
     * @param packet The incoming packet from the byte buffer.
     * @param buffer Packet data including extra buffer width.
     */
    @SuppressWarnings("ConstantConditions")
    private void processPacket(DatagramPacket packet, byte[] buffer) {
        int lmt = buffer.length;

        byte[] sourceId = new byte[0];
        byte[] destinationId = new byte[0];
        byte[] bc = new byte[0];
        byte[] bpl = new byte[0];
        byte[] sessionId = new byte[0];
        byte[] bundleType = new byte[0];
        byte[] bundleNumber = new byte[0];
        byte[] messageType = new byte[0];
        byte[] messageLength = new byte[0];
        byte[] opcode = new byte[0];

        // Process packet header
        if (lmt > 1) sourceId = NetworkUtils.subArray(buffer, 0, 1);
        if (lmt > 3) destinationId = NetworkUtils.subArray(buffer, 2, 3);
        if (lmt > 4) bc = NetworkUtils.subArray(buffer, 4, 4);
        if (lmt > 5) bpl = NetworkUtils.subArray(buffer, 5, 5);
        if (lmt > 9) sessionId = NetworkUtils.subArray(buffer, 6, 9);
        if (lmt > 10) bundleType = NetworkUtils.subArray(buffer, 10, 10);
        if (lmt > 12) bundleNumber = NetworkUtils.subArray(buffer, 11, 12);
        if (lmt > 14) messageType = NetworkUtils.subArray(buffer, 13, 14);
        if (lmt > 16) messageLength = NetworkUtils.subArray(buffer, 15, 16);
        if (lmt > 18) opcode = NetworkUtils.subArray(buffer, 17, 18);

        // User the CRC calculator to get the correct CRC
        // TODO: byte[] crc = CRC.calculateCRC();

        // TODO: Send data to the packet event bus for processing...
//        eventBus.publish(opcode, ServerMain.getInstance().getClientManager().getClient(packet.getAddress().getHostAddress()));

        if (!PRINT_DEBUG) return;
        println(getClass(), "------------------------------------------------------");
        println(getClass(), "Packet IP: " + packet.getAddress().toString());
        println(getClass(), "UDP Defined Packet Length: " + packet.getLength());

        printByteArray("SourceID", sourceId);
        printByteArray("DestinationID", destinationId);
        printByteArray("bc", bc);
        printByteArray("bpl", bpl);
        printByteArray("SessionId", sessionId);
        printByteArray("BundleType", bundleType);
        printByteArray("BundleNumber", bundleNumber);
        printByteArray("MessageType", messageType);
        printByteArray("MessageLength", messageLength);
        printByteArray("OpCode", opcode);

        // Only print out the actual length of the packet and not the buffer...
        println(getClass(), "Full Length Packet:");
        for (int i = 0; i < packet.getLength(); i++) {
            System.out.print(NetworkUtils.byteToHex(buffer[i]) + " ");
            buffer[i] = 0;
        }
        println(PRINT_DEBUG);
    }

    private void printByteArray(String message, byte[] array) {
        for (byte b : array) println(getClass(), message + ": " + NetworkUtils.byteToHex(b));
    }
}
