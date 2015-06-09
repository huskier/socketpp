//4/7/2013
//huskier
//IL_types.h
//
//

#ifndef IL_TYPES_H
#define IL_TYPES_H

/***********类型定义（此处应定义可提供给外部参照的类型定义）***********/
/*typedef自定义的类型不应被重新定义*/
/*数据类型（Linux下）*/
typedef char char_t;
typedef signed char int8_t;//!!!!!!!!!!
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef float float32_t;
typedef double float64_t;
typedef long double float128_t;

typedef int BOOL;       //下位机BOOL的类型为int，占4个bytes，此处将BOOL定义为int是为了兼容下位机的情形
typedef int STATUS;

//typedef int (*func_ptr)(void *);


/*定义错误或返回状态*/
/*定义返回状态*/
#define OK 0
#define STATUS_NORMAL                   0x00
#define STATUS_ERROR                    0x01
#define STATUS_WARNING                  0x02
#define STATUS_INVALID                  0x03
#define STATUS_UNAVAILABLE              0x04
#define STATUS_DISABLE                  0x05

/*网络状态初始化情况*/
#define NET_CONNECT_FAILED      0
#define NET_CONNECT_SUCCESS     1

#define NET_MSG_MAX_SIZE                1024    /*定义最大发送的数据长度*/

/******************文件传输系统宏定义**********************************/
#define FILE_BUF_LEN        1024         /*定义传输文件时每次传送的长度*/

/*定义传输文件时不同信息的命令号CMDnumber*/
#define FILE_CMD_POS       51
#define FILE_ACT_POS       52
#define FILE_ACT_VEL       53
#define FILE_FOL_ERR       54

/*定义文件传输结束时的标志号ack*/
#define FILE_NEOF       '0'
#define FILE_EOF        '1'
/*结构与联合体应使用紧凑型存储方式，否则在使用不同的编译器时可能会产生异常*/

#endif // IL_TYPES_H
