#include "mainwindow.h"
#include <QApplication>
#include "IL_global.h"

#include <thread>

#include "glog/logging.h"

#include <vips/vips8>

#include <functional>
#include <type_traits>

using namespace vips;

int main(int argc, char *argv[])
{
    // logging system ......
    google::InitGoogleLogging("IL_host");
    //google::SetLogDestination(google::INFO, "mylog_");
    google::SetStderrLogging(google::INFO);

    DLOG(INFO)<<"Begin logging......";

    UDTUpDown _udt_;

    if(vips_init(argv[0]))
        LOG(INFO)<<"vips_init(arg0) error occured....";

    std::cout << std::boolalpha;


    std::cout<<"ggNet::io_service is_move_constructible :"<<std::is_move_constructible<boost::asio::io_service>::value<<std::endl;

    ggNet::vx_Net.set_dest_ip_port("192.168.14.124",2001);
    ggNet::vx_Net.start_send_recv_thread();

    ggNet::udt_Net.set_dest_ip_port("192.168.14.124",4567);
    ggNet::udt_Net.connect();
    ggNet::udt_Net.start_send_recv_thread();


    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
