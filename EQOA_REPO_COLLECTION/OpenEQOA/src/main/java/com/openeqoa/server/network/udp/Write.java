package com.openeqoa.server.network.udp;

import java.io.ObjectOutputStream;

@FunctionalInterface
public interface Write {
    void accept(ObjectOutputStream outStream);
}
