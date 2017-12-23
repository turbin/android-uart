#include <stdarg.h>
#include "logger.h"

char gBuf[4096+120]={0};

unsigned char *sendbuf = gBuf;// + 5;
unsigned int   g_CmdLen = 0;

static inline void __nop(){}

int sprintf_a_cmd_valist(ndk_cmd_t cmd, char * fmt, va_list args)
{
	char c = '\0';
    char ch = '\0';
    char *str = NULL;
    int d = 0;
    unsigned int u = 0;
    unsigned long long l = 0;
    unsigned char * pbuf = sendbuf;
   /// va_list args;
    int  i = 0;
	
    memset(sendbuf, 0x00, sizeof(char) * PACKET_RESV_LEN);
    //NDK_ERROR_DEBUG("%s,line%d:--------------start-----\n", __FUNCTION__, __LINE__);
    memcpy(pbuf, (unsigned char *)&cmd, sizeof (int));
    pbuf += sizeof (int);
    if (fmt != NULL) {
       /// va_start(args, fmt);
        while ((c = *fmt++)) {
            *pbuf ++ = c;
            switch (c) {
            case 'c':
                ch = va_arg(args, int);
                *pbuf ++ = ch;
                break;
            case 's':
                str = va_arg(args, char *);
				u = va_arg(args, unsigned int);
			
                memcpy(pbuf, (unsigned char *)&u, sizeof (unsigned int));
                pbuf += sizeof (unsigned int);
                memcpy(pbuf, str, u);
                pbuf += u;
                break;
            case 'd':
                d = va_arg(args, int);
                memcpy(pbuf, (unsigned char *)&d, sizeof (int));
                pbuf += sizeof (int);
                break;
            case 'u':
                u = va_arg(args, unsigned int);
                memcpy(pbuf, (unsigned char *)&u, sizeof (unsigned int));
                pbuf += sizeof (unsigned int);
                break;
            case 'l':
                l = va_arg(args, unsigned long long);
                memcpy(pbuf, (unsigned char *)&l, sizeof (unsigned long long));
                pbuf += sizeof (unsigned long long);
                break;
			case 'n':
			   break;
            default:
                goto out;
            }
        }
out:
	   /// va_end(args);
	   __nop();

    }
    *pbuf ++ = 'v';
    g_CmdLen = (pbuf - sendbuf);

	// NDK_ERROR_DEBUG( "sprintf_a_cmd_valist Data packed:\n");
	LOGGER_ERROR( "Data packed cmd=%s:\n", _ndkcmd_to_readableString(cmd)); //always on in recording the commander
    ///NDK_ERROR_DEBUG_BUF(sendbuf,g_CmdLen);
	
    return 0;
}

int sprintf_a_cmd(int cmd, char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	sprintf_a_cmd_valist(cmd, (char*)fmt, ap);
	va_end(ap);

}

