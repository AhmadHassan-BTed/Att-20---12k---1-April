package com.openeqoa.server.network.udp.in;

import com.openeqoa.server.network.client.ClientHandler;
import com.openeqoa.server.network.udp.Opcode;
import com.openeqoa.server.network.udp.Opcodes;
import com.openeqoa.server.network.udp.PacketListener;
import com.openeqoa.server.network.udp.out.PingOut;

import static com.openeqoa.server.util.Log.println;

public class PingIn implements PacketListener {

    @Opcode(getOpcode = Opcodes.PING)
    public void onIncomingPing(ClientHandler clientHandler) {
        println(getClass(), "Pong!");
        new PingOut(clientHandler).sendPacket();
    }
}
