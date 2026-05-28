package com.openeqoa.server.network;

public class NetBuffer {

    private int pos = 0;
    private byte[] data;

    public NetBuffer(byte[] data) {
        this.data = data;
    }

    public short readShort() {
        short read = (short) (((0xFF & data[pos]) << 8) | (0xFF & data[pos + 1]));
        pos += 2;
        return read;
    }

    public int readInt() {
        int read = ((0xFF & data[pos]) << 24) | ((0xFF & data[pos + 1]) << 16) |
                ((0xFF & data[pos + 2]) << 8) | (0xFF & data[pos + 3]);
        pos += 4;
        return read;
    }

    public String readASCIINullTerminate(int len) {
        StringBuilder read = new StringBuilder();
        for (int i = 0; i < len; i++) {
            char ch = (char) data[pos + i];
            if (ch == '\0') {
                break;
            }
            read.append(ch);
        }
        pos += len;
        return read.toString();
    }

    public void readInformation(int len) {
        for (int i = pos; i < pos + len; i++) {

            byte b = data[i];
            int unsignedB = b & 0xFF;
            char ch = (char) b;

            System.out.println("[" + i + "] => {byte=" + b + ", unsigned byte=" + unsignedB + ", char=" + ch + "}");
        }
    }
}

