#include <vector>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <chrono>
#include <thread>
#include "Util.h"

std::string TimeToDate(time_t t)
{
	struct tm *ptm = localtime(&t);
	char buf[32];
	memset(buf, '\0', sizeof(buf));
	strftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", ptm);
	std::string str = buf;
	return str;
}

time_t DateToTime(const std::string &date)
{
	struct tm stm;
	int year, month, day, hour, minute, second;
	sscanf(date.c_str(), "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	stm.tm_year = year - 1900;
	stm.tm_mon = month - 1;
	stm.tm_mday = day;
	stm.tm_hour = hour;
	stm.tm_min = minute;
	stm.tm_sec = second;
	stm.tm_isdst = 0;

	time_t t = mktime(&stm);
	return t;
}

bool IsFloatEqual(float a, float b)
{
	return ::fabs(a - b) < 0.0001;
}

std::string StringFormat(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int nSize = std::vsnprintf(nullptr, 0, fmt, ap);
	std::vector<char> buff((nSize + 1), '\0');
	va_start(ap, fmt);
	std::vsnprintf(buff.data(), buff.size(), fmt, ap);
	va_end(ap);
	return buff.data();
}

void MySnprintf(char* buffer, size_t count, const char* format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	int nchars = tvsnprintf((char*)buffer, count, format, argptr);
	va_end(argptr);

	if (nchars == count)
	{
		buffer[count - 1] = '\0';
	}
}

time_t GetTimeSeconds()
{
	auto now = std::chrono::system_clock::now();
	auto now_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
	return (time_t)now_epoch.count();
}
time_t GetTimeMs()
{
	auto now = std::chrono::system_clock::now();
	auto now_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
	return (time_t)now_epoch.count();
}

time_t GetTimeMMs()
{
	auto now = std::chrono::system_clock::now();
	auto now_epoch = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
	return (time_t)now_epoch.count();
}

std::string &StringTrim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

int GetDigitDate(time_t t)
{
	struct tm *ptm = localtime(&t);
	return (ptm->tm_year + 1900) * 10000 + (ptm->tm_mon + 1) * 100 + ptm->tm_mday;
}

#ifdef __LINUX__
#include <unistd.h>
void MySleep(int t)
{
	usleep(t * 1000);
}
#else
#include <windows.h>
void MySleep(int t)
{
	Sleep(t);
}
#endif

//std::thread::id MyThreadID()
//{
//	return std::this_thread::get_id();
//}