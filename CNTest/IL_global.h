
#ifndef _IL_GLOBAL_H
#define _IL_GLOBAL_H

#include <string>
#include <iostream>
#include <string.h>

#include <pthread.h>
#include <semaphore.h>

#include "time.h"

#include <iomanip>
#include <iostream>

#include <QVector>

//#include "IL_net_UDPServer.h"
//#include "IL_net_UDTServer.h"

//#define SENDING_SIGNAL 58

#include <condition_variable>
#include <mutex>
#include <map>
#include <memory>

//#include "udp_async_server.h"
//#include "udt_sync_server.h"
//#include "udt_sync_client.h"
#include "socketpp.h"


using namespace std;

timespec timeout(time_t wait_seconds);

struct UDTUpDown{
   UDTUpDown()
   {
      // use this function to initialize the UDT library
      UDT::startup();
   }
   ~UDTUpDown()
   {
      // use this function to release the UDT library
      UDT::cleanup();
   }
};

class ggNet
{
public:
    static boost::asio::io_service io_service;
    //static UDPServer vx_Net;
    static socketpp::udp::async_server vx_Net;

    //static UDTServer udt_Net;
    //static socketpp::udt::sync_server udt_Net;
    static socketpp::udt::sync_client udt_Net;
    //static socketpp::udp::async_server
};

namespace TestNamespace {
    extern int variable_int;
}

#endif /* _IL_GLOBAL_H */
