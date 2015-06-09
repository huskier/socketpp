#ifndef IL_NET_UDPSERVER_H
#define IL_NET_UDPSERVER_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "IL_types.h"
#include "IL_net_struct.h"
#include <array>
#include <boost/shared_ptr.hpp>

#include <queue>

#include <semaphore.h>

#include <thread>

#include <condition_variable>
#include <mutex>
#include <map>
#include <memory>

using boost::asio::ip::udp;

using namespace std;

class UDPServer
{
public:
    UDPServer(boost::asio::io_service& io_service);

public:
    void set_dest_ip_port(std::string ip_addr, unsigned short port_num);
    void start_send_recv_thread();
    void stop_send_recv_thread();

public:
    void pack_post(NET_PACKET_STRUCT net_pack);

    STATUS wait_data(const int cmd, const time_t wait_seconds, NET_PACKET_STRUCT & net_pack);
    //Maybe we can add a async_wait_data(......)......

private:
    void run_io_service();

private:

    void send();
    void receive();

    void handle_sending_signal( const boost::system::error_code& error, int signal_number);

    void handle_receive_head(const boost::system::error_code & error, std::size_t /*bytes_transferred*/);
    void handle_receive_data(const boost::system::error_code & error, std::size_t /*bytes_transferred*/);

    void handle_send_head(boost::shared_ptr<std::string> /*message*/,
                          const boost::system::error_code& /*error*/,
                          std::size_t /*bytes_transferred*/);

    void handle_send_data(boost::shared_ptr<std::string> /*message*/,
                          const boost::system::error_code& /*error*/,
                          std::size_t /*bytes_transferred*/);



private:
    udp::socket socket_;
    udp::endpoint dest_endpoint;
    udp::endpoint sender_endpoint;

    static const int BUFF_SIZE = 1024;
    std::array<char,BUFF_SIZE> recv_buf;
    std::array<char,BUFF_SIZE> send_buf;

    NET_PACKET_STRUCT recv_packet;
    NET_PACKET_STRUCT send_packet;
    priority_queue<NET_PACKET_STRUCT, vector<NET_PACKET_STRUCT>, compare_NET_PACKET_STRUCT> send_queue;

    boost::asio::signal_set sending_signal;

    std::mutex cmd_finished_mtx;
    std::map<ushort,std::unique_ptr<condition_variable> > cmd_finished_cond_map;

private:
    boost::asio::io_service* p_io_service;

    std::thread * send_recv_thread;
};

#endif // IL_NET_UDPSERVER_H
