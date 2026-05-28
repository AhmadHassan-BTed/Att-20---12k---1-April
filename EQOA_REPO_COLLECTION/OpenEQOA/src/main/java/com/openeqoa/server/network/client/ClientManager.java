package com.openeqoa.server.network.client;

import java.util.HashMap;
import java.util.Map;

public class ClientManager {

    /**
     * Map: IpAddress -> ClientHandler
     */
    private final Map<String, ClientHandler> clientMap = new HashMap<>();

    public void addClient(String ipAddress, ClientHandler clientHandler) {
        clientMap.put(ipAddress, clientHandler);
    }

    public void removeClient(String ipAddress) {
        clientMap.remove(ipAddress);
    }

    public ClientHandler getClient(String ipAddress) {
        return clientMap.get(ipAddress);
    }

    public int getClientsOnline() {
        return clientMap.size();
    }
}
