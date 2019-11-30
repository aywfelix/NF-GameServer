#pragma once

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#ifdef __WINDOWS__
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <syslog.h>
#endif // __WINDOWS__

typedef uint32_t UINT;
typedef int64_t	 TIME;
typedef int32_t  INT;
typedef int64_t	 INT64;
typedef int32_t	 INT32;
typedef uint8_t	 BYTE;

#ifndef DELETE_PTR
#define DELETE_PTR(x) if((x)) { delete (x); (x)=nullptr;}
#endif

#ifndef DELETE_PTR_ARR
#define DELETE_PTR_ARR(x) if((x)) { delete[] (x); (x)=nullptr;}
#endif


#if defined(__WINDOWS__)
#define		tvsnprintf		_vsnprintf
#define		tsnprintf		_snprintf
#define		__PRETTY_FUNCTION__ __FUNCTION__ 
#elif defined(__LINUX__)
#define		tvsnprintf		vsnprintf
#define		tsnprintf		snprintf
#endif

#define TABLE_PATH "../DataTable/"