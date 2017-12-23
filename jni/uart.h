#ifndef UART_H_
#define UART_H_


#if 0
#include <string>
#include <vector>

typedef std::vector<char> ByteStream;

class Uart
{
    public:
        typedef enum{
            BAUD9600,
            BAUD19200,
            BAUD38400,
            BAUD57600,
            BAUD115200
        } BAUDRATE_T;

        typedef enum{
            MODE_7E1, // 7bits with 1bit for stopping and checking in even
            MODE_7O1, // 7bits with 1bit for stopping and checking in odd
            MODE_8N1,  // 8bits with stopping bit and checking mode
            MODE_7S1   // Space parity is setup the same as no parity (7S1):
        }CHECK_MODE_T; // parity check mode

        Uart(const std::string &devName,BAUDRATE_T rate, CHECK_MODE_T mode);
        Uart(const char* devName, BAUDRATE_T rate, CHECK_MODE_T mode);
        ~Uart();

        bool devOpen();
        bool devClose();
        inline bool isOpened(){return m_isOpend;}

        inline int  lastError(){return m_errorCode;}

        int send(const ByteStream& bytes, int nTimeout);
        int recv(ByteStream& bytes, int nTimeout);

        int send(const char* buffer, int nLengthOfBuf, int nTimeout);
        int recv(char* buffer, int nLengthOfBuf, int nTimeout);
    private:
        std::string m_dev;
        int m_rate;
        int m_mode;

        int  m_errorCode;
        int  m_fd;
        bool m_isOpend;
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
typedef enum{
    BAUD9600,
    BAUD19200,
    BAUD38400,
    BAUD57600,
    BAUD115200,

    BAUD460800=12
} BAUDRATE_T;

typedef enum{
    MODE_7E1, // 7bits with 1bit for stopping and checking in even
    MODE_7O1, // 7bits with 1bit for stopping and checking in odd
    MODE_8N1,  // 8bits with stopping bit and checking mode
    MODE_7S1   // Space parity is setup the same as no parity (7S1):
}CHECK_MODE_T; // parity check mode


int uart_open(const char* szDevName, int baudRate,int parityMode);
int uart_close();
int uart_send(const char* buffer, int len, int nTimeout);
int uart_recv(char* buffer, int len, int nTimeout);
int uart_err();

#ifdef __cplusplus
}
#endif

#endif

