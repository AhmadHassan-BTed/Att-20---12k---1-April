package com.openeqoa.server.network.udp;


import com.openeqoa.server.network.client.ClientHandler;

import java.io.ObjectOutputStream;

public abstract class PacketOut {

    /**
     * Opcode to send with the out-going packet.
     */
    protected final byte opcode;

    /**
     * The client handler that holds the object output stream
     */
    private final ClientHandler clientHandler;

    public PacketOut(byte opcode, ClientHandler clientHandler) {
        this.opcode = opcode;
        this.clientHandler = clientHandler;
    }

    /**
     * Sends the packet to the client.
     */
    public void sendPacket() {
        clientHandler.write(opcode, this::createPacket);
    }

    /**
     * Creates the packet.
     */
    protected abstract void createPacket(ObjectOutputStream write);
}
