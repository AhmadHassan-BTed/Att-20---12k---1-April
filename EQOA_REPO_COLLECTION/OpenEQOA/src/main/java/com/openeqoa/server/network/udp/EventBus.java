package com.openeqoa.server.network.udp;

import com.openeqoa.server.network.client.ClientHandler;
import lombok.AllArgsConstructor;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Map;

import static com.openeqoa.server.util.Log.println;

public class EventBus {

    /**
     * Contains opcodes and the associated class/method combinations to listen for.
     * <p>
     * Map:
     * Byte: Packet Opcode
     * CallbackData: Class and Method reference.
     */
    private final Map<Byte, CallbackData> packetListenerMap = new HashMap<>();

    @AllArgsConstructor
    private class CallbackData {

        /**
         * The PacketListener class associated with this callback data.
         */
        private PacketListener packetListener;

        /**
         * The method to invoke when a packet is received.
         */
        private Method method;
    }

    /**
     * Prepares the server to listen to a paticular packet.
     * Registers Opcodes found via annotations in the PacketListener class.
     *
     * @param packetListener The PacketListener we will listen for.
     */
    public void registerListener(PacketListener packetListener) {
        for (Method method : packetListener.getClass().getMethods()) {
            for (Opcode opcodeAnnotation : method.getAnnotationsByType(Opcode.class)) {
                Class<?>[] params = method.getParameterTypes();
                String error = "PacketListener: " + packetListener;
                if (params.length != 1)
                    throw new RuntimeException(error + " must have 1 parameters.");
                if (!params[0].equals(ClientHandler.class))
                    throw new RuntimeException(error + " first parameter must be of type ClientHandler");
                packetListenerMap.put(opcodeAnnotation.getOpcode(), new CallbackData(packetListener, method));
            }
        }
    }

    /**
     * When a packet is received we attempt to invoke methods that listen for its Opcode.
     *
     * @param opcode        Determines what class and methods to invoke.
     * @param clientHandler The client the packet was received from.
     */
    void publish(byte opcode, ClientHandler clientHandler) {
        CallbackData callbackData = packetListenerMap.get(opcode);
        if (callbackData == null) {
            println(getClass(), "Callback data was null for " + opcode + ". Is the event registered?");
            return;
        }
        try {
            // Opcode exists and PacketListener is registered, now lets invoke its methods.
            callbackData.method.invoke(callbackData.packetListener, clientHandler);
        } catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
            e.printStackTrace();
        }
    }
}
