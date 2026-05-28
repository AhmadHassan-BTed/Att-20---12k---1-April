package com.openeqoa.server.network.udp.out;

import com.openeqoa.server.network.client.ClientHandler;
import com.openeqoa.server.network.udp.Opcodes;
import com.openeqoa.server.network.udp.PacketOut;

import java.io.ObjectOutputStream;

import static com.openeqoa.server.util.Log.println;

public class PingOut extends PacketOut {

    public PingOut(ClientHandler clientHandler) {
        super(Opcodes.PING, clientHandler);
    }

    @Override
    protected void createPacket(ObjectOutputStream write) {
        println(getClass(), "Ping!");
    }
}
