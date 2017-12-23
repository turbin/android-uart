#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>

#define IN_ANDROID

#if defined(IN_ANDROID)
	#include <android/log.h>
	enum
	{
		LOG_DEBUG=ANDROID_LOG_DEBUG,
		LOG_WARN=ANDROID_LOG_WARN,
		LOG_ERROR=ANDROID_LOG_ERROR
	};

	#define __LOG(LEVEL, TAG, ...)	__android_log_print(LEVEL, TAG, __VA_ARGS__)
#elif defined(IN_LINUX)
	//#include <stdio.h>
	enum
	{
		LOG_DEBUG=0,
		LOG_WARN,
		LOG_ERROR
	};
	#define __LOG(LEVEL, TAG, ...) do{printf(__VA_ARGS__);printf("\n");}while(0)
#else
	#error "please specify the platform"
#endif

#include <stdarg.h>


#define LOG_NDK_TAG "UartLib"

#define LOGGER_DEBUG(...)  __LOG(LOG_DEBUG, LOG_NDK_TAG,__VA_ARGS__)
#define LOGGER_WARN(...)   __LOG(LOG_WARN,  LOG_NDK_TAG,__VA_ARGS__)
#define LOGGER_ERROR(...)  __LOG(LOG_ERROR, LOG_NDK_TAG,__VA_ARGS__)

static inline void __record(const char* file, int line, const char* function, int function_ret, int expect_ret) 
{
	LOGGER_ERROR("@FILE=%s,line=%ld, %s ret=%d,but expect:%d",file,line,function,function_ret,expect_ret);
}

static inline int _is_expect_ret(int function_ret, 
	int expect_ret,
	const char*file,
	const char*function,
	long line)
	{		
		return (function_ret==expect_ret)
				 ?(1==1)
				 :(__record(file,line,function,function_ret,expect_ret),(1==0));
	}

#define LOGGER_IS_EXPECT_RET(FUNCTION, EXPECT)   _is_expect_ret((FUNCTION),(EXPECT),__FILE__,#FUNCTION,__LINE__)

#define LOGGER_ASSERT_STDP_RET_AND_RETURN(REQUEST, EXPECT, RET_VALUE_IF_ERROR)  if(!API_IS_EXPECT_RET((REQUEST), (EXPECT))) return (RET_VALUE_IF_ERROR);

#define LOGGER_ASSERT_EXPRESSION(expression)	_is_expect_ret((expression),(1==1),__FILE__,#expression,__LINE__)

#define LOGGER_TRACE(...)   \
        do{LOGGER_DEBUG("@FILE=%s,line=%ld,fn=%s",(const char*)__FILE__,(long)__LINE__,(const char*)__FUNCTION__);\
           LOGGER_DEBUG(__VA_ARGS__);   \
        }while(0)

#define LOGGER_WARN_IT(...)  \
        do{LOGGER_ERROR("@FILE=%s,line=%ld,fn=%s",(const char*)__FILE__,(long)__LINE__,(const char*)__FUNCTION__);\
           LOGGER_ERROR(__VA_ARGS__);   \
        }while(0)

#endif

