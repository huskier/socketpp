
#ifndef _NET_STRUCT_H
#define _NET_STRUCT_H

#define NET_PACKET_HEAD_LEN sizeof(NET_PACKET_HEAD_STRUCT)
#define UDT_PACKET_HEAD_LEN sizeof(UDT_PACKET_HEAD_STRUCT)

//UDP and TCP related structs......
typedef struct
{
    uint16_t cmd;              //命令号
    uint16_t total_length;     //
    uint16_t cnt;              //建议交流去掉这个变量名字，如果实在是要保留的话，建议将此变量名字改为：data_group_cnt;
    uint16_t check;
    uint16_t priority;
    uint16_t status;
} NET_PACKET_HEAD_STRUCT;

typedef union _NET_PACKET_HEAD_UNION
{
    NET_PACKET_HEAD_STRUCT  head;
    unsigned char   ch[NET_PACKET_HEAD_LEN];
}NET_PACKET_HEAD_UNION;

typedef struct
{
    NET_PACKET_HEAD_UNION head_union;
    char *p_data;
}NET_PACKET_STRUCT, *PNET_PACKET_STRUCT;

typedef struct
{
    char ip_addr[50];
    int port;
    int socket_fd;
}NET_PARAM_STRUCT;

//此比较应该是升序排列吧，写好了程序测试下了。。。。。。
class compare_NET_PACKET_STRUCT {
public:
    bool operator()(NET_PACKET_STRUCT& n1, NET_PACKET_STRUCT& n2)
    {
       if(n1.head_union.head.priority < n2.head_union.head.priority)
           return true;
       else
           return false;
    }
};

//UDT related structs......
typedef struct
{
    uint32_t cmd;
    uint32_t total_length;
    uint32_t cnt;
    uint32_t check;
    uint32_t priority;
    uint32_t status;
} UDT_PACKET_HEAD_STRUCT;

typedef union _UDT_PACKET_HEAD_UNION
{
    UDT_PACKET_HEAD_STRUCT  head;
    unsigned char   ch[UDT_PACKET_HEAD_LEN];
}UDT_PACKET_HEAD_UNION;

typedef struct
{
    UDT_PACKET_HEAD_UNION head_union;
    char *p_data;
}UDT_PACKET_STRUCT, *PUDT_PACKET_STRUCT;

typedef struct
{
    char ip_addr[50];
    int port;
    int socket_fd;
}UDT_PARAM_STRUCT;

//此比较应该是升序排列吧，写好了程序测试下了。。。。。。
class compare_UDT_PACKET_STRUCT {
public:
    bool operator()(UDT_PACKET_STRUCT& n1, UDT_PACKET_STRUCT& n2)
    {
       if(n1.head_union.head.priority < n2.head_union.head.priority)
           return true;
       else
           return false;
    }
};

#endif /* _NET_STRUCT_H */
