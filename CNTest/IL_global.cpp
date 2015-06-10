
#include "IL_global.h"
#include "glog/logging.h"
#include "time.h"
#include "util.h"
#include <QApplication>

#include <QFile>

#include <QDebug>

#include <thread>

#include <QDateTime>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

//#include "udp_async_server.h"
//#include "udt_sync_client.h"
#include "socketpp.h"

namespace TestNamespace {
    int variable_int;
}

//UDTServer ggNet::udt_Net;
//socketpp::udt::sync_server ggNet::udt_Net;
socketpp::udt::sync_client ggNet::udt_Net;

boost::asio::io_service ggNet::io_service;

//UDPServer ggNet::vx_Net(ggNet::io_service);

socketpp::udp::async_server ggNet::vx_Net(ggNet::io_service);

timespec timeout(time_t wait_seconds)
{
    timespec curr_time;
    timespec target_time;

    clock_gettime(CLOCK_REALTIME, &curr_time);

    target_time.tv_sec = curr_time.tv_sec + wait_seconds;
    target_time.tv_nsec = curr_time.tv_nsec + 0;

    return target_time;
}
