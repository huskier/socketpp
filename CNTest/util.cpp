#include "util.h"
#include <cstring>
#include <cstdio>
#include <string>
#include <QDateTime>
#include <glog/logging.h>
#include <QDebug>

using namespace std;

//network byte order = big_enidan
bool is_big_enidan()
{  
    union data
    {
        int a;
        char c;
    }test_data;
    test_data.a = 0x00000001;

    if(test_data.c == 0x01)
    {
        return false;
    }
    else
    {
        return true;
    }
}

std::string int2hex(int val)
{
    char ch[10];
    std::string str = "0x";
    memset(ch, 0, 10);
    sprintf(ch, "%04X", val);
    str.append(ch);
    return str;
}

//converting the float data from host byte order to network byte order
float htonf(float data)
{
    //if(!is_big_enidan())
    if(__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        FLOAT_CONV d1, d2;

        d1.f = data;

        d2.c[0] = d1.c[3];
        d2.c[1] = d1.c[2];
        d2.c[2] = d1.c[1];
        d2.c[3] = d1.c[0];

        return d2.f;
    }
    else
    {
        return data;
    }
}

//converting the float data from network byte order to host byte order
float ntohf(float data)
{
    //if(!is_big_enidan())
    if(__BYTE_ORDER == __LITTLE_ENDIAN)
    {
        FLOAT_CONV d1, d2;

        d1.f = data;

        d2.c[0] = d1.c[3];
        d2.c[1] = d1.c[2];
        d2.c[2] = d1.c[1];
        d2.c[3] = d1.c[0];

        return d2.f;
    }
    else
    {
        return data;
    }
}

double htond(double data)
{
    //if(!is_big_enidan())
    if(true)
    {
        DOUBLE_CONV d1, d2;

        d1.f = data;

        d2.c[0] = d1.c[7];
        d2.c[1] = d1.c[6];
        d2.c[2] = d1.c[5];
        d2.c[3] = d1.c[4];
        d2.c[4] = d1.c[3];
        d2.c[5] = d1.c[2];
        d2.c[6] = d1.c[1];
        d2.c[7] = d1.c[0];

        return d2.f;
    }
    else
    {
        return data;
    }
}

string timestamp()
{
    time_t ltime;
    struct tm *Tm;

    ltime=time(NULL);
    Tm=localtime(&ltime);

    char buffer[256];
    int len;
    len = sprintf (buffer,"%04d_%02d%02d_%02d%02d%02d",
                           Tm->tm_year + 1900,
                           Tm->tm_mon + 1,
                           Tm->tm_mday,
                           Tm->tm_hour,
                           Tm->tm_min,
                           Tm->tm_sec);

    string timestamp_str;
    timestamp_str.assign(buffer, len);

    return timestamp_str;
}

