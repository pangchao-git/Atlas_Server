/*
 * @Description  : network_utils
 * @Version      : 1.0.0
 * @Author       : mdf
 * @Date         : 2020-05-07 10:27:25
 * @LastEditors  : mdf
 * @LastEditTime : 2020-05-09 16:34:48
 */
#include "network_utils.hpp"

namespace cfx {

    HostInetInfo NetworkUtils::queryHostInetInfo() {
        io_service ios;
        tcp::resolver resolver(ios);
        tcp::resolver::query query(host_name(), "");
        return resolver.resolve(query);
    }

    std::string NetworkUtils::hostIP(unsigned short family) {
        auto hostInetInfo = queryHostInetInfo();
        tcp::resolver::iterator end;
        while(hostInetInfo != end) {
            tcp::endpoint ep = *hostInetInfo++;
            sockaddr sa = *ep.data();
            if (sa.sa_family == family) {
                return ep.address().to_string();
            }
        }
        return nullptr;
    }
    std::string NetworkUtils::hostIPbyEthName(const char *eth_name) {
        int sock;
        struct sockaddr_in sin;
        struct ifreq ifr;
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1) {
            perror("socket");
            return nullptr;
        }
    
        strncpy(ifr.ifr_name, eth_name, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = 0;
        if (ioctl(sock, SIOCGIFADDR, &ifr) < 0) {
            perror("ioctl");
            return nullptr;
        }
    
        memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
        return inet_ntoa(sin.sin_addr);
    }   
}
