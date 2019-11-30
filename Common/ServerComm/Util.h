#pragma once

#include <time.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "ServerComm/CommonDef.h"

#define PI 3.14159265f

bool IsFloatEqual(float a, float b);
std::string StringFormat(const char *fmt, ...);
void MySnprintf(char* buffer, size_t count, const char* format, ...);

std::string TimeToDate(time_t t);
time_t DateToTime(const std::string &date);
int GetDigitDate(time_t t);
time_t GetTimeSeconds();
time_t GetTimeMs();
time_t GetTimeMMs();

std::string &StringTrim(std::string &s);

template <typename T>
T StringToNum(const std::string &str)
{
    std::istringstream iss(str);
    T num;
    iss >> num;
    return num;
}

template <typename T>
class AutoDelete
{
public:
    AutoDelete(T *t) : m_t(t) {}
    ~AutoDelete()
    {
        if (m_t != nullptr)
        {
            delete m_t;
            m_t = nullptr;
        }
    }
private:
    T *m_t;
};

void MySleep(int t);
//std::thread::id MyThreadID();
