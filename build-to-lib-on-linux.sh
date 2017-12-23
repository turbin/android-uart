#g++ -o main uart.cpp -std=c++11
#g++ -std=c++11 -DIN_LINUX -o libuart.so -shared -fPIC uart.cpp
#g++ -std=c++11 -DIN_LINUX -o libuartwrapper.so -shared -fPIC uart.cpp uartWrapper.cpp

g++ -std=c++11 -DIN_LINUX -DUSE_FACK_INTERFACE -o  $PWD/jni/libuartjnihelper.so -shared -fPIC $PWD/jni/uart.cpp $PWD/jni/uartjnihelper.cpp -I $JAVA_HOME/include -I $JAVA_HOME/include/linux