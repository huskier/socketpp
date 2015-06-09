#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <QString>
using namespace std;

typedef union{
    float f;
    char c[4];
}FLOAT_CONV;

typedef union{
    double f;
    char c[8];
}DOUBLE_CONV;

std::string int2hex(int val);
bool is_big_enidan();
float htonf(float data);
float ntohf(float data);
double htond(double data);

string timestamp();

/*
template<class out_type,class in_value>
out_type convert_to(const in_value & t)
{
    stringstream stream;
    stream<<t;//向流中传值
    out_type result;//这里存储转换结果
    stream>>result;//向result中写入值
    return result;
}
*/

template <typename T>
T sign(T t)
{

    return (t < 0) ? T(-1) : T(1);

    /*
    if( t == 0 )
        return T(0);
    else
        return (t < 0) ? T(-1) : T(1);
    */
}

#endif // UTIL_H
