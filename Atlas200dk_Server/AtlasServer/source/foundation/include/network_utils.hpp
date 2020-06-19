/*
 * @Description  : network utils 
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-07 10:27:25
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-09 16:58:34
 */
#pragma once

#include <string>
#include "std_micro_service.hpp"

using namespace boost::asio;
using namespace boost::asio::ip;

namespace cfx {
   
   using HostInetInfo = tcp::resolver::iterator;

   class NetworkUtils {
   private:
      static HostInetInfo queryHostInetInfo();
      static std::string hostIP(unsigned short family);
   public:
      // gets the host IP4 string formatted
      static std::string hostIP4() {
         return hostIP(AF_INET);
      }

      // gets the host IP6 string formatted
      static std::string hostIP6() {

         return hostIP(AF_INET6);
      }
      static std::string hostName() {
         return ip::host_name();
      }
      static std::string hostIPbyEthName(const char *eth_name);
   };      
}
