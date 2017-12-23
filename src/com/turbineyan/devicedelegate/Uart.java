package com.turbineyan.devicedelegate;

import java.util.stream.Stream;

import com.turbineyan.devicedelegate.UartConfig;
import com.turbineyan.devicedelegate.UartJniHelper;

public class Uart {
    public UartConfig mConfig;
    public String     mDeviceName;


	/**
	 *@brief 		创建一个设备实例.
	 *@details
     *@param    config - uart 参数配置对象方法
	 *@return   Uart 对象
	*/
    public static Uart createInstance(UartConfig config){
        return new Uart("/dev/ttys0",config); // asume the Uart device mount on /dev/tty0
    }

    public static Uart createInstance(String devName, UartConfig config) {
        return new Uart(devName, config);
    }
    


	/**
	 *@brief 		创建一个设备实例.
	 *@details
     *@param    config - uart 参数配置对象方法
	 *@return   Uart 对象
    */
    Uart(String devName, UartConfig config) {
        this.mDeviceName = devName;
        this.mConfig = config;    
    }

    public int open() {
        if(!UartJniHelper.devOpen(this.mDeviceName, this.mConfig.mBaudRate, this.mConfig.mCheckMode))
            return UartJniHelper.lastError();

        return 0;
    }

    public int close() {
         if(!UartJniHelper.devClose()) return UartJniHelper.lastError();
         return 0;
    }
    
    public int send(byte[] bytes, int nTimeoutS){
        return UartJniHelper.send(bytes, nTimeoutS);
    }

    public int recv(byte[] bytes, int nTimeoutS){
        return UartJniHelper.recv(bytes, nTimeoutS);
    }
}
