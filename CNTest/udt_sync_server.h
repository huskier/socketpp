#ifndef UDT_SYNC_SERVER_H
#define UDT_SYNC_SERVER_H

#include "udt/udt.h"
#include "IL_types.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <map>

#include <queue>
#include <vector>

#include "IL_net_struct.h"

namespace socketpp{

    namespace udt{

        class sync_server
        {
        public:
            sync_server();

        public:
            //void set_dest_ip_port(std::string ip_addr, unsigned short port_num);

            //void connect();

            void create_connection();

            void start_send_recv_thread();

            void stop_send_recv_thread();

            void pack_post(uint32_t cmd, uint32_t total_length, uint32_t cnt, uint32_t check, uint32_t priority, uint32_t status, char *p_data);
            void pack_post(UDT_PACKET_STRUCT udt_pack);


            //STATUS wait_data(const int cmd, const time_t wait_seconds, UDT_PACKET_STRUCT & udt_pack, int cnt = 0);
            //Maybe we can add a async_wait_data fuction into the class......

        private:
            void send();
            void receive();

        private:

            UDTSOCKET socketfd;
            struct sockaddr_in dest_sockaddr;

            UDTSOCKET recver;

            std::condition_variable sending_cv;

            std::thread *sending_thread, *recving_thread, *create_connection_thread;

            std::mutex sending_mutex;

            std::mutex cmd_finished_mtx;
            std::map<ushort,std::unique_ptr<std::condition_variable> > cmd_finished_cond_map;

            /*将其作为全局共享变量，网络接收线程首先将数据接收到此变量中，然后发出广播接收信号，
             *之后由发出命令号的单元将此变量所存储到的内容copy到局部变量后，再之后释放互斥锁。
             *在读取或者是写入内容到recv_packet结构体中时，全程由互斥锁cmd_finished_mutex保护。*/
            UDT_PACKET_STRUCT recv_packet;

            static const int BUFF_SIZE = 10000000;

            char recv_buf[BUFF_SIZE];
            char send_buf[BUFF_SIZE];

            bool is_connected;

            UDT_PACKET_STRUCT send_packet;

            std::priority_queue<UDT_PACKET_STRUCT, std::vector<UDT_PACKET_STRUCT>, compare_UDT_PACKET_STRUCT> send_queue;
        };
    }   //namespace udt
}  //namespace socketpp

#endif // UDT_SYNC_SERVER_H
