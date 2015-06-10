
#include "udp_async_server.h"

#include "glog/logging.h"
#include "util.h"
#include <string.h>
#include <thread>
#include <csignal>

#include "IL_global.h"

namespace socketpp{
namespace udp{

    async_server::async_server(boost::asio::io_service& io_service)
        : socket_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 2001)),
          sending_signal(io_service)
    {
        p_io_service = &io_service;

        boost::asio::socket_base::reuse_address option(true);
        socket_.set_option(option);

        std::unique_ptr<condition_variable> cv_ptr;
        for(unsigned short i = 0x0000; i<0xFFFF; i++)
        {
            cv_ptr = std::unique_ptr<condition_variable>(new condition_variable());
            cmd_finished_cond_map[i] = std::move(cv_ptr);
        }

        dest_endpoint = boost::asio::ip::udp::endpoint(
                    boost::asio::ip::address::from_string("192.168.14.124"), 2001);

        sending_signal.add(SENDING_SIGNAL);

    }

    void async_server::set_dest_ip_port(std::string ip_addr, unsigned short port_num)
    {
        dest_endpoint = boost::asio::ip::udp::endpoint(
                    boost::asio::ip::address::from_string(ip_addr), port_num);
    }

    void async_server::run_io_service()
    {
        send();
        receive();

        (*p_io_service).run();
    }

    void async_server::start_send_recv_thread()
    {
        send_recv_thread = new std::thread(std::mem_fn(&async_server::run_io_service),this);
    }

    void async_server::stop_send_recv_thread()
    {
        (*p_io_service).stop();
    }

    void async_server::send()
    {
        sending_signal.async_wait(
                  boost::bind(&async_server::handle_sending_signal, this,
                              boost::asio::placeholders::error,
                              boost::asio::placeholders::signal_number));
    }

    void async_server::handle_sending_signal(const boost::system::error_code &error, int signal_number)
    {
        //LOG(INFO)<<"IN handle sending signal......";

        //std::unique_lock<std::mutex> lck(send_packet_mutex);   //not needed....

        if(!send_queue.empty())
        {
            send_packet = send_queue.top();    //取最高优先级的队列元素。。。。。。
            send_queue.pop();                  //取出元素后，删除队列头的元素。。。。。。

            memset(send_buf.data(), 0, sizeof(send_buf));

            //copy head info into send_buf...........
            memcpy(send_buf.data(), &send_packet, sizeof(NET_PACKET_HEAD_STRUCT));

            std::string head_str = std::string(send_buf.data(),sizeof(NET_PACKET_HEAD_STRUCT));

            boost::shared_ptr<std::string> message(new std::string(head_str));

            socket_.async_send_to(boost::asio::buffer(*message), dest_endpoint,
                boost::bind(&async_server::handle_send_head, this, message,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
        }
    }

    void async_server::handle_send_head(boost::shared_ptr<string> msg, const boost::system::error_code & error, size_t bytes_sent)
    {
        //std::unique_lock<std::mutex> lck(send_packet_mutex);   //not needed....

        if(!error)
        {
            //LOG(INFO)<<"IN handle_send_head......sent head data bytes : "<<bytes_sent;

            LOG(INFO)<<"cmd = "<<int2hex(ntohs(recv_packet.head_union.head.cmd))<<"; send successful (head)......";

            int data_len = ntohs(send_packet.head_union.head.total_length) -12;

            //LOG(INFO)<<"data_len = "<<data_len;

            if(data_len > 0)
            {
                //LOG(INFO)<<"IN handle_send_head......data_len > 0";

                memset(send_buf.data(), 0, sizeof(send_buf));
                memcpy(send_buf.data(), send_packet.p_data, data_len);
                std::string data_str = std::string(send_buf.data(),data_len);

                boost::shared_ptr<std::string> message(new std::string(data_str));

                socket_.async_send_to(boost::asio::buffer(*message),dest_endpoint,
                    boost::bind(&async_server::handle_send_data, this, message,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            }
            else
            {
                LOG(INFO)<<"Before another send()......";
                send();
            }
        }
    }

    void async_server::handle_send_data(boost::shared_ptr<string> msg, const boost::system::error_code & error, size_t bytes_sent)
    {
        if(!error)
        {
            LOG(INFO)<<"cmd = "<<int2hex(ntohs(recv_packet.head_union.head.cmd))<<"; send successful (data)......";

            //LOG(INFO)<<"IN handle_send_data......send data bytes = "<<bytes_sent;
            send();
        }
    }

    void async_server::receive()
    {
        LOG(INFO)<<"Begin async receive....";
        socket_.async_receive_from(
            boost::asio::buffer(recv_buf,NET_PACKET_HEAD_LEN),sender_endpoint,
            boost::bind(&async_server::handle_receive_head,this,
              boost::asio::placeholders::error(),
              boost::asio::placeholders::bytes_transferred()));
    }

    void async_server::handle_receive_head(const boost::system::error_code & error,
                                        std::size_t bytes_received /*bytes_transferred*/)
    {
        //LOG(INFO)<<"[*********]Before std::unique_lock<std::mutex> lck(cmd_finished_mtx); IN handle_receive_head......";
        std::unique_lock<std::mutex> lck(recv_packet_mutex);
        //LOG(INFO)<<"[*********]After std::unique_lock<std::mutex> lck(cmd_finished_mtx); IN handle_receive_head......";

        if(!error)
        {
            //LOG(INFO)<<"IN handle_receive_head......";

            memset(&recv_packet.head_union.head, 0, sizeof(recv_packet.head_union.head));
            memcpy(&recv_packet.head_union.head, recv_buf.data(), sizeof(recv_packet.head_union.head));
            LOG(INFO)<<"cmd = "<<int2hex(ntohs(recv_packet.head_union.head.cmd))<<"; receive successful (head)......";

            recv_packet.head_union.head.cmd = ntohs(recv_packet.head_union.head.cmd);
            recv_packet.head_union.head.total_length = ntohs(recv_packet.head_union.head.total_length);
            recv_packet.head_union.head.cnt = ntohs(recv_packet.head_union.head.cnt);
            recv_packet.head_union.head.check = ntohs(recv_packet.head_union.head.check);
            recv_packet.head_union.head.priority = ntohs(recv_packet.head_union.head.priority);
            recv_packet.head_union.head.status = ntohs(recv_packet.head_union.head.status);

            //if total_length > 1024......, deal with it...... //receive data
            ushort data_len = recv_packet.head_union.head.total_length - NET_PACKET_HEAD_LEN;

            if(data_len == 0)
            {
                recv_packet.p_data = NULL;

                (*cmd_finished_cond_map[recv_packet.head_union.head.cmd]).notify_one();

                receive();
            }
            else if(data_len >0)
            {
                memset(recv_buf.data(), 0, BUFF_SIZE);
                //LOG(INFO)<<"Before ::recv(data).....";

                socket_.async_receive_from(
                    boost::asio::buffer(recv_buf,data_len),sender_endpoint,
                    boost::bind(&async_server::handle_receive_data,this,
                      boost::asio::placeholders::error(),
                      boost::asio::placeholders::bytes_transferred()));
            }
        }
    }

    void async_server::handle_receive_data(const boost::system::error_code & error,
                                        std::size_t bytes_received/*bytes_transferred*/)
    {
        //LOG(INFO)<<"before std::unique_lock<std::mutex> lck(cmd_finished_mtx); IN handle_receive_data......";
        std::unique_lock<std::mutex> lck(recv_packet_mutex);
        //LOG(INFO)<<"after std::unique_lock<std::mutex> lck(cmd_finished_mtx); IN handle_receive_data......";

        if(!error)
        {
            //LOG(INFO)<<"IN handle_receive_data...... received byters = "<<bytes_received;

            ushort data_len = recv_packet.head_union.head.total_length - NET_PACKET_HEAD_LEN;

            if(data_len != bytes_received)
            {
                LOG(INFO)<<"data_len != bytes_received......";
            }

            recv_packet.p_data = new char[data_len];
            memcpy(recv_packet.p_data, recv_buf.data(), data_len);

            //usleep(10000);    //delay 10ms to ensure entering timewait... before broadcasting.......

            //网络上接收的数据已经准备好，保存在结构体变量recv_packet中，此处发出广播
            (*cmd_finished_cond_map[recv_packet.head_union.head.cmd]).notify_one();

            receive();
        }
    }

    void async_server::pack_post(NET_PACKET_STRUCT net_pack)
    {
        net_pack.head_union.head.cmd = htons(net_pack.head_union.head.cmd);
        net_pack.head_union.head.total_length = htons(net_pack.head_union.head.total_length);
        net_pack.head_union.head.cnt = htons(net_pack.head_union.head.cnt);
        net_pack.head_union.head.check = htons(net_pack.head_union.head.check);
        net_pack.head_union.head.priority = htons(net_pack.head_union.head.priority);
        net_pack.head_union.head.status = htons(net_pack.head_union.head.status);

        send_queue.push(net_pack);

        std::raise(SENDING_SIGNAL);
        //LOG(INFO)<<"After std::raise(SENDING_SIGNAL)......";
    }

    STATUS async_server::wait_data(const int cmd, const time_t wait_seconds, NET_PACKET_STRUCT &net_pack)
    {
        try
        {
            std::unique_lock<std::mutex> lck(recv_packet_mutex);

            //LOG(INFO)<<"IN the beginning of UDPServer::wait_data... cmd = "<<int2hex(cmd);

            std::cv_status status = (*cmd_finished_cond_map[cmd]).wait_for(lck,std::chrono::seconds(wait_seconds));

            if(status == std::cv_status::timeout)
            {
                LOG(INFO)<<"wait_for 2s timeout......";
                return ETIMEDOUT;
            }

            memcpy(&net_pack, &recv_packet, sizeof(NET_PACKET_STRUCT));

            //LOG(INFO)<<"[###########] I am sleeping for 8 seconds...... cmd = "<<int2hex(cmd);
            //sleep(8);

            int data_len = recv_packet.head_union.head.total_length - NET_PACKET_HEAD_LEN;
            //LOG(INFO)<<"data_len = "<<data_len;
            if(data_len == 0)
            {
                net_pack.p_data = NULL;
            }
            else if(data_len > 0)
            {
                if(recv_packet.p_data != NULL)
                {
                    net_pack.p_data = new char[data_len];
                    memcpy(net_pack.p_data, recv_packet.p_data, data_len);
                    //LOG(INFO)<<"before delete[] gNet::recv_packet.p_data "<<"gNet.cnt = "<<gNet::recv_packet.head_union.head.cnt<<" local cnt = "<<cnt;
                    delete[] recv_packet.p_data;
                    recv_packet.p_data = NULL;
                    //LOG(INFO)<<"after delete[] gNet::recv_packet.p_data"<<" cnt = "<<cnt;
                }
                else
                {
                    return -1;
                }
            }

            //LOG(INFO)<<"IN the end of UDPServer::wait_data... cmd = "<<int2hex(cmd);

            return OK;
        }
        catch(const std::exception& ex)
        {
            LOG(INFO)<<"std::exception = "<<ex.what();
        }
        catch(...)
        {
            LOG(INFO)<<"Something wrong in wait_data_from_net(...)";
        }
    }
}  // namespace udp
} //namespace socketpp
