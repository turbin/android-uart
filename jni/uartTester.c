#include <stdlib.h>
#include <stdio.h>
#include "uart.h"
#include "logger.h"
#include <errno.h>

#include <time.h>

//extern int sdtpInit0();
extern int NDk_SysGetK21Version(char *version);
extern int NDK_InitCom( int (*send)(unsigned char * data, int len,unsigned int respondTimeoutMs),
                        int (*recv)(unsigned char * buf, int buflen, unsigned int timeout), char * info );

extern int sdtpSend(uint8_t * data, int datalen, unsigned int respondTimeoutMs);
extern int sdtpRecv(uint8_t * buff, int bufflen,unsigned int timeout);
extern int sdtpInit(int (*send)(unsigned char * data, int len),
                    int (*recv)(unsigned char * buf, int buflen,unsigned int timeoutMs), char * info);

static int wrapper_send(unsigned char * data, int len)
{
    int ret =uart_send(data, len, -1);
    printf("@here! wrapper_send=%d \n", ret);
    return ret;
}

static int wrapper_recv(unsigned char * buf, int buflen,unsigned int timeoutMs)
{
    int ret= uart_recv(buf, buflen, timeoutMs);
    printf("@here! wrapper_send=%d \n", ret);
    return ret;
}

int main(int argc, char* argv[]) {

    if(!LOGGER_IS_EXPECT_RET(uart_open("/dev/ttyHSL1", BAUD460800, MODE_8N1), (1==1)))
    {
        printf("uart_open error=%s \n",strerror(uart_err()));
        return -1;
    }

    if(!LOGGER_IS_EXPECT_RET(sdtpInit(wrapper_send, wrapper_recv, NULL), 0)) {
        printf("sdtpInit error=%s \n", strerror(uart_err()));
        return -1;
    }

    if(!LOGGER_IS_EXPECT_RET(NDK_InitCom(sdtpSend, sdtpRecv, NULL), 0)) {
        printf("sdtpInit error=%s \n", strerror(uart_err()));
        return -1;
    }

    char version[2000]={0};

    if(!LOGGER_IS_EXPECT_RET(NDk_SysGetK21Version(&version), 0)){
        printf("NDk_SysGetK21Version error! \n");
    }
    else{
        printf("k21 version=%s \n", version);
    }
    
    uart_close();
    return 0;
}



