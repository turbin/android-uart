package com.turbineyan.devicedelegate;

public class UartJniHelper{

    static {
		System.loadLibrary("uartjnihelper");
	}

    public static native boolean devOpen(String devName, int baudRate, int checkMode);
    public static native boolean devClose();

    public static native int send(byte[] bytes, int timeoutS);
    public static native int recv(byte[] bytes, int timeoutS);
    public static native int lastError();
}