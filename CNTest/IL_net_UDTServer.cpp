#include "IL_net_UDTServer.h"
#include <arpa/inet.h>

#include <glog/logging.h>

#include "util.h"

UDTServer::UDTServer()
{
        std::unique_ptr<std::condition_variable> cv_ptr;

        for(unsigned short i = 0x0000; i<0xFFFF; i++)
        {
            cv_ptr = std::unique_ptr<std::condition_variable>(new std::condition_variable());
            cmd_finished_cond_map[i] = std::move(cv_ptr);
        }
}

void UDTServer::set_dest_ip_port(std::string ip_addr, unsigned short port_num)
{
    dest_sockaddr.sin_family = PF_INET;
    dest_sockaddr.sin_addr.s_addr = inet_addr(ip_addr.c_str());
    dest_sockaddr.sin_port = htons(port_num);
}

void UDTServer::connect()
{
    socketfd = UDT::socket(PF_INET, SOCK_STREAM, 0);

    if(socketfd == -1)
    {
        LOG(INFO)<<"create socket error......";
    }
    else
    {
        LOG(INFO)<<"create socket OK......";
    }

    // connect to the server, implict bind
    if(UDT::ERROR == UDT::connect(socketfd, (struct sockaddr* )&dest_sockaddr, sizeof(dest_sockaddr)))
    {
       LOG(INFO) << "connect: " << UDT::getlasterror().getErrorMessage() << endl;
       return ;
    }
    else
    {
        LOG(INFO)<<"Connect successful......!";
    }
}

void UDTServer::start_send_recv_thread()
{
    sending_thread = new std::thread(std::mem_fn(&UDTServer::send),this);
    recving_thread = new std::thread(std::mem_fn(&UDTServer::receive),this);
}

void UDTServer::stop_send_recv_thread()
{
    //firstly change  UDT_RCVSYN to false, and let recv return immidelilly.....Then.......
    //setting exit_variable to.......
}

void UDTServer::pack_post(uint32_t cmd, uint32_t total_length, uint32_t cnt, uint32_t check, uint32_t priority, uint32_t status, char *p_data)
{
    UDT_PACKET_STRUCT udt_pack;

    udt_pack.head_union.head.cmd = htonl(cmd);
    udt_pack.head_union.head.total_length = htonl(total_length);
    udt_pack.head_union.head.cnt = htonl(cnt);
    udt_pack.head_union.head.check = htonl(check);
    udt_pack.head_union.head.priority = htonl(priority);
    udt_pack.head_union.head.status = htonl(status);
    udt_pack.p_data = p_data;

    send_queue.push(udt_pack);

    sending_cv.notify_one();
}

void UDTServer::pack_post(UDT_PACKET_STRUCT udt_pack)
{
    udt_pack.head_union.head.cmd = htons(udt_pack.head_union.head.cmd);
    udt_pack.head_union.head.total_length = htons(udt_pack.head_union.head.total_length);
    udt_pack.head_union.head.cnt = htons(udt_pack.head_union.head.cnt);
    udt_pack.head_union.head.check = htons(udt_pack.head_union.head.check);
    udt_pack.head_union.head.priority = htons(udt_pack.head_union.head.priority);
    udt_pack.head_union.head.status = htons(udt_pack.head_union.head.status);

    send_queue.push(udt_pack);

    sending_cv.notify_one();
}

void UDTServer::send()
{
    int packet_size;
    int ssize;
    int ss;

    while(true)
    {
        std::unique_lock<std::mutex> lck(sending_mutex);
        if(send_queue.empty())
        {
            sending_cv.wait(lck);
        }
        else
        {
            /*********************************************************************/
            send_packet = send_queue.top();    //取最高优先级的队列元素。。。。。。
            send_queue.pop();                  //取出元素后，删除队列头的元素。。。。。。
            /*********************************************************************/

            int data_len = ntohl(send_packet.head_union.head.total_length) - UDT_PACKET_HEAD_LEN;
            //LOG(INFO)<<"data_len is:"<<data_len<<".........................";

            memset(send_buf, 0, sizeof(send_buf));
            //copy head info into send_buf...........
            memcpy(send_buf, &send_packet, sizeof(UDT_PACKET_HEAD_STRUCT));

            //sending head info
            packet_size = sizeof(UDT_PACKET_HEAD_STRUCT);
            ssize = 0;
            while (ssize < packet_size)
            {
              if (UDT::ERROR == (ss = UDT::send(socketfd, send_buf + ssize, packet_size - ssize, 0)))
              {
                LOG(INFO) << "send:" << UDT::getlasterror().getErrorMessage() << endl;
                break;
              }

              ssize += ss;
            }

            if(ssize == packet_size)
            {
                LOG(INFO)<<"cmd = "<<int2hex(ntohl(send_packet.head_union.head.cmd))<<"; send head successful......";
            }
            else
            {
                LOG(ERROR)<<"send function error......";
                is_connected = false;
                break;
            }

            usleep(100);

            if (data_len != 0)
            {
                //copy data into send_buf................
                memcpy(send_buf + sizeof(UDT_PACKET_HEAD_STRUCT), send_packet.p_data, data_len);

                //int bytes_sent1 = ::sendto(main_socket.socket_fd, send_buf + sizeof(NET_PACKET_HEAD_STRUCT), data_len, 0,(struct sockaddr*)&main_sockaddr_UDP,sizeof(main_sockaddr_UDP));

                //sending data info
                packet_size = data_len;
                ssize = 0;
                while (ssize < packet_size)
                {
                  if (UDT::ERROR == (ss = UDT::send(socketfd, send_buf + sizeof(UDT_PACKET_HEAD_STRUCT) + ssize, packet_size - ssize, 0)))
                  {
                    LOG(INFO) << "send:" << UDT::getlasterror().getErrorMessage() << endl;
                    break;
                  }

                  ssize += ss;
                }

                //LOG(INFO) << "After ::send(...) function......";
                //LOG(INFO)<<"data_len is "<<data_len;

                if(ssize == packet_size)
                {
                    LOG(INFO)<<"cmd = "<<int2hex(ntohl(send_packet.head_union.head.cmd))<<"; send data successful......";
                    delete[] send_packet.p_data;
                }
                else
                {
                    LOG(ERROR)<<"send function error......";
                    is_connected = false;
                    break;
                }
            }
        }
    }
}

void UDTServer::receive()
{
    int packet_size;
    int rsize;
    int rs;

    while(true)
    {
            memset(recv_buf, 0, BUFF_SIZE);

            //receiving head info
            rsize = 0;
            packet_size = UDT_PACKET_HEAD_LEN;
            while (rsize < packet_size)
            {
               try
               {
                   if (UDT::ERROR == (rs = UDT::recv(socketfd, recv_buf + rsize, packet_size - rsize, 0)))
                   {
                      LOG(INFO) << "recv:" << UDT::getlasterror().getErrorMessage();
                      break;
                   }
               }
               catch(...)
               {
                   LOG(INFO)<<"Something was wrong in recv......";
               }

               rsize += rs;
            }

            //加锁, 开始保护结构体变量recv_packet
            std::unique_lock<std::mutex> lck(cmd_finished_mtx);

            if(rsize == packet_size)
            {
                memset(&recv_packet.head_union.head, 0, sizeof(recv_packet.head_union.head));
                memcpy(&recv_packet.head_union.head, recv_buf, sizeof(recv_packet.head_union.head));
                LOG(INFO)<<"cmd = "<<int2hex(ntohl(recv_packet.head_union.head.cmd))<<"; receive successful (head)......";
            }
            else
            {
                LOG(ERROR)<<"recv() fuction error (receiving head)......";
                break;
            }

            recv_packet.head_union.head.cmd = ntohl(recv_packet.head_union.head.cmd);
            recv_packet.head_union.head.total_length = ntohl(recv_packet.head_union.head.total_length);
            recv_packet.head_union.head.cnt = ntohl(recv_packet.head_union.head.cnt);
            recv_packet.head_union.head.check = ntohl(recv_packet.head_union.head.check);
            recv_packet.head_union.head.priority = ntohl(recv_packet.head_union.head.priority);
            recv_packet.head_union.head.status = ntohl(recv_packet.head_union.head.status);

            //receive data
            int data_len = recv_packet.head_union.head.total_length - UDT_PACKET_HEAD_LEN;

            if(data_len == 0)
            {
                recv_packet.p_data = NULL;
            }
            else if(data_len >0)
            {
                memset(recv_buf, 0, BUFF_SIZE);
                //LOG(INFO)<<"Before ::recv(data).....";

                //receiving data...
                rsize = 0;
                packet_size = data_len;
                while (rsize < packet_size)
                {
                   //int rcv_size;
                   //int var_size = sizeof(int);
                   //UDT::getsockopt(recver, 0, UDT_RCVDATA, &rcv_size, &var_size);
                   if (UDT::ERROR == (rs = UDT::recv(socketfd, recv_buf + rsize, packet_size - rsize, 0)))
                   {
                      LOG(INFO) << "recv:" << UDT::getlasterror().getErrorMessage();
                      break;
                   }

                   //LOG(INFO) << "received size for this time is : "<< rs;

                   rsize += rs;
                }

                if(rsize == packet_size)
                {
                    recv_packet.p_data = new char[data_len];
                    memcpy(recv_packet.p_data, recv_buf, data_len);
                }
                else
                {
                    LOG(ERROR)<<"recv() fuction error (receiving data)......";
                    break;
                }
            }

            //释放互斥锁cmd_finished_mutex
            //LOG(INFO) << "Before pthread_mutex_unlock ...... ";
            //pthread_mutex_unlock(&cmd_finished_mutex);
            //LOG(INFO) << "After pthread_mutex_unlock ...... ";

            /*********************此段文字有待于进一步斟酌*******************************************
             为了确保pthread_cond_timewait(...)函数先于pthread_cond_broadcast(...)函数执行
            此处如果用互斥锁或者是在单元函数中为pthread_cond_timewait(...)函数单独开辟线程，则程序变得非常复杂，不容易被理解。
            同时，也不能从根本上解决问题。比如，如果是为pthread_cond_timewait(...)函数单独开辟线程，并将开辟线程代码放在
            package_post(...)函数之前，会有如下问题：
            1. 互斥锁或许还是先被receive线程锁住，等receive线程释放锁之后
            2. 若在线程开辟后再加入延迟，以确保互斥锁后被receive线程锁住，则不如直接在receive的parse中加入延迟后进行广播，
               来的容易理解和方便。
            ***************************************************************************************/
            usleep(10000);    //delay 10ms to ensure entering timewait... before broadcasting.......

            //网络上接收的数据已经准备好，保存在结构体变量recv_packet中，此处发出广播
            //LOG(INFO) << "Before pthread_cond_broadcast ...... ";
            //pthread_cond_broadcast(&gVar::cmd_finished_cond);
            //pthread_cond_broadcast(gVar::cmd_finished_cond_QMap[recv_packet.head_union.head.cmd]);
            //LOG(INFO) << "After pthread_cond_broadcast ...... ";

            (*cmd_finished_cond_map[recv_packet.head_union.head.cmd]).notify_one();

        }
}

STATUS UDTServer::wait_data(const int cmd, const time_t wait_seconds, UDT_PACKET_STRUCT & udt_pack, int cnt)
{
    std::unique_lock<std::mutex> lck(cmd_finished_mtx);

    LOG(INFO)<<"wait_data_from_net ... cmd = "<<int2hex(cmd);

    std::cv_status status = (*cmd_finished_cond_map[cmd]).wait_for(lck,std::chrono::seconds(wait_seconds));

    if(status == std::cv_status::timeout)
    {
        LOG(INFO)<<"wait_for 2s timeout......";

        return ETIMEDOUT;
    }

    memcpy(&udt_pack, &recv_packet, sizeof(UDT_PACKET_STRUCT));

    int data_len = recv_packet.head_union.head.total_length - UDT_PACKET_HEAD_LEN;

    //LOG(INFO)<<"data_len is "<<data_len;

    if(data_len == 0)
    {
        udt_pack.p_data = NULL;
    }
    else if(data_len > 0)
    {

        udt_pack.p_data = new char[data_len];
        memcpy(udt_pack.p_data, recv_packet.p_data, data_len);

        //LOG(INFO)<<"Before delete[] recv_packet.p_data...... IN gUDT4PICam.cpp";

        //LOG(INFO)<<"recv_packet.head_union.head.cnt = "<<recv_packet.head_union.head.cnt<<"......IN gUDT4PICam.cpp";
        delete[] recv_packet.p_data;

        //LOG(INFO)<<"After delete[] recv_packet.p_data....... IN gUDT4PICam.cpp";
    }

    return OK;
}
