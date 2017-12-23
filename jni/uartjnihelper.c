#include "uartjnihelper.h"
#include "logger.h"
#include "uart.h"

#include <string.h>

#define JNI_FALSE 0
#define JNI_TRUE 1

#ifdef USE_FACK_INTERFACE
static char gszBuffer[1024]={0};
static int  gnLength = 0;
#define MINI(A, B) ((A) > (B)?(B):(A))


#define _TRUE (1==1)
#define _FALSE (1==0)

static void __dump_buffer() {
    LOGGER_TRACE("gnLength=%d", gnLength);
    LOGGER_TRACE("gzBuffer=%s", gszBuffer);
}
static void __push_in(const char* dataIn, int length) {
    gnLength = MINI(sizeof(gszBuffer),length);
    memcpy(gszBuffer, dataIn, length);
}

static void __pop_up(char* dataOut, int *length) {
    *length = MINI(gnLength, *length);
    memcpy(dataOut, gszBuffer, *length);
    gnLength -= *length;
}

static int __fack_open(const char* szDevName, int baudRate,int parityMode){
    LOGGER_TRACE("dev name=%s",   szDevName);
    LOGGER_TRACE("parityMode=%d", parityMode);
    LOGGER_TRACE("baudRate=%d",   baudRate);
    return _TRUE;
}

static int __fack_close() {
    LOGGER_TRACE("@here! ");
    return _TRUE;
}

static int __fack_send(const char* buffer, int len, int nTimeout) {
    
    LOGGER_TRACE("nTimeout=%d", nTimeout);
    __push_in((const char*)buffer, len);
    __dump_buffer();
    return len;
}

static int __fack_recv(char* buffer, int len, int nTimeout) {
    LOGGER_DEBUG("nTimeout=%d", nTimeout);
    int nLength = len;
    __pop_up(buffer, &nLength);
    __dump_buffer();
    return nLength;
}

static int __fack_err()
{   
    LOGGER_TRACE("@here");
    return 0;
}

#endif

typedef struct DevOpe_Tag
{
    bool (*_open)(const char*, int ,int);
    bool (*_close)();
    int  (*_send)(const char* ,int , int);
    int  (*_recv)(char* ,int, int);
    int  (*_err)();
}DevOpe_T;

static inline DevOpe_T* __init_DevNode() {
    static DevOpe_T gpt = {
        __fack_open,
        __fack_close,
        __fack_send,
        __fack_recv,
        __fack_err
    };

    return &gpt;
}

static inline DevOpe_T* getDevOpt() {

    static DevOpe_T* pt=NULL;
    if(pt == NULL) pt = __init_DevNode();

    return pt;    
}

/*
 * Class:     com_turbineyan_devicedelegate_UartJniHelper
 * Method:    devOpen
 * Signature: (Ljava/lang/String;II)Z
 */
JNIEXPORT jboolean JNICALL Java_com_turbineyan_devicedelegate_UartJniHelper_devOpen(
            JNIEnv *env, jclass clzz, jstring devName, jint baudRate, jint parityMode
) 
{
    const char *szDevName = env->GetStringUTFChars(devName, 0);

    // use your string
    int ret = getDevOpt()->_open(szDevName, (int) baudRate, (int)parityMode)?JNI_TRUE:JNI_FALSE;

    env->ReleaseStringUTFChars(devName, szDevName);

    return (ret==_TRUE)?JNI_TRUE:JNI_FALSE;
 } 

/*
 * Class:     com_turbineyan_devicedelegate_UartJniHelper
 * Method:    devClose
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_turbineyan_devicedelegate_UartJniHelper_devClose(JNIEnv *, jclass) {
    
    return (getDevOpt()->_close()==_TRUE)?JNI_TRUE:JNI_FALSE;
}

/*
 * Class:     com_turbineyan_devicedelegate_UartJniHelper
 * Method:    send
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_turbineyan_devicedelegate_UartJniHelper_send(JNIEnv *env, jclass clzz, jbyteArray j_array, jint timeout) {

    //1. 获取数组长度  
    jint arr_len = env->GetArrayLength(j_array);  
    //2. 根据数组长度和数组元素的数据类型申请存放java数组元素的缓冲区  
    char* bytes = new char[arr_len];
    //3. 初始化缓冲区  
    memset(bytes,0, arr_len);

    //4. 拷贝Java数组中的所有元素到缓冲区中  
    env->GetByteArrayRegion(j_array,0,arr_len, (jbyte*)bytes);

    jint ret = getDevOpt()->_send(bytes, arr_len, timeout);

    delete [] bytes;

    return ret;
}

/*
 * Class:     com_turbineyan_devicedelegate_UartJniHelper
 * Method:    recv
 * Signature: ([BI)I
 */
JNIEXPORT jint JNICALL Java_com_turbineyan_devicedelegate_UartJniHelper_recv(JNIEnv *env, jclass clzz, jbyteArray j_array, jint timeout) {
        
    //1. 获取数组长度
    jint arr_len = env->GetArrayLength(j_array);
    LOGGER_TRACE("arr_len=%d", arr_len);
    
    if(!LOGGER_ASSERT_EXPRESSION(arr_len > 0)) {
        return -1;
    }

    char* bytes = new char[arr_len];
    
    memset(bytes,0, arr_len);

    jint ret = getDevOpt()->_recv(bytes, arr_len, timeout);

    if(!LOGGER_ASSERT_EXPRESSION(ret > 0)) {
        delete [] bytes;
        return ret;
    }

    //4. 拷贝Java数组中的所有元素到缓冲区中  
    env->SetByteArrayRegion(j_array,0, ret, (const jbyte* )bytes);
    delete [] bytes;
    return ret;
}

/*
 * Class:     com_turbineyan_devicedelegate_UartJniHelper
 * Method:    lastError
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_turbineyan_devicedelegate_UartJniHelper_lastError(JNIEnv *, jclass){

    return getDevOpt()->_err();
}











