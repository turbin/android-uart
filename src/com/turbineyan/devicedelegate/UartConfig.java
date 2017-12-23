package com.turbineyan.devicedelegate;

public class UartConfig{
    
    public int mBaudRate;
    public int mCheckMode;
    
    public enum BaudRate {
        BAUD9600(0), BAUD19200(1), BAUD38400(2), BAUD57600(3),BAUD115200(4);
        private final int value;

        private BaudRate(int value) {
            this.value = value;
        }
    
        public int getValue() {
            return value;
        }
    }


    public enum ParityMode {
        MODE_7E1(0), // 7bits with 1bit for stopping and checking in even
        MODE_7O1(1), // 7bits with 1bit for stopping and checking in odd
        MODE_8N1(2); // 8bits with stopping bit and checking mode

        private final int value;
        
        private ParityMode(int value) {
            this.value = value;
        }
    
        public int getValue() {
            return value;
        }
    }

    /*
        config = "{B19200, NONE}"
    */
    public UartConfig(int baudRate, int checkMode){
            this.mBaudRate  = baudRate;
            this.mCheckMode = checkMode;
    }

    public Boolean parse() {
        return false;
    }
}