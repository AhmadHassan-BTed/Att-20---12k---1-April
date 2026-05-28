package com.openeqoa.server.util;

import java.util.Arrays;

public class NetworkUtils {

    /**
     * Converts a byte to a String.
     *
     * @param b The byte to convert.
     * @return A String representation of the byte value.
     */
    public static String byteToHex(byte b) {
        return String.format("%02x", b);
    }

    /**
     * Converts a Hex value to an Array of Bytes.
     *
     * @param string The String to convert to bytes.
     * @return A byte array representation of the hex string.
     */
    public static byte[] hexStringToByteArray(String string) {
        int length = string.length();
        byte[] data = new byte[length / 2];
        for (int i = 0; i < length; i += 2) {
            data[i / 2] = (byte) ((Character.digit(string.charAt(i), 16) << 4)
                    + Character.digit(string.charAt(i + 1), 16));
        }
        return data;
    }

    public static void reverseByteArray(byte[] array) {
        if (array == null) {
            return;
        }
        int i = 0;
        int j = array.length - 1;
        byte tmp;
        while (j > i) {
            tmp = array[j];
            array[j] = array[i];
            array[i] = tmp;
            j--;
            i++;
        }
    }

    public static byte[] subArray(byte[] array, int beginning, int end) {
        return Arrays.copyOfRange(array, beginning, end + 1);
    }

}
