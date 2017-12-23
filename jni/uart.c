#include "uart.h"
#include "logger.h"

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <assert.h>
#include <time.h>
#include <termios.h>
#include <string.h>

int _getBaudRate(int nBaudRate)
{
	switch(nBaudRate) {
		case BAUD9600:
			return B9600;
	 	
		case BAUD19200:
			return B19200;
		
		case BAUD38400:
			return B38400;

		case BAUD57600:
			return B57600;

		case BAUD115200:
			return B1152000;
		
		case BAUD460800:
			return B460800;

		default:
			LOGGER_WARN_IT("not support baud rate=%d, we will set baud rate in default(B9600)!", nBaudRate);
			return B9600;
	}
}
static inline int getError()
{
	return errno;
}

static inline const char* getErrorMessage(){
	return strerror(errno);
}



static int __set_parity_mode(struct termios *options,int nMode)
{

	switch(nMode)
	{
		case MODE_8N1:
		{     ///No parity (8N1):
			options->c_cflag &= ~PARENB;
			options->c_cflag &= ~CSTOPB;
			options->c_cflag &= ~CSIZE;
			options->c_cflag |= CS8;
		}
		return 0;

		case MODE_7E1:
		{
			// Even parity (7E1):
			options->c_cflag |= PARENB;
			options->c_cflag &= ~PARODD;
			options->c_cflag &= ~CSTOPB;
			options->c_cflag &= ~CSIZE;
			options->c_cflag |= CS7;
		}
		return 0;


		case MODE_7O1:
		{
			///Odd parity (7O1):
			options->c_cflag |= PARENB;
			options->c_cflag |= PARODD;
			options->c_cflag &= ~CSTOPB;
			options->c_cflag &= ~CSIZE;
			options->c_cflag |= CS7;
		}
		return 0;
	/// Space parity is setup the same as no parity (7S1):

		case MODE_7S1:
		{
			options->c_cflag &= ~PARENB;
			options->c_cflag &= ~CSTOPB;
			options->c_cflag &= ~CSIZE;
			options->c_cflag |= CS8;
		}
		return 0;

		default:
			LOGGER_WARN_IT("invalid check mode =%d", nMode);
		return -1;
	}

}

static int __set_io_option(struct termios *options)
{
	/// for input option
	options->c_iflag |= BRKINT;
	//cfg->c_iflag &= ~IXON;
	options->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); //set as a raw channel
	options->c_iflag &= ~(IXON | IXOFF | IXANY); //disable flow control
	options->c_iflag &= ~IGNBRK;

	// for output option
	//cfg->c_iflag &= ~IXON;
	options->c_oflag &= ~OPOST;
}

static int __initCom(const char *devName, int *pnFd, int nBaudRate, int nMode)
{
	LOGGER_TRACE("@here");

	int ret = 0;
	struct termios myCfg = {0};
	struct termios *cfg=&myCfg;

	int fd = open(devName, O_RDWR | O_NOCTTY | O_NONBLOCK);
	LOGGER_TRACE("open %s fd=%d\n", devName, fd);

	if(fd < 0){
		LOGGER_TRACE("open dev failed=%s error=%s!\n", devName, getErrorMessage());
		return -1;
	}

	ret = tcgetattr(fd, cfg);
	if(ret < 0){
		LOGGER_TRACE("tcgetattr dev=%s error=%s! \n", devName,getErrorMessage());
		return -1;
	}


	cfmakeraw(cfg);
	//cfsetispeed(cfg,B460800);
	//cfsetospeed(cfg,B460800);

	cfsetispeed(cfg,_getBaudRate(nBaudRate));
	cfsetispeed(cfg,_getBaudRate(nBaudRate));

/*
	cfg->c_iflag |= BRKINT;
	//cfg->c_iflag &= ~IXON;
	cfg->c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG); //set as a raw channel
	cfg->c_iflag &= ~(IXON | IXOFF | IXANY); //disable flow control
	cfg->c_iflag &= ~IGNBRK;
*/
	__set_io_option(cfg);
/*
	cfg->c_cflag &= ~CSIZE;
	cfg->c_cflag &= ~PARENB;
	cfg->c_cflag &= ~CSTOPB;
	cfg->c_cflag |= CS8;
*/
	ret =__set_parity_mode(cfg, nMode);
	if(ret < 0) {
		return  -1;
	}
	
	ret = tcsetattr(fd,TCSANOW,cfg);
	
	if(ret < 0){
		LOGGER_TRACE("tcsetattr dev=%s error=%s! \n", devName,getErrorMessage());
		return -1;
	}
	
	ret = ioctl(fd, TCFLSH, 0);
	
	if(ret < 0){
		LOGGER_TRACE("ioctl TCFLSH dev=%s error=%s! \n", devName,getErrorMessage());
		return -1;
	}

	ret = ioctl(fd, TCFLSH, 1);

	if(ret < 0){
		LOGGER_TRACE("ioctl TCFLSH dev=%s error=%s! \n", devName,getErrorMessage());
		return -1;
	}

	if(pnFd) *pnFd = fd;
	return 0;
}


static int __transfer_send(int nUartFd, const char * data, int len)
{
	LOGGER_TRACE("@here");

	int maxfd = 0, sendcount = 0, ret = 0 ;
	fd_set sendset;
	struct timeval tvp = {0};
	int i = 0;

	if(nUartFd < 0){
		LOGGER_WARN_IT( "__transfer_send nUartFd error!");
		return -EBADFD;
	}

	maxfd = nUartFd + 1;
	sendcount = 0;
	tvp.tv_sec = 10;
	tvp.tv_usec = 0;

	while (sendcount < len) {
		FD_ZERO(&sendset);
		FD_SET(nUartFd, &sendset);

		ret = select(maxfd, NULL, &sendset, NULL, &tvp) ;

		if(ret <= 0) break;

		ret = write(nUartFd, (const void*)(data + sendcount), len - sendcount);

		if(ret <= 0) break;

		sendcount += ret;
	}

	return sendcount;
}


static int __transfer_recv(int nUartFd,const char *buf, int buflen,unsigned int timeout)
{

	LOGGER_TRACE("@here");
	
	int maxfd = 0, readcount = 0, ret = 0;
	fd_set readset ;
	struct timeval tvp = {0};
	struct timeval *vp = NULL;

	///memset(buf, 0x00, buflen);

	if(nUartFd < 0){
		LOGGER_WARN_IT( "__transfer_recv nUartFd error!");
		return -EBADFD;
	}

	maxfd = nUartFd + 1 ;
	readcount = 0 ;
	
	if(timeout == 0) {
		vp = NULL;
	}
	else {
		tvp.tv_sec = timeout / 1000;
		tvp.tv_usec = (timeout % 1000) * 1000 ;
		vp = &tvp;
	}

	while (readcount < buflen) {
		FD_ZERO(&readset) ;
		FD_SET(nUartFd, &readset);
		ret = select(maxfd, &readset, NULL, NULL, vp) ;
		if (ret <= 0){
			if (EINTR == errno)
				continue;
			break;
		}

		if(!FD_ISSET(nUartFd, &readset)){
			LOGGER_WARN_IT( "fd not be set, to be continue polling...");
			continue;
		}

		ret = read(nUartFd, (void *)(buf + readcount), buflen - readcount);
		if (ret <= 0) {
			break ;
		}

		readcount += ret ;
	}
	
	return (readcount > 0 ? readcount : ret);
}

#if 0
Uart::Uart(const std::string &devName,BAUDRATE_T rate, CHECK_MODE_T mode)
    :m_dev(devName),m_rate(rate),m_mode(mode)
{

}

Uart::Uart(const char* devName, BAUDRATE_T rate, CHECK_MODE_T mode)
	:m_dev(devName),m_rate(rate),m_mode(mode)
{

}

Uart::~Uart()
{
    if(isOpened()) devClose();
}

bool Uart::devOpen()
{
	m_errorCode = initCom(&m_dev[0], m_fd, m_rate, m_mode);
	return m_errorCode < 0?false:true;
}


bool Uart::devClose()
{
	if(::close(m_fd) == 0)return true;

	m_errorCode = getError();
	return false;
}


int Uart::send(const ByteStream& bytes, int nTimeout)
{
	return send((const char*)bytes.data(), (int)bytes.size(), nTimeout);
}

int Uart::recv(ByteStream& bytes, int nTimeout)
{
	return recv((char *)bytes.data(), (int)bytes.size(), nTimeout);
}

int Uart::send(const char* buffer, int nLengthOfBuf, int nTimeout)
{
	int ret = transfer_send(m_fd, buffer, nLengthOfBuf);
	if(ret < 0) m_errorCode = ret;

	return ret;
}

int Uart::recv(char* buffer, int nLengthOfBuf, int nTimeout)
{
	int ret = transfer_recv(m_fd, buffer, nLengthOfBuf, nTimeout);
	if(ret < 0) m_errorCode = ret;

	return ret;

}
#endif


static int gnFd = -1;

static int *__getFdRef(){return &gnFd;}

static int __getFd() {return gnFd;}

int uart_open(const char* szDevName, int baudRate,int parityMode)
{
	if(__getFd() != -1) close(__getFd());

	return (__initCom(szDevName, __getFdRef(), baudRate, parityMode)==0);
}

int uart_close()
{
	if(__getFd() != -1) close(__getFd());

	*__getFdRef() = -1;
	return (1==1);
}

int uart_send(const char* buffer, int len, int nTimeout)
{
	return __transfer_send(__getFd(),buffer, len);
}

int uart_recv(char* buffer, int len, int nTimeout)
{
	return __transfer_recv(__getFd(), buffer, len, nTimeout);
}


int uart_err()
{
	return getError();
}


