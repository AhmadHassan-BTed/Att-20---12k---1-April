package com.openeqoa.server;

import com.openeqoa.server.game.GameLoop;
import com.openeqoa.server.network.client.ClientManager;
import com.openeqoa.server.network.tcp.TCPConnection;
import com.openeqoa.server.network.udp.UDPConnection;
import com.openeqoa.server.network.udp.in.PingIn;
import lombok.Getter;
import lombok.Setter;

import static com.openeqoa.server.util.Log.println;

@Getter
public
class ServerMain {

    private static ServerMain instance = null;

    private final ClientManager clientManager = new ClientManager();
    private final TCPConnection tcpConnection = new TCPConnection();
    private final UDPConnection udpConnection = new UDPConnection();
    private final GameLoop gameLoop = new GameLoop();

    /**
     * Used to handle closing down all threads associated with
     * the server. Volatile allows the variable to exist
     * between threads.
     */
    @Setter
    private volatile boolean running = true;

    public static void main(String[] args) {
        ServerMain.getInstance().startServer();
    }

    /**
     * Gets the main instance of this class.
     *
     * @return A singleton instance of this class.
     */
    public static ServerMain getInstance() {
        if (instance == null) instance = new ServerMain();
        return instance;
    }

    /**
     * Initializes the game server.
     */
    private void startServer() {
        println(getClass(), "Initializing network...");
        tcpConnection.openServer();
        udpConnection.openServer((eventBus) -> {
            // Register UDP packet listeners here.
            eventBus.registerListener(new PingIn());
        });
        gameLoop.start();
    }
}
