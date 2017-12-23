package com.turbineyan.devicedelegate;

import java.util.stream.Stream;

import com.turbineyan.devicedelegate.UartConfig;
import com.turbineyan.devicedelegate.UartJniHelper;

public class Uart {
    private UartConfig mConfig;
    private String     mDeviceName;

	/**
	 *@brief 		创建一个设备实例.
	 *@details
     *@param    config - uart 参数配置对象方法
	 *@return   Uart 对象
	*/
    public static Uart createInstance(UartConfig config){
        return new Uart("/dev/tty0", config); // asume the Uart device mount on /dev/tty0
    }
    
	/**
	 *@brief 		创建一个设备实例.
	 *@details
     *@param    config - uart 参数配置对象方法
	 *@return   Uart 对象
    */
    
    Uart(String devName, UartConfig config){
        mDeviceName = devName;
        mConfig     = config;
    }

    public int open(){
        if(!UartJniHelper.devOpen(mDeviceName, mConfig.BaudRate, mConfig.CheckMode))
            return UartJniHelper.lastError();

        return 0;
    }

    public int close(){
         if(!UartJniHelper.devClose()) return UartJniHelper.lastError();
         return 0;
    }
    
    public int send(byte[] bytes, int nTimeoutS){
        return UartJniHelper.send(bytes, timeoutS);
    }

    public int recv(byte[] bytes, int nTimeoutS){
        return UartJniHelper.recv(bytes, timeoutS);
    }
}
